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
#include "deviceinfo.h"
#include "devicetype.h"
#include "devicehandler.h"

#include "mainconfig.h"
#include "profilemanager.h"
#include "tabletprofile.h"
#include "deviceprofile.h"
#include "x11utils.h"

#include <KDE/KLocalizedString>

namespace Wacom
{
    class TabletHandlerPrivate
    {
        public:
            MainConfig            mainConfig;
            ProfileManager        profileManager;   /**< Profile manager which reads and writes profiles from the configuration file */
            DeviceHandler*        deviceHandler;    /**< Pointer to the tablet device */
            QString               currentProfile;   /**< currently active profile */
            int                   currentDeviceId;  /**< currently conencted tablet device. id comes from x11 */

    }; // CLASS
} // NAMESPACE

using namespace Wacom;

TabletHandler::TabletHandler(DeviceHandler& deviceHandler)
    : QObject(NULL), d_ptr(new TabletHandlerPrivate)
{
    Q_D( TabletHandler );
    d->deviceHandler = &deviceHandler;
    d->profileManager.open( QLatin1String( "tabletprofilesrc" ) );
}


void TabletHandler::actionTogglePenMode()
{
    Q_D( TabletHandler );
    d->deviceHandler->togglePenMode();
}


void TabletHandler::actionToggleTouch()
{
    Q_D( TabletHandler );
    d->deviceHandler->toggleTouch();
}


void TabletHandler::onDeviceAdded( int deviceid )
{
    Q_D( TabletHandler );

    // if we already have a device ... skip this step
    if( d->deviceHandler->isDeviceAvailable() || deviceid == 0) {
        return;
    }

    // No tablet available, so reload tablet information
    d->deviceHandler->detectTablet();

    // if we found something notify about it and set the default profile to it
    if( d->deviceHandler->isDeviceAvailable() ) {

        d->currentDeviceId = deviceid;

        emit notify( QLatin1String("tabletAdded"),
                     i18n("Tablet added"),
                     i18n("New %1 tablet added", d->deviceHandler->getInformation(DeviceInfo::TabletName) ));

        emit tabletAdded();
        setProfile(d->mainConfig.getLastProfile());
    }
}


void TabletHandler::onDeviceRemoved( int deviceid )
{
    Q_D( TabletHandler );
    if( d->deviceHandler->isDeviceAvailable() ) {
        if( d->currentDeviceId == deviceid ) {
            emit notify( QLatin1String("tabletRemoved"),
                         i18n("Tablet removed"),
                         i18n("Tablet %1 removed", d->deviceHandler->getInformation(DeviceInfo::TabletName) ));

            d->deviceHandler->clearDeviceInformation();
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

        QString stylusName = d->deviceHandler->getDeviceName(DeviceType::Stylus);
        QString eraserName = d->deviceHandler->getDeviceName(DeviceType::Eraser);
        QString touchName = d->deviceHandler->getDeviceName(DeviceType::Touch);

        d->deviceHandler->setConfiguration( stylusName, QLatin1String( "Rotate" ), QString::fromLatin1( "%1" ).arg( rotatecmd ) );
        d->deviceHandler->setConfiguration( eraserName, QLatin1String( "Rotate" ), QString::fromLatin1( "%1" ).arg( rotatecmd ) );

        if( !touchName.isEmpty() ) {
            d->deviceHandler->setConfiguration( touchName, QLatin1String( "Rotate" ), QString::fromLatin1( "%1" ).arg( rotatecmd ) );
        }

        setProfile(profile());
    }
}


bool TabletHandler::tabletAvailable() const
{
    Q_D( const TabletHandler );
    return d->deviceHandler->isDeviceAvailable();
}


void TabletHandler::setProfile( const QString &profile )
{
    Q_D( TabletHandler );

    d->profileManager.readProfiles(d->deviceHandler->getInformation(DeviceInfo::TabletName));
    TabletProfile tabletProfile = d->profileManager.loadProfile(profile);

    if (tabletProfile.listDevices().isEmpty()) {
        emit notify( QLatin1String( "tabletError" ),
                     i18n( "Graphic Tablet error" ),
                     i18n( "Profile <b>%1</b> does not exist", profile ) );

    } else {
        d->currentProfile = profile;
        d->deviceHandler->applyProfile( tabletProfile );
        d->mainConfig.setLastProfile(profile);

        emit profileChanged( profile );
    }
}


QString TabletHandler::profile() const
{
    Q_D( const TabletHandler );
    return d->currentProfile;
}


QStringList TabletHandler::profileList() const
{
    Q_D( const TabletHandler );

    // we can not reload the profile manager from a const method so we have
    // to create a new instance here and let it read the configuration file.
    ProfileManager profileManager(QLatin1String( "tabletprofilesrc" ));
    profileManager.readProfiles(d->deviceHandler->getInformation(DeviceInfo::TabletName));

    return profileManager.listProfiles();
}



