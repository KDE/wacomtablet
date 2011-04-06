/*
 * Copyright 2010 Jörg Ehrichs <joerg.ehichs@gmx.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "tabletdaemon.h"
#include "devicehandler.h"
#include "wacomadaptor.h"
#include "wacomdeviceadaptor.h"
#include "../version.h"

#include "xdeviceeventnotifier.h"

// KDE includes
#include <KDE/KPluginFactory>
#include <KDE/KAboutData>
#include <KDE/KNotification>
#include <KDE/KLocale>
#include <KDE/KLocalizedString>
#include <KDE/KIconLoader>
#include <KDE/KSharedConfig>
#include <KDE/KActionCollection>
#include <KDE/KAction>
#include <KDE/KDebug>

//Qt includes
#include <QtGui/QX11Info>

// X11 includes
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/XInput.h>
#include <X11/extensions/XInput2.h>

using namespace Wacom;

K_PLUGIN_FACTORY( WacomTabletFactory, registerPlugin<TabletDaemon>(); )
K_EXPORT_PLUGIN( WacomTabletFactory( "wacomtabletdaemon" ) )

namespace Wacom {
/**
  * Private class of the TabletDaemon for the d-pointer
  *
  */
class TabletDaemonPrivate {
public:
    DeviceHandler       *deviceHandler;  /**< Pointer to the tablet device */
    XDeviceEventNotifier *xEventNotifier; /**< X11 Event handler to detect when the tablet is connected/removed */
    KSharedConfig::Ptr  profilesConfig;  /**< Shared pointer to the configuration file that holds all tablet profiles */
    KComponentData      applicationData; /**< Basic application data */
    KIconLoader         *iconLoader;     /**< Simple loader for the notification icon */
    int                 deviceid;        /**< current conencted tablet deviceid. id comes from x11 */
    QString             curProfile;      /**< currently active profile */
    bool                initPhase;       /**< used to suppress the tablet add notification on kded loading. */
    KActionCollection   *actionCollection; /**< Collection of all global actions */
};
}

TabletDaemon::TabletDaemon( QObject *parent, const QVariantList &args )
    : KDEDModule( parent ), d_ptr( new TabletDaemonPrivate )
{
    Q_UNUSED( args );
    Q_D( TabletDaemon );

    KGlobal::locale()->insertCatalog( QLatin1String( "wacomtablet" ) );

    KAboutData about( "wacomtablet", "wacomtablet", ki18n( "Graphic Tablet Configuration daemon" ), kded_version,
                      ki18n( "A Wacom tablet control daemon" ),
                      KAboutData::License_GPL,
                      ki18n( "(c) 2010 Jörg Ehrichs" ),
                      KLocalizedString(),
                      "http://www.etricceline.de" );

    about.addAuthor( ki18n( "Jörg Ehrichs" ), ki18n( "Maintainer" ) , "joerg.ehrichs@gmx.de" );

    d->applicationData = KComponentData( about );
    d->iconLoader = new KIconLoader( d->applicationData );
    d->profilesConfig = KSharedConfig::openConfig( QLatin1String( "tabletprofilesrc" ), KConfig::SimpleConfig );
    d->deviceHandler = new DeviceHandler();

    //DBus connection
    new WacomAdaptor( this );
    new WacomDeviceAdaptor( d->deviceHandler );
    QDBusConnection::sessionBus().registerObject( QLatin1String( "/Tablet" ), this );
    QDBusConnection::sessionBus().registerObject( QLatin1String( "/Device" ), d->deviceHandler );
    QDBusConnection::sessionBus().registerService( QLatin1String( "org.kde.Wacom" ) );


    d->initPhase = true;

    d->xEventNotifier = new XDeviceEventNotifier();

    d->xEventNotifier->start();
    connect( d->xEventNotifier, SIGNAL( deviceAdded( int ) ), this, SLOT( deviceAdded( int ) ) );
    connect( d->xEventNotifier, SIGNAL( deviceRemoved( int ) ), this, SLOT( deviceRemoved( int ) ) );
    connect( d->xEventNotifier, SIGNAL( screenRotated( int ) ), this, SLOT( screenRotated( int ) ) );

    //check for devices on startup
    int deviceid = findTabletDevice();
    if( deviceid != 0 ) {
        deviceAdded( deviceid );
    }

    d->initPhase = false;

    setupActions();
}

TabletDaemon::~TabletDaemon()
{
    this->d_ptr->xEventNotifier->stop();

    QDBusConnection::sessionBus().unregisterService( QLatin1String( "org.kde.Wacom" ) );
    delete this->d_ptr->xEventNotifier;
    delete this->d_ptr->deviceHandler;
    delete this->d_ptr->iconLoader;
    delete this->d_ptr->actionCollection;
    delete this->d_ptr;
}

void TabletDaemon::deviceAdded( int deviceid )
{
    Q_D( TabletDaemon );

    // if we already have a device ... skip this step
    if( d->deviceHandler->isDeviceAvailable() ) {
        return;
    }

    // No tablet available, so reload tablet information
    d->deviceHandler->detectTablet();

    // if we found something notify about it and set the default profile to it
    if( d->deviceHandler->isDeviceAvailable() ) {
        if( !d->initPhase ) {
            KNotification *notification = new KNotification( QLatin1String( "tabletAdded" ) );
            notification->setTitle( i18n( "Tablet added" ) );
            notification->setText( i18n( "New %1 tablet added", d->deviceHandler->deviceName() ) );
            notification->setPixmap( d->iconLoader->loadIcon( QLatin1String( "input-tablet" ), KIconLoader::Panel ) );
            notification->setComponentData( d->applicationData );
            notification->sendEvent();

            delete notification;
        }

        d->deviceid = deviceid;

        emit tabletAdded();

        //get last used profilename
        KSharedConfigPtr config = KSharedConfig::openConfig( QLatin1String( "wacomtablet-kderc" ) );
        KConfigGroup generalGroup( config, "General" );

        QString profileName = generalGroup.readEntry( "lastprofile", QString() );

        if( profileName.isEmpty() ) {
            setProfile( QLatin1String( "default" ) );
        }
        else {
            setProfile( profileName );
        }
    }
}

void TabletDaemon::deviceRemoved( int deviceid )
{
    Q_D( TabletDaemon );
    if( d->deviceHandler->isDeviceAvailable() ) {
        if( d->deviceid == deviceid ) {
            KNotification *notification = new KNotification( QLatin1String( "tabletRemoved" ) );
            notification->setTitle( i18n( "Tablet removed" ) );
            notification->setText( i18n( "Tablet %1 removed", d->deviceHandler->deviceName() ) );
            notification->setComponentData( d->applicationData );
            notification->sendEvent();
            d->deviceHandler->clearDeviceInformation();

            emit tabletRemoved();
            delete notification;
        }
    }
}

bool TabletDaemon::tabletAvailable() const
{
    Q_D( const TabletDaemon );
    return d->deviceHandler->isDeviceAvailable();
}

void TabletDaemon::setProfile( const QString &profile )
{
    Q_D( TabletDaemon );
    d->profilesConfig->reparseConfiguration();
    KConfigGroup deviceGroup = KConfigGroup( d->profilesConfig, d->deviceHandler->deviceName() );
    KConfigGroup profileGroup = KConfigGroup( &deviceGroup, profile );

    if( profileGroup.groupList().isEmpty() ) {
        notifyError( i18n( "Profile <b>%1</b> does not exist", profile ) );
    }
    else {
        d->curProfile = profile;
        d->deviceHandler->applyProfile( &profileGroup );

        emit profileChanged( profile );

        //write as last used profile into the config file
        KSharedConfigPtr config = KSharedConfig::openConfig( QLatin1String( "wacomtablet-kderc" ) );
        KConfigGroup generalGroup( config, "General" );

        generalGroup.writeEntry( "lastprofile", profile );
    }
}

QString TabletDaemon::profile() const
{
    Q_D( const TabletDaemon );
    return d->curProfile;
}

QStringList TabletDaemon::profileList() const
{
    Q_D( const TabletDaemon );

    //get list of all profiles
    KSharedConfig::Ptr profilesConfig = KSharedConfig::openConfig( QLatin1String( "tabletprofilesrc" ), KConfig::SimpleConfig );
    KConfigGroup deviceGroup = KConfigGroup( profilesConfig, d->deviceHandler->deviceName() );

    return deviceGroup.groupList();
}

void TabletDaemon::notifyError( const QString &message ) const
{
    Q_D( const TabletDaemon );
    KNotification *notification = new KNotification( QLatin1String( "tabletError" ) );
    notification->setTitle( i18n( "Graphic Tablet error" ) );
    notification->setText( message );
    notification->setComponentData( d->applicationData );
    notification->sendEvent();

    delete notification;
}

int TabletDaemon::findTabletDevice()
{
    bool deviceFound = false;
    int deviceId = 0;
    int ndevices;
    XDeviceInfo *info = XListInputDevices( QX11Info::display(), &ndevices );

    for( int i = 0; i < ndevices; i++ ) {
        uint wacom_prop = XInternAtom( QX11Info::display(), "Wacom Tool Type", True );

        XDevice *dev = XOpenDevice( QX11Info::display(), info[i].id );
        if( !dev ) {
            continue;
        }

        int natoms;
        Atom *atoms = XListDeviceProperties( QX11Info::display(), dev, &natoms );

        if( natoms ) {
            for( int j = 0; j < natoms; j++ ) {
                if( atoms[j] == wacom_prop ) {
                    deviceFound = true;
                    deviceId = info[i].id;
                }
            }
        }

        XFree( atoms );
        XCloseDevice( QX11Info::display(), dev );

        if( deviceFound ) {
            break;
        }
    }

    XFreeDeviceList( info );

    return deviceId;
}

void TabletDaemon::setupActions()
{
    Q_D( TabletDaemon );

    d->actionCollection = new KActionCollection( this );

    KAction *action = d->actionCollection->addAction(QLatin1String("toggle-touch-mode"));
    action->setText( i18nc( "@action", "Enable/Disable the Touch Tool" ) );
    action->setIcon( KIcon( QLatin1String( "input-tablet" ) ) );
    action->setGlobalShortcut( KShortcut( Qt::CTRL + Qt::ALT + Qt::Key_T ) );
    connect( action, SIGNAL( triggered() ), this, SLOT( actionToggleTouch() ) );

    action = d->actionCollection->addAction(QLatin1String("toggle-stylus-mode"));
    action->setText( i18nc( "@action", "Toggle the Stylus Tool Relative/Absolute" ) );
    action->setIcon( KIcon( QLatin1String( "input-tablet" ) ) );
    action->setGlobalShortcut( KShortcut( Qt::CTRL + Qt::ALT + Qt::Key_S ) );
    connect( action, SIGNAL( triggered() ), this, SLOT( actionTogglePenMode() ) );
}

void TabletDaemon::screenRotated( int screenRotation )
{
    Q_D( const TabletDaemon );

    KConfigGroup deviceGroup = KConfigGroup( d->profilesConfig, d->deviceHandler->deviceName() );
    KConfigGroup configGroup = KConfigGroup( &deviceGroup, d->curProfile );
    KConfigGroup stylusConfig( &configGroup, QLatin1String( "stylus" ) );

    if( stylusConfig.readEntry( QLatin1String( "0RotateWithScreen" ) ) == QLatin1String( "true" ) ) {

        QString stylusName = d->deviceHandler->stylusName();
        QString eraserName = d->deviceHandler->eraserName();
        QString touchName = d->deviceHandler->touchName();

        d->deviceHandler->setConfiguration( stylusName, QLatin1String( "Rotate" ), QString::fromLatin1( "%1" ).arg( screenRotation ) );
        d->deviceHandler->setConfiguration( eraserName, QLatin1String( "Rotate" ), QString::fromLatin1( "%1" ).arg( screenRotation ) );

        if( !touchName.isEmpty() ) {
            d->deviceHandler->setConfiguration( touchName, QLatin1String( "Rotate" ), QString::fromLatin1( "%1" ).arg( screenRotation ) );
        }
    }
}

void TabletDaemon::actionToggleTouch()
{
    Q_D( const TabletDaemon );

    d->deviceHandler->toggleTouch();
}

void TabletDaemon::actionTogglePenMode()
{
    Q_D( const TabletDaemon );

    d->deviceHandler->togglePenMode();
}
