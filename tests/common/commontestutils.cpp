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

#include <QtTest>
#include <QtCore>
#include <QString>

#include "commontestutils.h"

#include "deviceproperty.h"

using namespace Wacom;

const long    CommonTestUtils::DEVICEINFORMATION_DEVICE_ID     = 42;
const QString CommonTestUtils::DEVICEINFORMATION_DEVICE_NODE   = QLatin1String("/dev/input/event1");
const long    CommonTestUtils::DEVICEINFORMATION_PRODUCT_ID    = 1234;
const long    CommonTestUtils::DEVICEINFORMATION_TABLET_SERIAL = 123456;
const long    CommonTestUtils::DEVICEINFORMATION_VENDOR_ID     = 4321;

void CommonTestUtils::assertValues(const DeviceInformation& info, const DeviceType& expectedType, const QString& expectedName)
{
    QVERIFY  (info.getDeviceId() == DEVICEINFORMATION_DEVICE_ID);
    QCOMPARE (info.getDeviceNode(), DEVICEINFORMATION_DEVICE_NODE);
    QCOMPARE (info.getName(), expectedName);
    QVERIFY  (info.getProductId() == DEVICEINFORMATION_PRODUCT_ID);
    QVERIFY  (info.getTabletSerial() == DEVICEINFORMATION_TABLET_SERIAL);
    QVERIFY  (info.getType() == expectedType);
    QVERIFY  (info.getVendorId() == DEVICEINFORMATION_VENDOR_ID);
}



void CommonTestUtils::assertValues(DeviceProfile& profile, const char* name)
{
    foreach(const DeviceProperty& property, DeviceProperty::list()) {
        QCOMPARE(profile.getProperty(property.id()), property.id().key());
    }

    if (name != NULL) {
        QCOMPARE(profile.getName(), QLatin1String(name));
    } else {
        QCOMPARE(profile.getName(), DeviceType::Pad.key());
    }
}



void CommonTestUtils::setValues(DeviceInformation& info)
{
    info.setDeviceId (DEVICEINFORMATION_DEVICE_ID);
    info.setDeviceNode (DEVICEINFORMATION_DEVICE_NODE);
    info.setProductId (DEVICEINFORMATION_PRODUCT_ID);
    info.setTabletSerial (DEVICEINFORMATION_TABLET_SERIAL);
    info.setVendorId (DEVICEINFORMATION_VENDOR_ID);
}



void CommonTestUtils::setValues(DeviceProfile& profile)
{
    foreach(const DeviceProperty& property, DeviceProperty::list()) {
        profile.setProperty(property.id(), property.id().key());
    }

    if (profile.getDeviceType().isEmpty()) {
        profile.setDeviceType(DeviceType::Pad);
    }
}
