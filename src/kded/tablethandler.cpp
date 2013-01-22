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

#include "debug.h"

#include "tablethandler.h"

#include "tabletbackendinterface.h"
#include "tabletbackendfactory.h"
#include "tabletinfo.h"
#include "devicetype.h"
#include "stringutils.h"

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
    if(d->tabletBackend) {
        kDebug() << QString::fromLatin1("Ignoring tablet '%1' as another one is already connected.").arg(info.get(TabletInfo::TabletId));
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
    DeviceProfile stylusProfile = tabletProfile.getDevice(DeviceType::Stylus);
    DeviceProfile eraserProfile = tabletProfile.getDevice(DeviceType::Eraser);
    DeviceProfile touchProfile = tabletProfile.getDevice(DeviceType::Touch);

    if (StringUtils::asBool (stylusProfile.getProperty (Property::RotateWithScreen))) {

        kDebug() << "Rotate tablet :: " << screenRotation.key();

        //FIXME: there is a better way to transform rotation value to int hopefully?
        int intKey = 0;
        if( screenRotation.key() == QLatin1String("ccw") )
            intKey = 1;
        if( screenRotation.key() == QLatin1String("half") )
            intKey = 2;
        if( screenRotation.key() == QLatin1String("cw") )
            intKey = 3;

        // also save new rotation into the profile
        // See Bug: 312055
        setProperty( DeviceType::Stylus, Property::Rotate, QString::fromLatin1( "%1" ).arg( screenRotation.key() ) );
        stylusProfile.setProperty( Property::Rotate, QString::fromLatin1( "%1" ).arg( intKey ) );

        setProperty( DeviceType::Eraser, Property::Rotate, QString::fromLatin1( "%1" ).arg( screenRotation.key() ) );
        eraserProfile.setProperty( Property::Rotate, QString::fromLatin1( "%1" ).arg( intKey ) );

        if(d->tabletInformation.hasDevice (DeviceType::Touch)) {
            setProperty( DeviceType::Touch, Property::Rotate, QString::fromLatin1( "%1" ).arg( screenRotation.key() ) );
            touchProfile.setProperty( Property::Rotate, QString::fromLatin1( "%1" ).arg( intKey ) );
        }

        tabletProfile.setDevice(stylusProfile);
        tabletProfile.setDevice(eraserProfile);
        tabletProfile.setDevice(touchProfile);
        d->profileManager.saveProfile(tabletProfile);
    }
}


void TabletHandler::onTogglePenMode()
{
    Q_D( TabletHandler );

    if( !d->tabletBackend ) {
        return;
    }

    // also save the pen mode into the profile to remember the user selection after
    // the tablet was reconnected
    TabletProfile tabletProfile = d->profileManager.loadProfile(d->currentProfile);
    DeviceProfile stylusProfile = tabletProfile.getDevice(DeviceType::Stylus);
    DeviceProfile eraserProfile = tabletProfile.getDevice(DeviceType::Eraser);

    if(d->tabletInformation.hasDevice(DeviceType::Stylus)) {
        toggleMode(DeviceType::Stylus);
        stylusProfile.setProperty( Property::Mode, d->tabletBackend->getProperty(DeviceType::Stylus, Property::Mode) );
    }

    if(d->tabletInformation.hasDevice(DeviceType::Eraser)) {
        toggleMode(DeviceType::Eraser);
        eraserProfile.setProperty( Property::Mode, d->tabletBackend->getProperty(DeviceType::Stylus, Property::Mode) );
    }

    tabletProfile.setDevice(stylusProfile);
    tabletProfile.setDevice(eraserProfile);
    d->profileManager.saveProfile(tabletProfile);
}



void TabletHandler::onToggleTouch()
{
    Q_D( TabletHandler );

    if( !d->tabletBackend || !d->tabletInformation.hasDevice(DeviceType::Touch) ) {
        return;
    }

    QString touchMode = d->tabletBackend->getProperty(DeviceType::Touch, Property::Touch);

    // also save the touch on/off into the profile to remember the user selection after
    // the tablet was reconnected
    TabletProfile tabletProfile = d->profileManager.loadProfile(d->currentProfile);
    DeviceProfile touchProfile = tabletProfile.getDevice(DeviceType::Touch);

    if( touchMode.compare( QLatin1String( "off" ), Qt::CaseInsensitive) == 0 ) {
        d->tabletBackend->setProperty(DeviceType::Touch, Property::Touch, QLatin1String("on"));
        touchProfile.setProperty( Property::Touch, QLatin1String("on" ) );
    } else {
        d->tabletBackend->setProperty(DeviceType::Touch, Property::Touch, QLatin1String("off"));
        touchProfile.setProperty( Property::Touch, QLatin1String("off") );
    }

    tabletProfile.setDevice(touchProfile);
    d->profileManager.saveProfile(tabletProfile);
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

    if (!d->tabletBackend) {
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
