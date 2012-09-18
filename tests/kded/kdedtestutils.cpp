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

#include "kdedtestutils.h"

#include <QtTest>
#include <QtCore>

using namespace Wacom;

void KdedTestUtils::assertTabletInformation(const TabletInformation& expectedInformation, const TabletInformation& actualInformation)
{
    // make sure the device list is equal
    QStringList expectedDeviceList = expectedInformation.getDeviceList();
    QStringList actualDeviceList   = actualInformation.getDeviceList();
    QVERIFY(expectedDeviceList.size() == actualDeviceList.size());

    for (int i = 0 ; i < expectedDeviceList.size() ; ++i) {
        QCOMPARE(expectedDeviceList.at(i), actualDeviceList.at(i));
    }

    // make sure the devices are equal
    foreach(const DeviceType& type, DeviceType::list()) {
        QCOMPARE(expectedInformation.getDeviceName(type), actualInformation.getDeviceName(type));
    }

    // compare tablet information
    QVERIFY(expectedInformation == actualInformation);

    foreach(const TabletInfo& info, TabletInfo::list()) {
        QCOMPARE(expectedInformation.get(info), actualInformation.get(info));
    }

    // check pad buttons
    QVERIFY(expectedInformation.hasButtons() == actualInformation.hasButtons());

    // check availability
    QVERIFY(expectedInformation.isAvailable() == actualInformation.isAvailable());
}
