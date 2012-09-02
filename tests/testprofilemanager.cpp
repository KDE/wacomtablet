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
    DeviceProfile writeDeviceProfile2;

    DeviceProfileTestUtils::setValues(writeDeviceProfile1);
    DeviceProfileTestUtils::setValues(writeDeviceProfile2);
    writeDeviceProfile1.setName(QLatin1String("Test Device Profile 1"));
    writeDeviceProfile2.setName(QLatin1String("Test Device Profile 2"));

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
    QVERIFY(readTabletProfile.hasDevice(writeDeviceProfile1.getName()));
    QVERIFY(readTabletProfile.hasDevice(writeDeviceProfile2.getName()));
    
    DeviceProfile readDeviceProfile1 = readTabletProfile.getDevice(writeDeviceProfile1.getName());
    DeviceProfile readDeviceProfile2 = readTabletProfile.getDevice(writeDeviceProfile2.getName());

    QCOMPARE(writeDeviceProfile1.getName(), readDeviceProfile1.getName());
    QCOMPARE(writeDeviceProfile2.getName(), readDeviceProfile2.getName());

    readDeviceProfile1.setName(QLatin1String("Name"));
    readDeviceProfile2.setName(QLatin1String("Name"));

    DeviceProfileTestUtils::assertValues(readDeviceProfile1);
    DeviceProfileTestUtils::assertValues(readDeviceProfile2);
}

