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
#include "tabletinfo.h"
#include "deviceinterface.h"
#include "devicetype.h"
#include "xsetwacominterface.h"

// common includes
#include "dbustabletinterface.h" // required to copy TabletInformation from/to QDBusArgument
#include "deviceprofile.h"
#include "tabletdatabase.h"
#include "mainconfig.h"
#include "profilemanager.h"
#include "tabletprofile.h"
#include "x11utils.h"

#include <KDE/KLocalizedString>

#include <QtDBus/QDBusArgument>

namespace Wacom
{
    class TabletHandlerPrivate
    {
        public:
            MainConfig            mainConfig;
            ProfileManager        profileManager;   /**< Profile manager which reads and writes profiles from the configuration file */
            TabletDatabase        tabletDatabase;

            TabletInformation     tabletInformation;
            DeviceInterface      *currentDevice;     //!< Handler for the current device to get/set its configuration.
            bool                  isDeviceAvailable; //!< Is a tabled device connected or not?

            QMap<QString,QString> buttonMapping;     /**< Map the hardwarebuttons 1-X to its kernel numbering scheme
                                                          @see http://sourceforge.net/mailarchive/message.php?msg_id=27512095 */
            QString               currentProfile;   /**< currently active profile */
            int                   currentDeviceId;  /**< currently conencted tablet device. id comes from x11 */
    }; // CLASS
} // NAMESPACE

using namespace Wacom;

TabletHandler::TabletHandler()
    : QObject(NULL), d_ptr(new TabletHandlerPrivate)
{
    Q_D( TabletHandler );

    d->isDeviceAvailable = false;
    d->currentDevice     = NULL;

    d->profileManager.open( QLatin1String( "tabletprofilesrc" ) );
}


TabletHandler::~TabletHandler()
{
    delete d_ptr;
}



QString TabletHandler::getProperty(const QString& device, const Property& property) const
{
    return getProperty(device, property.key());
}


void TabletHandler::setProperty(const QString& device, const Property& property, const QString& value)
{
    setProperty(device, property.key(), value);
}




void TabletHandler::onDeviceAdded( int deviceid )
{
    Q_D( TabletHandler );

    // if we already have a device ... skip this step
    if( d->isDeviceAvailable || deviceid == 0) {
        return;
    }

    // No tablet available, so reload tablet information
    detectTablet();

    // if we found something notify about it and set the default profile to it
    if( d->isDeviceAvailable ) {

        d->currentDeviceId = deviceid;

        emit notify( QLatin1String("tabletAdded"),
                     i18n("Tablet added"),
                     i18n("New %1 tablet added", d->tabletInformation.get(TabletInfo::TabletName) ));

        emit tabletAdded(d->tabletInformation);
        setProfile(d->mainConfig.getLastProfile());
    }
}



void TabletHandler::onDeviceRemoved( int deviceid )
{
    Q_D( TabletHandler );
    if( d->isDeviceAvailable ) {
        d->isDeviceAvailable = false;
        if( d->currentDeviceId == deviceid ) {
            emit notify( QLatin1String("tabletRemoved"),
                         i18n("Tablet removed"),
                         i18n("Tablet %1 removed", d->tabletInformation.get(TabletInfo::TabletName) ));

            clearTabletInformation();
            emit tabletRemoved();
        }
    }
}



void TabletHandler::onScreenRotated( TabletRotation screenRotation )
{
    Q_D( TabletHandler );

    TabletProfile tabletProfile = d->profileManager.loadProfile(d->currentProfile);
    DeviceProfile stylusProfile = tabletProfile.getDevice(QLatin1String( "stylus" ));

    kDebug() << "xRandR screen rotation detected.";

    if ( stylusProfile.getRotateWithScreen() == QLatin1String( "true" ) ) {

        QString rotatecmd;

        switch(screenRotation) {
            case NONE:
                rotatecmd = QLatin1String("none");
                break;
            case CW:
                rotatecmd = QLatin1String("cw");
                break;
            case CCW:
                rotatecmd = QLatin1String("ccw");
                break;
            case HALF:
                rotatecmd = QLatin1String("half");
                break;
        }

        kDebug() << "Rotate tablet :: " << rotatecmd;

        QString stylusName = d->tabletInformation.getDeviceName(DeviceType::Stylus);
        QString eraserName = d->tabletInformation.getDeviceName(DeviceType::Eraser);
        QString touchName = d->tabletInformation.getDeviceName(DeviceType::Touch);

        setProperty( stylusName, Property::Rotate.key(), QString::fromLatin1( "%1" ).arg( rotatecmd ) );
        setProperty( eraserName, Property::Rotate.key(), QString::fromLatin1( "%1" ).arg( rotatecmd ) );

        if( !touchName.isEmpty() ) {
            setProperty( touchName, Property::Rotate.key(), QString::fromLatin1( "%1" ).arg( rotatecmd ) );
        }

        setProfile(d->currentProfile);
    }
}


QString TabletHandler::getProperty(const QString& device, const QString& param) const
{
    Q_D( const TabletHandler );

    if( !d->currentDevice ) {
        return QString();
    }

    const Property* property = Property::find(param);

    if (property == NULL) {
        kError() << QString::fromLatin1("Can not get invalid property '%1'!").arg(param);
        return QString();
    }

    return d->currentDevice->getProperty( device, *property );
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

    if (!d->currentDevice) {
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

        foreach(const DeviceType& type, DeviceType::list()) {
            if (d->tabletInformation.hasDevice(type)) {
                d->currentDevice->applyProfile (d->tabletInformation.getDeviceName(type), type, tabletProfile);
            }
        }

        d->mainConfig.setLastProfile(profile);
        emit profileChanged( profile );
    }
}



void TabletHandler::setProperty(const QString& device, const QString& param, const QString& value)
{
    Q_D( TabletHandler );

    if( !d->currentDevice ) {
        return;
    }

    const Property* property = Property::find(param);

    if (property == NULL) {
        kError() << QString::fromLatin1("Can not set invalid property '%1' to '%2'!").arg(param).arg(value);
        return;
    }

    d->currentDevice->setProperty( device, *property, value );
}



void TabletHandler::togglePenMode()
{
    Q_D( TabletHandler );

    if( !d->currentDevice ) {
        return;
    }

    if(!d->tabletInformation.hasDevice(DeviceType::Stylus)) {
        d->currentDevice->toggleMode(d->tabletInformation.getDeviceName(DeviceType::Stylus));
    }

    if(!d->tabletInformation.hasDevice(DeviceType::Eraser)) {
        d->currentDevice->toggleMode(d->tabletInformation.getDeviceName(DeviceType::Eraser) );
    }

}



void TabletHandler::toggleTouch()
{
    Q_D( TabletHandler );

    if( !d->currentDevice || d->tabletInformation.hasDevice(DeviceType::Touch) ) {
        return;
    }

    d->currentDevice->toggleTouch(d->tabletInformation.getDeviceName(DeviceType::Touch));
}



void TabletHandler::clearTabletInformation()
{
    Q_D( TabletHandler );

    TabletInformation empty;

    d->isDeviceAvailable = false;
    d->tabletInformation = empty;

    delete d->currentDevice;
    d->currentDevice = NULL;

    d->buttonMapping.clear();
}



bool TabletHandler::detectTablet()
{
    Q_D( TabletHandler );

    TabletInformation devinfo;

    if (!X11Utils::findTabletDevice(devinfo)) {
        kDebug() << "no input devices (pad/stylus/eraser/cursor/touch) found via xinput";
        return false;
    }

    kDebug() << "XInput found a device! ::" << devinfo.tabletId;

    if (!d->tabletDatabase.lookupDevice(devinfo, devinfo.tabletId)) {
        kDebug() << "Could not find device in database: " << devinfo.tabletId;
        return false;
    }

    d->tabletInformation  = devinfo;

    // lookup button mapping
    d->tabletDatabase.lookupButtonMapping(d->buttonMapping, devinfo.companyId, devinfo.tabletId);

    // set device backend
    selectDeviceBackend( d->tabletDatabase.lookupBackend(devinfo.companyId) );

    // \0/
    d->isDeviceAvailable = true;

    return true;
}


void TabletHandler::selectDeviceBackend(const QString& backendName)
{
    Q_D( TabletHandler );

    if( backendName == QLatin1String( "wacom-tools" ) ) {
        d->currentDevice = new XsetwacomInterface();
        d->currentDevice->setButtonMapping(d->buttonMapping);
    }

    if( !d->currentDevice ) {
        kError() << "unknown device backend!" << backendName;
    }
}
