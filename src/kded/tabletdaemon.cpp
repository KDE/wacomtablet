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

#include "debug.h"
#include "tabletdaemon.h"

#include "dbustabletservice.h"
#include "tablethandler.h"
#include "wacomadaptor.h"
#include "../version.h"

// common includes
#include "aboutdata.h"
#include "x11utils.h"

// stdlib includes
#include <memory>

// KDE includes
#include <KDE/KPluginFactory>
#include <KDE/KNotification>
#include <KDE/KLocale>
#include <KDE/KLocalizedString>
#include <KDE/KIconLoader>
#include <KDE/KComponentData>
#include <KDE/KActionCollection>
#include <KDE/KAction>
#include <KDE/KDebug>

using namespace Wacom;

K_PLUGIN_FACTORY( WacomTabletFactory, registerPlugin<TabletDaemon>(); )
K_EXPORT_PLUGIN( WacomTabletFactory( "wacomtabletdaemon" ) )

namespace Wacom {
/**
  * Private class of the TabletDaemon for the d-pointer
  */
class TabletDaemonPrivate {
public:
    TabletDaemonPrivate() : tabletHandler(), dbusTabletService(tabletHandler) {}

    TabletHandler                    tabletHandler;    /**< tablet handler */
    DBusTabletService                dbusTabletService;
    XDeviceEventNotifier             xEventNotifier;   /**< X11 Event handler to detect when the tablet is connected/removed */
    std::auto_ptr<KComponentData>    applicationData;  /**< Basic application data */
    std::auto_ptr<KActionCollection> actionCollection; /**< Collection of all global actions */

}; // CLASS
}  // NAMESPACE



TabletDaemon::TabletDaemon( QObject *parent, const QVariantList &args )
    : KDEDModule( parent ), d_ptr( new TabletDaemonPrivate )
{
    Q_UNUSED( args );
    Q_D( TabletDaemon );

    setupApplication();
    setupDBus();
    setupXEventNotifier();
    setupActions();
    setupDevice();

    // Connecting this after the device has been set up ensures that no notification is send on startup.
    connect( &(d->tabletHandler), SIGNAL(notify(QString,QString,QString)), this, SLOT(onNotify(QString,QString,QString)) );
}



TabletDaemon::~TabletDaemon()
{
    this->d_ptr->xEventNotifier.stop();

    //QDBusConnection::sessionBus().unregisterService( QLatin1String( "org.kde.Wacom" ) );

    delete this->d_ptr;
}



void TabletDaemon::onNotify(const QString& eventId, const QString& title, const QString& message)
{
    Q_D( TabletDaemon );
    static KIconLoader iconLoader( *(d->applicationData) );

    KNotification notification(eventId);
    notification.setTitle(title);
    notification.setText(message);
    notification.setComponentData( *(d->applicationData) );
    notification.setPixmap( iconLoader.loadIcon( QLatin1String( "input-tablet" ), KIconLoader::Panel ) );
    notification.sendEvent();
}



void TabletDaemon::setupActions()
{
    Q_D( TabletDaemon );

    //if someone adds another action also add it to kcmodule/generalwidget.cpp
    d->actionCollection = std::auto_ptr<KActionCollection>(new KActionCollection(this, *(d->applicationData)));

    KAction *action = d->actionCollection->addAction(QLatin1String("Toggle touch tool"));
    action->setText( i18nc( "@action", "Enable/Disable the Touch Tool" ) );
    action->setGlobalShortcut( KShortcut( Qt::CTRL + Qt::META + Qt::Key_T ) );
    connect( action, SIGNAL( triggered() ), &(d->tabletHandler), SLOT( onToggleTouch() ) );

    action = d->actionCollection->addAction(QLatin1String("Toggle stylus mode"));
    action->setText( i18nc( "@action", "Toggle the Stylus Tool Relative/Absolute" ) );
    action->setGlobalShortcut( KShortcut( Qt::CTRL + Qt::META + Qt::Key_S ) );
    connect( action, SIGNAL( triggered() ), &(d->tabletHandler), SLOT( onTogglePenMode() ) );
}



void TabletDaemon::setupApplication()
{
    Q_D( TabletDaemon );

    KGlobal::locale()->insertCatalog( QLatin1String( "wacomtablet" ) );

    static AboutData about( "wacomtablet",
                            ki18n( "Graphic Tablet Configuration daemon" ),
                            kded_version, ki18n( "A Wacom tablet control daemon" ) );

    d->applicationData = std::auto_ptr<KComponentData>(new KComponentData(about));
}



void TabletDaemon::setupDBus()
{
    Q_D( TabletDaemon );

    // connect tablet handler events to D-Bus
    // this is done here and not in the D-Bus tablet service to facilitate unit testing
    connect(&(d->tabletHandler), SIGNAL (profileChanged(const QString&)),        &(d->dbusTabletService), SLOT (onProfileChanged(const QString&)));
    connect(&(d->tabletHandler), SIGNAL (tabletAdded(const TabletInformation&)), &(d->dbusTabletService), SLOT (onTabletAdded(const TabletInformation&)));
    connect(&(d->tabletHandler), SIGNAL (tabletRemoved()),                       &(d->dbusTabletService), SLOT (onTabletRemoved()));
}



void TabletDaemon::setupDevice()
{
    Q_D( TabletDaemon );

    TabletInformation tabinfo;

    if (X11Utils::findTabletDevice(tabinfo)) {
        bool ok       = false;
        int  deviceId = tabinfo.xdeviceId.toInt(&ok, 10);

        if (ok) {
            d->tabletHandler.onDeviceAdded(deviceId);
        }
    }
}



void TabletDaemon::setupXEventNotifier()
{
    Q_D( TabletDaemon );

    d->xEventNotifier.start();
    connect( &(d->xEventNotifier), SIGNAL( deviceAdded( int ) ),              &(d->tabletHandler), SLOT( onDeviceAdded( int ) ) );
    connect( &(d->xEventNotifier), SIGNAL( deviceRemoved( int ) ),            &(d->tabletHandler), SLOT( onDeviceRemoved( int ) ) );
    connect( &(d->xEventNotifier), SIGNAL( screenRotated( TabletRotation ) ), &(d->tabletHandler), SLOT( onScreenRotated( TabletRotation ) ) );
}

