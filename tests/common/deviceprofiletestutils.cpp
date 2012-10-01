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

#include "deviceprofiletestutils.h"

#include "deviceproperty.h"

using namespace Wacom;

void DeviceProfileTestUtils::assertValues(DeviceProfile& profile, const char* name)
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


void DeviceProfileTestUtils::setValues(DeviceProfile& profile)
{
    foreach(const DeviceProperty& property, DeviceProperty::list()) {
        profile.setProperty(property.id(), property.id().key());
    }

    if (profile.getDeviceType().isEmpty()) {
        profile.setDeviceType(DeviceType::Pad);
    }
}
