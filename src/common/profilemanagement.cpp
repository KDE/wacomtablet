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

//Qt includes
#include <QRegExp>
#include <QDBusInterface>
#include <QDBusReply>

#include <QDebug>

using namespace Wacom;

ProfileManagement::ProfileManagement()
    : m_profileManager(QLatin1String("tabletprofilesrc"))
{
}

ProfileManagement::ProfileManagement(const QString &deviceName, const QString &touchName)
    : m_deviceName(deviceName)
    , m_touchName(touchName)
    , m_profileManager(QLatin1String("tabletprofilesrc"))
{
    qDebug() << "Create instance for :: " << deviceName << touchName;
}

ProfileManagement::ProfileManagement(const ProfileManagement& )
    : m_profileManager(QLatin1String("tabletprofilesrc"))
{
    // this class is a singleton - no copying allowed
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


ProfileManagement& ProfileManagement::instance(const QString &deviceName, const QString &touchName)
{
    static ProfileManagement instance(deviceName, touchName);
    return instance;
}

void ProfileManagement::setTabletId(const QString &tabletId)
{
    m_tabletId = tabletId;
}

void ProfileManagement::createNewProfile( const QString &profilename )
{
    if (profilename.isEmpty()) {
        qDebug() << "Can not create a profile with no name!";
    }

    //get information via DBus
    m_profileName = profilename;

    if( m_deviceName.isEmpty() ) {
        qDebug() << "no device information are found. Can't create a new profile";
        return;
    }

    qDebug() << "Creating a new profile for :: device:" << m_deviceName;

    m_profileManager.readProfiles(m_deviceName);
    TabletProfile tabletProfile = m_profileManager.loadProfile(profilename);
    DeviceProfile padDevice     = tabletProfile.getDevice(DeviceType::Pad);

    padDevice.setProperty(Property::AbsWheelUp, QLatin1String("4"));
    padDevice.setProperty(Property::AbsWheelDown, QLatin1String("5"));

    tabletProfile.setDevice(padDevice);


    DeviceProfile stylusDevice = tabletProfile.getDevice(DeviceType::Stylus);

    stylusDevice.setProperty(Property::Button1, QLatin1String("1"));
    stylusDevice.setProperty(Property::Button2, QLatin1String("2"));
    stylusDevice.setProperty(Property::Button3, QLatin1String("3"));
    stylusDevice.setProperty(Property::Mode, QLatin1String("absolute"));
    stylusDevice.setProperty(Property::PressureCurve, QLatin1String("0 0 100 100"));
    stylusDevice.setProperty(Property::RawSample, QLatin1String("4"));
    stylusDevice.setProperty(Property::Rotate, ScreenRotation::AUTO.key());
    stylusDevice.setProperty(Property::Suppress, QLatin1String("2"));
    stylusDevice.setProperty(Property::Threshold, QLatin1String("27"));

    tabletProfile.setDevice(stylusDevice);


    DeviceProfile eraserDevice = tabletProfile.getDevice(DeviceType::Eraser);

    eraserDevice.setProperty(Property::Button1, QLatin1String("1"));
    eraserDevice.setProperty(Property::Button2, QLatin1String("2"));
    eraserDevice.setProperty(Property::Button3, QLatin1String("3"));
    eraserDevice.setProperty(Property::Mode, QLatin1String("absolute"));
    eraserDevice.setProperty(Property::PressureCurve, QLatin1String("0 0 100 100"));
    eraserDevice.setProperty(Property::RawSample, QLatin1String("4"));
    eraserDevice.setProperty(Property::Rotate, ScreenRotation::AUTO.key());
    eraserDevice.setProperty(Property::Threshold, QLatin1String("27"));
    eraserDevice.setProperty(Property::Suppress, QLatin1String("2"));

    tabletProfile.setDevice(eraserDevice);


    // also add section for the touch if we have a touch tool
    if( !m_touchName.isEmpty() ) {

        DeviceProfile touchDevice = tabletProfile.getDevice(DeviceType::Touch);

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
        createNewProfile(QLatin1String("Default"));
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
    QDBusReply<QString> deviceName  = DBusTabletInterface::instance().getInformation(m_tabletId, TabletInfo::TabletName);

    if( deviceName.isValid() ) {
        m_deviceName = deviceName;
    }

    QDBusReply<QString> touchName = DBusTabletInterface::instance().getDeviceName(m_tabletId, DeviceType::Touch);
    if( touchName.isValid() ) {
        qDebug() << "touchName.isValid()::" << m_touchName << "value" << touchName.value();
        m_touchName = touchName.value();
    }
    else {
        m_touchName.clear();
    }
}
