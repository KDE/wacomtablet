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

    // determine auto-rotation configuration and new rotation
    TabletProfile   tabletProfile      = d->profileManager.loadProfile(d->currentProfile);
    DeviceProfile   stylusProfile      = tabletProfile.getDevice(DeviceType::Stylus);

    QString               rotateProperty   = stylusProfile.getProperty( Property::Rotate);
    const ScreenRotation* lookupRotation   = ScreenRotation::find(rotateProperty);
    ScreenRotation        tabletRotation   = (lookupRotation != NULL) ? *lookupRotation : ScreenRotation::NONE;

    bool                  doAutoInvert     = (tabletRotation == ScreenRotation::AUTO_INVERTED);
    bool                  doAutoRotation   = (doAutoInvert || tabletRotation == ScreenRotation::AUTO);
    ScreenRotation        newRotation      = screenRotation;

    if (doAutoInvert) {
        // the user wants inverted auto-rotation
        if (screenRotation == ScreenRotation::CW) {
            newRotation = ScreenRotation::CCW;
        } else if (screenRotation == ScreenRotation::CCW) {
            newRotation = ScreenRotation::CW;
        }
    }

    if (!doAutoRotation) {
        return; // auto-rotation is disabled
    }

    kDebug() << "Rotate tablet :: " << newRotation.key();

    setProperty( DeviceType::Stylus, Property::Rotate, newRotation.key() );
    setProperty( DeviceType::Eraser, Property::Rotate, newRotation.key() );

    if(hasDevice(DeviceType::Touch)) {
        setProperty( DeviceType::Touch, Property::Rotate, newRotation.key() );
    }
}


void TabletHandler::onTogglePenMode()
{
    Q_D( TabletHandler );

    if( !hasTablet() || !hasDevice(DeviceType::Stylus)) {
        return;
    }

    // also save the pen mode into the profile to remember the user selection after
    // the tablet was reconnected
    TabletProfile tabletProfile = d->profileManager.loadProfile(d->currentProfile);
    DeviceProfile stylusProfile = tabletProfile.getDevice(DeviceType::Stylus);
    DeviceProfile eraserProfile = tabletProfile.getDevice(DeviceType::Eraser);

    QString newMode = toggleMode(DeviceType::Stylus);
    stylusProfile.setProperty( Property::Mode, newMode );

    if(hasDevice(DeviceType::Eraser)) {
        setProperty(DeviceType::Eraser, Property::Mode, newMode);
        eraserProfile.setProperty( Property::Mode, newMode );
    }

    tabletProfile.setDevice(stylusProfile);
    tabletProfile.setDevice(eraserProfile);
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

    QRegExp monitorRegExp(QLatin1String("map(\\d+)"), Qt::CaseInsensitive);

    TabletProfile tabletProfile  = d->profileManager.loadProfile(d->currentProfile);
    DeviceProfile stylusProfile  = tabletProfile.getDevice(DeviceType::Stylus);
    QString       screenSpace    = stylusProfile.getProperty(Property::ScreenSpace);
    int           screenCount    = X11Info::getNumberOfScreens();

    if (monitorRegExp.indexIn(screenSpace, 0) != -1) {
        int screen = monitorRegExp.cap(1).toInt() + 1;

        if (screen >= screenCount) {
            screenSpace = QLatin1String("full");
        } else {
            screenSpace = QString::fromLatin1("map%1").arg(screen);
        }
    } else {
        screenSpace = (screenCount == 1) ? QLatin1String("full") : QLatin1String("map0");
    }

    mapTabletToOutput(screenSpace);
}


void TabletHandler::onMapToFullScreen()
{
    mapTabletToOutput(QLatin1String("full"));
}



void TabletHandler::onMapToScreen1()
{
    mapTabletToOutput(QLatin1String("map0"));
}



void TabletHandler::onMapToScreen2()
{
    if (X11Info::getNumberOfScreens() >= 2) {
        mapTabletToOutput(QLatin1String("map1"));
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
        // Set rotation if auto-rotation is enabled.
        // This is necessary because the user could change the rotation of the tablet in the KCM
        // and then immediately switch to auto-rotation. In this case the tablet would still be
        // rotated in the wrong direction. Also on startup the current rotation has to be set
        // according to the current screen rotation.
        onScreenRotated(X11Info::getScreenRotation());

        // set profile
        d->currentProfile = profile;
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


const QString TabletHandler::getScreenSpaceMapping(const QString& screenSpace, const QString& screenMapping) const
{
    QString screen = screenSpace;
    QString mapping;

    if (screenSpace.contains(QLatin1String("full"), Qt::CaseInsensitive)) {
        screen = QLatin1String("desktop");
    }

    QString regexStr = QString::fromLatin1("%1:\\s*((?:-?\\d+\\s*){4})").arg(screen);
    QRegExp regex(regexStr, Qt::CaseInsensitive);

    if (regex.indexIn(screenMapping, 0) != -1) {
        mapping = regex.cap(1).trimmed();
    }

    return mapping;
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



void TabletHandler::mapTabletToOutput(const QString& output)
{
    Q_D( TabletHandler );

    if (!hasTablet()) {
        return; // we do not have a tablet
    }

    QString       absoluteMode   = QLatin1String("absolute");
    TabletProfile tabletProfile  = d->profileManager.loadProfile(d->currentProfile);
    DeviceProfile stylusProfile  = tabletProfile.getDevice(DeviceType::Stylus);
    DeviceProfile eraserProfile  = tabletProfile.getDevice(DeviceType::Eraser);
    DeviceProfile touchProfile   = tabletProfile.getDevice(DeviceType::Touch);

    QString       tabletMap      = stylusProfile.getProperty(Property::ScreenMap);
    QString       touchMap       = touchProfile.getProperty(Property::ScreenMap);
    QString       tabletMapping  = getScreenSpaceMapping(output, tabletMap);
    QString       touchMapping   = getScreenSpaceMapping(output, touchMap);

    // map the tablet to the output geometry
    // also set the tablet to absolute mode as output mapping does not work in relative mode
    // the profiles needs to be updated or else the current setting will be reset each time it is reloaded
    setProperty(DeviceType::Stylus, Property::Mode, absoluteMode);
    setProperty(DeviceType::Stylus, Property::ScreenSpace, output);
    setProperty(DeviceType::Stylus, Property::Area, tabletMapping);

    setProperty(DeviceType::Eraser, Property::Mode, absoluteMode);
    setProperty(DeviceType::Eraser, Property::ScreenSpace, output);
    setProperty(DeviceType::Eraser, Property::Area, tabletMapping);

    stylusProfile.setProperty(Property::Mode, absoluteMode);
    stylusProfile.setProperty(Property::ScreenSpace, output);
    stylusProfile.setProperty(Property::Area, tabletMapping);
    eraserProfile.setProperty(Property::Mode, absoluteMode);
    eraserProfile.setProperty(Property::ScreenSpace, output);
    eraserProfile.setProperty(Property::Area, tabletMapping);
    tabletProfile.setDevice(stylusProfile);
    tabletProfile.setDevice(eraserProfile);

    if (hasDevice(DeviceType::Touch)) {
        setProperty(DeviceType::Touch, Property::Mode, absoluteMode);
        setProperty(DeviceType::Touch, Property::ScreenSpace, output);
        setProperty(DeviceType::Touch, Property::Area, touchMapping);

        touchProfile.setProperty(Property::Mode, absoluteMode);
        touchProfile.setProperty(Property::ScreenSpace, output);
        touchProfile.setProperty(Property::Area, touchMapping);
        tabletProfile.setDevice(touchProfile);
    }

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



const QString TabletHandler::toggleMode(const DeviceType& type)
{
    QString mode = getProperty(type, Property::Mode);

    if( mode.compare( QLatin1String( "absolute" ), Qt::CaseInsensitive ) == 0 ) {
        mode = QLatin1String("relative");
        setProperty(type, Property::Mode, mode);
    } else {
        mode = QLatin1String("absolute");
        setProperty(type, Property::Mode, mode);
    }

    return mode;
}
