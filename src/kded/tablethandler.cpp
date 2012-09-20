/*
 * Copyright 2009, 2010 Jörg Ehrichs <joerg.ehichs@gmx.de>
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

#include "tablethandler.h"

#include "tabletbackendinterface.h"
#include "tabletbackendfactory.h"
#include "tabletinfo.h"
#include "devicetype.h"

// common includes
#include "dbustabletinterface.h" // required to copy TabletInformation from/to QDBusArgument
#include "deviceprofile.h"
#include "tabletdatabase.h"
#include "mainconfig.h"
#include "profilemanager.h"
#include "tabletprofile.h"

#include <KDE/KLocalizedString>

#include <QtDBus/QDBusArgument>
#include <X11/Xlib.h>

namespace Wacom
{
    class TabletHandlerPrivate
    {
        public:
            MainConfig              mainConfig;        //!< Main config file which stores general parameters.
            ProfileManager          profileManager;    //!< Profile manager which reads profile configuration from file.
            TabletBackendInterface *tabletBackend;     //!< Tablet backend of the currently connected tablet.
            TabletInformation       tabletInformation; //!< Information of currently connected tablet.
            QString                 currentProfile;    //!< Currently active profile.
    }; // CLASS
} // NAMESPACE

using namespace Wacom;

TabletHandler::TabletHandler()
    : TabletHandlerInterface(NULL), d_ptr(new TabletHandlerPrivate)
{
    Q_D( TabletHandler );

    d->tabletBackend = NULL;
    d->profileManager.open( QLatin1String( "tabletprofilesrc" ) );
}


TabletHandler::~TabletHandler()
{
    clearTabletInformation();
    delete d_ptr;
}



QString TabletHandler::getProperty(const DeviceType& deviceType, const Property& property) const
{
    Q_D( const TabletHandler );

    if( !d->tabletBackend ) {
        kError() << QString::fromLatin1("Unable to get property '%1' from device '%2' as no device is currently available!").arg(property.key()).arg(deviceType.key());
        return QString();
    }

    return d->tabletBackend->getProperty( deviceType, property );
}


void TabletHandler::onTabletAdded( const TabletInformation& info )
{
    Q_D( TabletHandler );

    int deviceId = info.getXDeviceId();

    // if we already have a device ... skip this step
    if(d->tabletBackend || deviceId == 0) {
        return;
    }

    // create tablet backend
    d->tabletInformation = info;
    d->tabletBackend     = TabletBackendFactory::createBackend(d->tabletInformation);

    if (d->tabletBackend == NULL) {
        clearTabletInformation();
        return;
    }

    d->tabletInformation.setAvailable(true);

    // if we found something notify about it and set the default profile to it
    emit notify( QLatin1String("tabletAdded"),
                 i18n("Tablet added"),
                 i18n("New %1 tablet added",
                 d->tabletInformation.get(TabletInfo::TabletName) ));

    emit tabletAdded(d->tabletInformation);

    // set profile which was last used
    setProfile(d->mainConfig.getLastProfile());
}



void TabletHandler::onTabletRemoved( const TabletInformation& info )
{
    Q_D( TabletHandler );

    int deviceId = info.getXDeviceId();

    if ( d->tabletBackend && d->tabletInformation.getXDeviceId() == deviceId ) {
        emit notify( QLatin1String("tabletRemoved"),
                     i18n("Tablet removed"),
                     i18n("Tablet %1 removed",
                     d->tabletInformation.get(TabletInfo::TabletName) ));

        clearTabletInformation();
        emit tabletRemoved();
    }
}



void TabletHandler::onScreenRotated( const TabletRotation& screenRotation )
{
    Q_D( TabletHandler );

    TabletProfile tabletProfile = d->profileManager.loadProfile(d->currentProfile);
    DeviceProfile stylusProfile = tabletProfile.getDevice(DeviceType::Stylus.key());

    kDebug() << "xRandR screen rotation detected.";

    if ( stylusProfile.getRotateWithScreen() == QLatin1String( "true" ) ) {

        kDebug() << "Rotate tablet :: " << screenRotation.key();

        setProperty( DeviceType::Stylus, Property::Rotate, QString::fromLatin1( "%1" ).arg( screenRotation.key() ) );
        setProperty( DeviceType::Eraser, Property::Rotate, QString::fromLatin1( "%1" ).arg( screenRotation.key() ) );

        if(d->tabletInformation.hasDevice (DeviceType::Touch)) {
            setProperty( DeviceType::Touch, Property::Rotate, QString::fromLatin1( "%1" ).arg( screenRotation.key() ) );
        }

        setProfile(d->currentProfile);
    }
}


void TabletHandler::onTogglePenMode()
{
    Q_D( TabletHandler );

    if( !d->tabletBackend ) {
        return;
    }

    if(!d->tabletInformation.hasDevice(DeviceType::Stylus)) {
        toggleMode(DeviceType::Stylus);
    }

    if(!d->tabletInformation.hasDevice(DeviceType::Eraser)) {
        toggleMode(DeviceType::Eraser);
    }
}



void TabletHandler::onToggleTouch()
{
    Q_D( TabletHandler );

    if( !d->tabletBackend || d->tabletInformation.hasDevice(DeviceType::Touch) ) {
        return;
    }

    QString touchMode = d->tabletBackend->getProperty(DeviceType::Touch, Property::Touch);

    if( touchMode.compare( QLatin1String( "off" ), Qt::CaseInsensitive) == 0 ) {
        d->tabletBackend->setProperty(DeviceType::Touch, Property::Touch, QLatin1String("on"));
    } else {
        d->tabletBackend->setProperty(DeviceType::Touch, Property::Touch, QLatin1String("off"));
    }
}



QStringList TabletHandler::listProfiles() const
{
    Q_D( const TabletHandler );
    // we can not reload the profile manager from a const method so we have
    // to create a new instance here and let it read the configuration file.
    ProfileManager profileManager(QLatin1String( "tabletprofilesrc" ));
    profileManager.readProfiles(d->tabletInformation.get(TabletInfo::TabletName));

    return profileManager.listProfiles();
}



void TabletHandler::setProfile( const QString &profile )
{
    Q_D( TabletHandler );

    if (!d->tabletBackend) {
        return;
    }

    d->profileManager.readProfiles(d->tabletInformation.get(TabletInfo::TabletName));

    TabletProfile tabletProfile = d->profileManager.loadProfile(profile);

    if (tabletProfile.listDevices().isEmpty()) {
        emit notify( QLatin1String( "tabletError" ),
                     i18n( "Graphic Tablet error" ),
                     i18n( "Profile <b>%1</b> does not exist", profile ) );

    } else {
        d->currentProfile = profile;
        d->tabletBackend->setProfile(tabletProfile);
        d->mainConfig.setLastProfile(profile);
        emit profileChanged( profile );
    }
}



void TabletHandler::setProperty(const DeviceType& deviceType, const Property& property, const QString& value)
{
    Q_D( TabletHandler );

    if (!d->tabletBackend) {
        kError() << QString::fromLatin1("Unable to set property '%1' on device '%2' to '%3' as no device is currently available!").arg(property.key()).arg(deviceType.key()).arg(value);
        return;
    }

    d->tabletBackend->setProperty(deviceType, property, value);
}



void TabletHandler::clearTabletInformation()
{
    Q_D( TabletHandler );

    TabletInformation empty;
    d->tabletInformation = empty;
    d->tabletInformation.setAvailable(false);

    if (d->tabletBackend) {
        delete d->tabletBackend;
        d->tabletBackend = NULL;
    }
}



void TabletHandler::toggleMode(const DeviceType& type)
{
    Q_D( TabletHandler );

    if (!d->tabletBackend) {
        return;
    }

    QString mode = d->tabletBackend->getProperty(type, Property::Mode);

    if( mode.compare( QLatin1String( "Absolute" ), Qt::CaseInsensitive ) == 0 ) {
        d->tabletBackend->setProperty(type, Property::Mode, QLatin1String("Relative"));
    } else {
        d->tabletBackend->setProperty(type, Property::Mode, QLatin1String("Absolute"));
    }
}
