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

#include <QDir>
#include <QString>
#include <QTemporaryFile>

#include "deviceprofile.h"
#include "deviceprofiletestutils.h"
#include "tabletprofile.h"
#include "profilemanager.h"

#include "testprofilemanager.moc"

using namespace Wacom;

void TestProfileManager::testConfig()
{
    QTemporaryFile tempFile(QDir::tempPath() + QDir::separator() + QLatin1String("testprofilemanagerrc_XXXXXX"));
    QVERIFY(tempFile.open());
    tempFile.close();
    tempFile.setAutoRemove(true);

    DeviceProfile writeDeviceProfile1;
    DeviceType    writeDeviceProfile1Type = DeviceType::Stylus;
    DeviceProfile writeDeviceProfile2;
    DeviceType    writeDeviceProfile2Type = DeviceType::Eraser;

    DeviceProfileTestUtils::setValues(writeDeviceProfile1);
    DeviceProfileTestUtils::setValues(writeDeviceProfile2);
    writeDeviceProfile1.setDeviceType(writeDeviceProfile1Type);;
    writeDeviceProfile2.setDeviceType(writeDeviceProfile2Type);;

    TabletProfile writeTabletProfile(QLatin1String("Test Tablet Profile"));
    writeTabletProfile.setDevice(writeDeviceProfile1);
    writeTabletProfile.setDevice(writeDeviceProfile2);

    ProfileManager writeManager(tempFile.fileName());

    writeManager.readProfiles(QLatin1String("Test Tablet Device"));
    writeManager.saveProfile(writeTabletProfile);

    QVERIFY(writeManager.hasIdentifier(QLatin1String("Test Tablet Device")));
    QVERIFY(writeManager.hasProfile(writeTabletProfile.getName()));

    ProfileManager readManager(tempFile.fileName());
    QVERIFY(readManager.hasIdentifier(QLatin1String("Test Tablet Device")));

    readManager.readProfiles(QLatin1String("Test Tablet Device"));
    QVERIFY(readManager.hasProfile(writeTabletProfile.getName()));

    TabletProfile readTabletProfile = readManager.loadProfile(writeTabletProfile.getName());
    QCOMPARE(writeTabletProfile.getName(), readTabletProfile.getName());
    QVERIFY(readTabletProfile.listDevices().size() == 2);
    QVERIFY(readTabletProfile.hasDevice(writeDeviceProfile1Type));
    QVERIFY(readTabletProfile.hasDevice(writeDeviceProfile2Type));

    DeviceProfile readDeviceProfile1 = readTabletProfile.getDevice(writeDeviceProfile1Type);
    DeviceProfile readDeviceProfile2 = readTabletProfile.getDevice(writeDeviceProfile2Type);

    QCOMPARE(writeDeviceProfile1.getName(), readDeviceProfile1.getName());
    QCOMPARE(writeDeviceProfile2.getName(), readDeviceProfile2.getName());

    DeviceProfileTestUtils::assertValues(readDeviceProfile1, writeDeviceProfile1Type.key().toLatin1().constData());
    DeviceProfileTestUtils::assertValues(readDeviceProfile2, writeDeviceProfile2Type.key().toLatin1().constData());
}

