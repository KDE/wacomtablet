/*
 * This file is part of the KDE wacomtablet project. For copyright
 * information and license terms see the AUTHORS and COPYING files
 * in the top-level directory of this distribution.
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

#include "tablethandler.h"
#include "debug.h"

#include "tabletbackendinterface.h"
#include "tabletbackendfactory.h"
#include "tabletinfo.h"
#include "devicetype.h"
#include "screenmap.h"
#include "screenspace.h"
#include "stringutils.h"

// common includes
#include "deviceprofile.h"
#include "tabletdatabase.h"
#include "mainconfig.h"
#include "profilemanager.h"
#include "tabletprofile.h"
#include "x11info.h"

#include <QtCore/QList>
#include <QtCore/QRect>
#include <QtCore/QRegExp>

#include <KDE/KLocalizedString>


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
    d->profileManager.open (QLatin1String("tabletprofilesrc"));
    d->mainConfig.open(QLatin1String("wacomtablet-kderc"));
}


TabletHandler::TabletHandler(const QString& profileFile, const QString configFile)
    : TabletHandlerInterface(NULL), d_ptr(new TabletHandlerPrivate)
{
    Q_D( TabletHandler );

    d->tabletBackend = NULL;
    d->profileManager.open( profileFile );
    d->mainConfig.open(configFile);
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

    // if we already have a device ... skip this step
    if(hasTablet()) {
        kDebug() << QString::fromLatin1("Ignoring tablet '%1' as another one is already connected.").arg(info.get(TabletInfo::TabletId));
        return;
    }

    kDebug() << "Taking control of new tablet" << info.get(TabletInfo::TabletName)
             << "(" << info.get(TabletInfo::TabletId) << ") ["
             << (info.hasDevice(DeviceType::Stylus) ? "stylus" : "")
             << (info.hasDevice(DeviceType::Eraser) ? "eraser" : "")
             << (info.hasDevice(DeviceType::Pad)    ? "pad"    : "")
             << (info.hasDevice(DeviceType::Touch)  ? "touch"  : "")
             << (info.hasDevice(DeviceType::Cursor) ? "cursor" : "")
             << "]";

    // create tablet backend
    clearTabletInformation();
    d->tabletBackend = TabletBackendFactory::createBackend(info);

    if (d->tabletBackend == NULL) {
        return; // no valid backend found
    }

    // update tablet information
    d->tabletInformation = info;
    d->tabletInformation.setAvailable(true);

    // if we found something notify about it and set the default profile to it
    emit notify( QLatin1String("tabletAdded"),
                 i18n("Tablet Connected"),
                 i18n("New tablet '%1' connected.",
                 d->tabletInformation.get(TabletInfo::TabletName) ));

    // set profile which was last used
    setProfile(d->mainConfig.getLastProfile());

    // notify everyone else about the new tablet
    emit tabletAdded(d->tabletInformation);
}



void TabletHandler::onTabletRemoved( const TabletInformation& info )
{
    Q_D( TabletHandler );

    if ( d->tabletBackend && d->tabletInformation.getTabletSerial() == info.getTabletSerial() ) {
        emit notify( QLatin1String("tabletRemoved"),
                     i18n("Tablet removed"),
                     i18n("Tablet %1 removed",
                     d->tabletInformation.get(TabletInfo::TabletName) ));

        clearTabletInformation();
        emit tabletRemoved();
    }
}



void TabletHandler::onScreenRotated( const ScreenRotation& screenRotation )
{
    Q_D( TabletHandler );

    TabletProfile tabletProfile = d->profileManager.loadProfile(d->currentProfile);

    // rotation has to be applied before screen mapping
    autoRotateTablet(screenRotation, tabletProfile);

    // when the rotation changes, the screen mapping has to be applied again
    mapTabletToCurrentScreenSpace(tabletProfile);
}


void TabletHandler::onTogglePenMode()
{
    Q_D( TabletHandler );

    if( !hasTablet() || !hasDevice(DeviceType::Stylus)) {
        return;
    }

    // read current mode and screen space from profile
    TabletProfile tabletProfile = d->profileManager.loadProfile(d->currentProfile);
    DeviceProfile stylusProfile = tabletProfile.getDevice(DeviceType::Stylus);

    QString     trackingMode = stylusProfile.getProperty(Property::Mode);
    ScreenSpace screenSpace(stylusProfile.getProperty(Property::ScreenSpace));

    // toggle tracking mode
    if (trackingMode.contains(QLatin1String("relative"), Qt::CaseInsensitive)) {
        trackingMode = QLatin1String("absolute");

    } else {
        // if the new mode is "relative" we have to switch to full desktop
        // as screen mappings are not supported in absolute mode
        trackingMode = QLatin1String("relative");
        screenSpace  = ScreenSpace::desktop();
    }

    // map tablet to output which will also save the current mode in the profile
    mapDeviceToOutput(DeviceType::Stylus, screenSpace, trackingMode, tabletProfile);
    mapDeviceToOutput(DeviceType::Eraser, screenSpace, trackingMode, tabletProfile);

    d->profileManager.saveProfile(tabletProfile);
}



void TabletHandler::onToggleTouch()
{
    Q_D( TabletHandler );

    if( !hasDevice(DeviceType::Touch) ) {
        return;
    }

    QString touchMode = getProperty(DeviceType::Touch, Property::Touch);

    // also save the touch on/off into the profile to remember the user selection after
    // the tablet was reconnected
    TabletProfile tabletProfile = d->profileManager.loadProfile(d->currentProfile);
    DeviceProfile touchProfile = tabletProfile.getDevice(DeviceType::Touch);

    if( touchMode.compare( QLatin1String( "off" ), Qt::CaseInsensitive) == 0 ) {
        setProperty(DeviceType::Touch, Property::Touch, QLatin1String("on"));
        touchProfile.setProperty( Property::Touch, QLatin1String("on" ) );
    } else {
        setProperty(DeviceType::Touch, Property::Touch, QLatin1String("off"));
        touchProfile.setProperty( Property::Touch, QLatin1String("off") );
    }

    tabletProfile.setDevice(touchProfile);
    d->profileManager.saveProfile(tabletProfile);
}


void TabletHandler::onToggleScreenMapping()
{
    Q_D( TabletHandler );

    if (!hasTablet()) {
        return;
    }

    TabletProfile tabletProfile  = d->profileManager.loadProfile(d->currentProfile);
    DeviceProfile stylusProfile  = tabletProfile.getDevice(DeviceType::Stylus);
    ScreenSpace   screenSpace    = ScreenSpace(stylusProfile.getProperty(Property::ScreenSpace));

    if (screenSpace.isMonitor()) {
        // get next monitor - mapTabletToOutput() will handle disconnected monitors
        int screenNumber = screenSpace.getScreenNumber() + 1;
        screenSpace = ScreenSpace::monitor(screenNumber);
    } else {
        screenSpace = ScreenSpace::monitor(0);
    }

    mapPenToScreenSpace(screenSpace.toString());
}


void TabletHandler::onMapToFullScreen()
{
    mapPenToScreenSpace(ScreenSpace::desktop().toString());
}



void TabletHandler::onMapToScreen1()
{
    mapPenToScreenSpace(ScreenSpace::monitor(0).toString());
}



void TabletHandler::onMapToScreen2()
{
    if (X11Info::getNumberOfScreens() >= 2) {
        mapPenToScreenSpace(ScreenSpace::monitor(1).toString());
    }
}


QStringList TabletHandler::listProfiles()
{
    Q_D( TabletHandler );

    d->profileManager.readProfiles(d->tabletInformation.get(TabletInfo::TabletName));
    return d->profileManager.listProfiles();
}



void TabletHandler::setProfile( const QString &profile )
{
    Q_D( TabletHandler );

    if (!hasTablet()) {
        kDebug() << QString::fromLatin1("Can not set tablet profile to '%1' as not backend is available!").arg(profile);
        return;
    }

    kDebug() << QString::fromLatin1("Loading tablet profile '%1'...").arg(profile);

    d->profileManager.readProfiles(d->tabletInformation.get(TabletInfo::TabletName));

    TabletProfile tabletProfile = d->profileManager.loadProfile(profile);

    if (tabletProfile.listDevices().isEmpty()) {
        kError() << QString::fromLatin1("Tablet profile '%1' does not exist!").arg(profile);
        emit notify( QLatin1String( "tabletError" ),
                     i18n( "Graphic Tablet error" ),
                     i18n( "Profile <b>%1</b> does not exist", profile ) );

    } else {
        // set profile
        d->currentProfile = profile;

        // Handle auto-rotation.
        // This has to be done before screen mapping!
        autoRotateTablet(X11Info::getScreenRotation(), tabletProfile);

        // Map tablet to screen.
        // This is necessary to ensure the correct area map is used. Somone might have changed
        // the ScreenSpace property without updating the Area property.
        mapTabletToCurrentScreenSpace(tabletProfile);

        // set profile on tablet
        d->tabletBackend->setProfile(tabletProfile);
        d->mainConfig.setLastProfile(profile);

        emit profileChanged( profile );
    }
}



void TabletHandler::setProperty(const DeviceType& deviceType, const Property& property, const QString& value)
{
    Q_D( TabletHandler );

    if (!hasTablet()) {
        kError() << QString::fromLatin1("Unable to set property '%1' on device '%2' to '%3' as no device is currently available!").arg(property.key()).arg(deviceType.key()).arg(value);
        return;
    }

    d->tabletBackend->setProperty(deviceType, property, value);
}


void TabletHandler::autoRotateTablet(const ScreenRotation &screenRotation, const TabletProfile &tabletProfile)
{
    // determine auto-rotation configuration
    DeviceProfile         stylusProfile    = tabletProfile.getDevice(DeviceType::Stylus);

    QString               rotateProperty   = stylusProfile.getProperty( Property::Rotate);
    const ScreenRotation* lookupRotation   = ScreenRotation::find(rotateProperty);
    ScreenRotation        tabletRotation   = (lookupRotation != NULL) ? *lookupRotation : ScreenRotation::NONE;

    bool                  doAutoInvert     = (tabletRotation == ScreenRotation::AUTO_INVERTED);
    bool                  doAutoRotation   = (doAutoInvert || tabletRotation == ScreenRotation::AUTO);

    if (!doAutoRotation) {
        return; // auto-rotation is disabled
    }

    // determine new rotation and set it
    ScreenRotation newRotation = (doAutoInvert) ? screenRotation.invert() : screenRotation;

    kDebug() << "Rotate tablet :: " << newRotation.key();

    setProperty( DeviceType::Stylus, Property::Rotate, newRotation.key() );
    setProperty( DeviceType::Eraser, Property::Rotate, newRotation.key() );

    if(hasDevice(DeviceType::Touch)) {
        setProperty( DeviceType::Touch, Property::Rotate, newRotation.key() );
    }
}


bool TabletHandler::hasDevice(const DeviceType& type) const
{
    Q_D( const TabletHandler );

    return (hasTablet() && d->tabletInformation.hasDevice(type));
}



bool TabletHandler::hasTablet() const
{
    Q_D( const TabletHandler );

    return (d->tabletBackend != NULL);
}


void TabletHandler::mapDeviceToOutput(const DeviceType& device, const ScreenSpace& screenSpace, const QString& trackingMode, TabletProfile& tabletProfile)
{
    if (!hasTablet() || !hasDevice(device)) {
        return; // we do not have a tablet or the requested device
    }

    ScreenSpace screen(screenSpace);
    int         screenCount = X11Info::getNumberOfScreens();

    if (screen.isMonitor()) {
        if (screen.isMonitor(0) && screenCount == 1) {
            // we got only one screen
            // map to desktop as this allows relative mode as well
            screen = ScreenSpace::desktop();

        } else if (screen.getScreenNumber() >= screenCount) {
            // requested screen got disconnected
            // use desktop instead
            screen = ScreenSpace::desktop();
        }
    }

    DeviceProfile deviceProfile = tabletProfile.getDevice(device);
    ScreenMap     screenMap(deviceProfile.getProperty(Property::ScreenMap));
    QString       tabletArea    = screenMap.getMappingAsString(screen);

    setProperty(device, Property::Mode, trackingMode);
    setProperty(device, Property::ScreenSpace, screen.toString());
    setProperty(device, Property::Area, tabletArea);

    deviceProfile.setProperty(Property::Mode, trackingMode);
    deviceProfile.setProperty(Property::ScreenSpace, screen.toString());
    deviceProfile.setProperty(Property::Area, tabletArea);

    tabletProfile.setDevice(deviceProfile);
}



void TabletHandler::mapPenToScreenSpace(const ScreenSpace& screenSpace, const QString& trackingMode)
{
    Q_D( TabletHandler );

    if (!hasTablet()) {
        return; // we do not have a tablet
    }

    TabletProfile tabletProfile  = d->profileManager.loadProfile(d->currentProfile);

    mapDeviceToOutput(DeviceType::Stylus, screenSpace, trackingMode, tabletProfile);
    mapDeviceToOutput(DeviceType::Eraser, screenSpace, trackingMode, tabletProfile);

    d->profileManager.saveProfile(tabletProfile);
}


void TabletHandler::mapTabletToCurrentScreenSpace(TabletProfile& tabletProfile)
{
    Q_D( TabletHandler );

    DeviceProfile stylusProfile = tabletProfile.getDevice(DeviceType::Stylus);
    DeviceProfile touchProfile  = tabletProfile.getDevice(DeviceType::Touch);

    QString       stylusMode    = stylusProfile.getProperty(Property::Mode);
    ScreenSpace   stylusSpace   = ScreenSpace(stylusProfile.getProperty(Property::ScreenSpace));
    QString       touchMode     = touchProfile.getProperty(Property::Mode);
    ScreenSpace   touchSpace    = ScreenSpace(touchProfile.getProperty(Property::ScreenSpace));

    mapDeviceToOutput(DeviceType::Stylus, stylusSpace, stylusMode, tabletProfile);
    mapDeviceToOutput(DeviceType::Eraser, stylusSpace, stylusMode, tabletProfile);
    mapDeviceToOutput(DeviceType::Touch,  touchSpace,  touchMode,  tabletProfile);

    d->profileManager.saveProfile(tabletProfile);
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

