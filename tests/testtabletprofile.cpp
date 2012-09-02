/*
 * Copyright 2012 Alexander Maret-Huskinson <alex@maret.de>
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
    DeviceProfile deviceProfile(QLatin1String("DEVICE"));
    TabletProfile tabletProfile(QLatin1String("TABLET"));
    tabletProfile.setDevice(deviceProfile);

    QVERIFY(tabletProfile.hasDevice(deviceProfile.getName()));
    QVERIFY(tabletProfile.listDevices().size() == 1);

    tabletProfile.clearDevices();

    QVERIFY(tabletProfile.listDevices().size() == 0);
}

void TestTabletProfile::testCopy()
{
    DeviceProfile profile1;
    DeviceProfile profile2;
    DeviceProfileTestUtils::setValues(profile1);
    DeviceProfileTestUtils::setValues(profile2);

    // names have to be set AFTER DeviceProfileTestUtils::setValues()
    profile1.setName(QLatin1String("DEVICE1"));
    profile2.setName(QLatin1String("DEVICE2"));

    TabletProfile tabletProfile(QLatin1String("TABLET"));
    tabletProfile.setDevice(profile1);
    tabletProfile.setDevice(profile2);

    TabletProfile tabletProfileCopy = tabletProfile;

    QCOMPARE(tabletProfileCopy.getName(), tabletProfile.getName());
    QVERIFY(tabletProfileCopy.hasDevice(profile1.getName()));
    QVERIFY(tabletProfileCopy.hasDevice(profile2.getName()));
    QVERIFY(tabletProfileCopy.listDevices().contains(profile1.getName()));
    QVERIFY(tabletProfileCopy.listDevices().contains(profile2.getName()));

    DeviceProfile profile1Copy = tabletProfileCopy.getDevice(profile1.getName());
    DeviceProfile profile2Copy = tabletProfileCopy.getDevice(profile2.getName());

    // reset names back to "name" before asserting values
    profile1Copy.setName(QLatin1String("Name"));
    profile2Copy.setName(QLatin1String("Name"));

    DeviceProfileTestUtils::assertValues(profile1Copy);
    DeviceProfileTestUtils::assertValues(profile2Copy);
}

void TestTabletProfile::testSetDevice()
{
    DeviceProfile deviceProfile(QLatin1String("DEVICE"));
    DeviceProfileTestUtils::setValues(deviceProfile);

    TabletProfile tabletProfile(QLatin1String("TABLET"));
    tabletProfile.setDevice(deviceProfile);

    QVERIFY(tabletProfile.hasDevice(deviceProfile.getName()));
    QVERIFY(tabletProfile.listDevices().size() == 1);

    DeviceProfile getProfile = tabletProfile.getDevice(deviceProfile.getName());
    DeviceProfileTestUtils::assertValues(getProfile);
}

