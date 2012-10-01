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

#include "deviceprofile.h"
#include "deviceprofiletestutils.h"
#include "tabletprofile.h"

#include "testtabletprofile.moc"

using namespace Wacom;

void TestTabletProfile::testConstructor()
{
    TabletProfile profile1;
    QVERIFY( profile1.getName().isEmpty());

    QLatin1String name("TEST");
    TabletProfile profile2(name);
    QCOMPARE(profile2.getName(), name);
}

void TestTabletProfile::testClearDevices()
{
    DeviceType    deviceType = DeviceType::Touch;
    DeviceProfile deviceProfile(deviceType);
    TabletProfile tabletProfile(QLatin1String("TABLET"));
    tabletProfile.setDevice(deviceProfile);

    QVERIFY(tabletProfile.hasDevice(deviceType));
    QVERIFY(tabletProfile.listDevices().size() == 1);

    tabletProfile.clearDevices();

    QVERIFY(tabletProfile.listDevices().size() == 0);
}

void TestTabletProfile::testCopy()
{
    DeviceProfile profile1;
    DeviceType    profile1Type = DeviceType::Stylus;
    DeviceProfile profile2;
    DeviceType    profile2Type = DeviceType::Eraser;
    DeviceProfileTestUtils::setValues(profile1);
    DeviceProfileTestUtils::setValues(profile2);

    // names have to be set AFTER DeviceProfileTestUtils::setValues()
    profile1.setDeviceType(profile1Type);
    profile2.setDeviceType(profile2Type);;

    TabletProfile tabletProfile(QLatin1String("TABLET"));
    tabletProfile.setDevice(profile1);
    tabletProfile.setDevice(profile2);

    TabletProfile tabletProfileCopy = tabletProfile;

    QCOMPARE(tabletProfileCopy.getName(), tabletProfile.getName());
    QVERIFY(tabletProfileCopy.hasDevice(profile1Type));
    QVERIFY(tabletProfileCopy.hasDevice(profile2Type));
    QVERIFY(tabletProfileCopy.listDevices().contains(profile1.getName()));
    QVERIFY(tabletProfileCopy.listDevices().contains(profile2.getName()));

    DeviceProfile profile1Copy = tabletProfileCopy.getDevice(profile1Type);
    DeviceProfile profile2Copy = tabletProfileCopy.getDevice(profile2Type);

    DeviceProfileTestUtils::assertValues(profile1Copy, profile1Type.key().toLatin1().constData());
    DeviceProfileTestUtils::assertValues(profile2Copy, profile2Type.key().toLatin1().constData());
}

void TestTabletProfile::testSetDevice()
{
    DeviceType    deviceType = DeviceType::Cursor;
    DeviceProfile deviceProfile;
    DeviceProfileTestUtils::setValues(deviceProfile);
    deviceProfile.setDeviceType(deviceType);

    TabletProfile tabletProfile(QLatin1String("TABLET"));
    tabletProfile.setDevice(deviceProfile);

    QVERIFY(tabletProfile.hasDevice(deviceType));
    QVERIFY(tabletProfile.listDevices().size() == 1);

    DeviceProfile getProfile = tabletProfile.getDevice(deviceType);
    DeviceProfileTestUtils::assertValues(getProfile, deviceType.key().toLatin1().constData());
}

