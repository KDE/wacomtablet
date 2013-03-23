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

#include "profilemanagement.h"

// common
#include "dbustabletinterface.h"
#include "tabletinfo.h"
#include "devicetype.h"
#include "tabletprofile.h"
#include "property.h"
#include "screenrotation.h"

//KDE includes
#include <KDE/KDebug>

//Qt includes
#include <QtCore/QRegExp>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>

#include <QtCore/QDebug>

using namespace Wacom;

ProfileManagement::ProfileManagement()
    : m_profileManager(QLatin1String("tabletprofilesrc"))
{
    reload();
}

ProfileManagement::ProfileManagement(const ProfileManagement& )
    : m_profileManager(QLatin1String("tabletprofilesrc"))
{
    // this class is a singleton - no copying allowed
    reload();
}


ProfileManagement& ProfileManagement::operator=(const ProfileManagement& )
{
    // this class is a singleton - no copying allowed
    return *this;
}


ProfileManagement& ProfileManagement::instance()
{
    static ProfileManagement instance;
    return instance;
}


void ProfileManagement::createNewProfile( const QString &profilename )
{
    //get information via DBus
    QDBusReply<QString> deviceName = DBusTabletInterface::instance().getInformation(TabletInfo::TabletName);
    m_deviceName = deviceName;

    if( m_deviceName.isEmpty() ) {
        kDebug() << "no device information are found. Can't create a new profile";
        return;
    }

    kDebug() << "Creating a new profile for :: device:" << m_deviceName;

    m_profileManager.readProfiles(m_deviceName);
    TabletProfile tabletProfile = m_profileManager.loadProfile(profilename);
    DeviceProfile padDevice     = tabletProfile.getDevice(DeviceType::Pad);

    padDevice.setProperty(Property::AbsWheelUp, QLatin1String("4"));
    padDevice.setProperty(Property::AbsWheelDown, QLatin1String("5"));

    tabletProfile.setDevice(padDevice);


    DeviceProfile stylusDevice = tabletProfile.getDevice(DeviceType::Stylus);

    stylusDevice.setProperty(Property::Area, QLatin1String("-1 -1 -2 -2")); // Area is in coordinate format! => -2 -2
    stylusDevice.setProperty(Property::Button2, QLatin1String("2"));
    stylusDevice.setProperty(Property::Button3, QLatin1String("3"));
    stylusDevice.setProperty(Property::Mode, QLatin1String("absolute"));
    stylusDevice.setProperty(Property::PressureCurve, QLatin1String("0 0 100 100"));
    stylusDevice.setProperty(Property::Rotate, ScreenRotation::AUTO.key());
    stylusDevice.setProperty(Property::Threshold, QLatin1String("27"));

    tabletProfile.setDevice(stylusDevice);


    DeviceProfile eraserDevice = tabletProfile.getDevice(DeviceType::Eraser);

    eraserDevice.setProperty(Property::Area, QLatin1String("-1 -1 -2 -2")); // Area is in coordinate format! => -2 -2
    eraserDevice.setProperty(Property::Button2, QLatin1String("2"));
    eraserDevice.setProperty(Property::Button3, QLatin1String("3"));
    eraserDevice.setProperty(Property::Mode, QLatin1String("absolute"));
    eraserDevice.setProperty(Property::PressureCurve, QLatin1String("0 0 100 100"));
    eraserDevice.setProperty(Property::Rotate, ScreenRotation::AUTO.key());
    eraserDevice.setProperty(Property::Threshold, QLatin1String("27"));

    tabletProfile.setDevice(eraserDevice);


    // also add section for the touch if we have a touch tool
    QDBusReply<QString> touchName = DBusTabletInterface::instance().getDeviceName(DeviceType::Touch);

    if( !touchName.value().isEmpty() ) {

        DeviceProfile touchDevice = tabletProfile.getDevice(DeviceType::Touch);

        touchDevice.setProperty(Property::Area, QLatin1String("-1 -1 -2 -2")); // Area is in coordinate format! => -2 -2
        touchDevice.setProperty(Property::Gesture, QLatin1String("on"));
        touchDevice.setProperty(Property::InvertScroll, QLatin1String("off"));
        touchDevice.setProperty(Property::Mode, QLatin1String("absolute"));
        touchDevice.setProperty(Property::Rotate, ScreenRotation::AUTO.key());
        touchDevice.setProperty(Property::ScrollDistance, QLatin1String("20"));
        touchDevice.setProperty(Property::TapTime, QLatin1String("250"));
        touchDevice.setProperty(Property::Touch, QLatin1String("on"));
        touchDevice.setProperty(Property::ZoomDistance, QLatin1String("50"));

        tabletProfile.setDevice(touchDevice);
    }

    m_profileManager.saveProfile(tabletProfile);
}

const QStringList ProfileManagement::availableProfiles()
{
    m_profileManager.readProfiles(m_deviceName);
    return m_profileManager.listProfiles();
}

void ProfileManagement::deleteProfile()
{
    m_profileManager.readProfiles(m_deviceName);
    m_profileManager.deleteProfile(m_profileName);

    m_profileName.clear();
    m_profileManager.reload();

    if (m_profileManager.listProfiles().isEmpty()) {
        createNewProfile();
        m_profileManager.reload();
    }
}

DeviceProfile ProfileManagement::loadDeviceProfile(const DeviceType& device)
{
    m_profileManager.readProfiles(m_deviceName);
    return m_profileManager.loadProfile(m_profileName).getDevice(device);
}


bool ProfileManagement::saveDeviceProfile(const DeviceProfile& profile)
{
    if (!m_profileManager.readProfiles(m_deviceName)) {
        return false;
    }

    TabletProfile tabletProfile = m_profileManager.loadProfile(m_profileName);
    tabletProfile.setDevice(profile);

    return m_profileManager.saveProfile(tabletProfile);
}



void ProfileManagement::setProfileName( const QString &name )
{
    m_profileName = name;
}

QString ProfileManagement::profileName() const
{
    return m_profileName;
}


void ProfileManagement::reload()
{
    QDBusReply<QString> deviceName  = DBusTabletInterface::instance().getInformation(TabletInfo::TabletName);

    if( deviceName.isValid() ) {
        m_deviceName = deviceName;
    }
}
