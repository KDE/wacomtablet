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
#include "testdeviceprofile.moc"

using namespace Wacom;

void TestDeviceProfile::testConstructor()
{
    DeviceProfile profile1;
    QVERIFY (profile1.getName().isEmpty());

    DeviceProfile profile2(DeviceType::Stylus);
    QCOMPARE (profile2.getDeviceType(), DeviceType::Stylus.key());
    QCOMPARE (profile2.getName(),       DeviceType::Stylus.key());
}

void TestDeviceProfile::testCopy()
{
    DeviceProfile profile1;
    DeviceProfile profile2;

    DeviceProfileTestUtils::setValues(profile1);
    profile2 = profile1;
    DeviceProfileTestUtils::assertValues(profile2);

    DeviceProfile profile3(profile2);
    DeviceProfileTestUtils::assertValues(profile3);
}

void TestDeviceProfile::testSetter()
{
    DeviceProfile profile;

    DeviceProfileTestUtils::setValues(profile);
    DeviceProfileTestUtils::assertValues(profile);
}

