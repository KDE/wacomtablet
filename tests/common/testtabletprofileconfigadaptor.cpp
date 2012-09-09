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

#include <KDE/KSharedConfig>
#include <KDE/KConfigGroup>

#include "deviceprofile.h"
#include "deviceprofiletestutils.h"
#include "tabletprofile.h"
#include "tabletprofileconfigadaptor.h"

#include "testtabletprofileconfigadaptor.moc"

using namespace Wacom;

void TestTabletProfileConfigAdaptor::testConfig()
{
    QTemporaryFile tempFile(QDir::tempPath() + QDir::separator() + QLatin1String("testtabletprofileconfigadaptorrc_XXXXXX"));
    QVERIFY(tempFile.open());
    tempFile.close();
    tempFile.setAutoRemove(true);

    KSharedConfig::Ptr config = KSharedConfig::openConfig( tempFile.fileName(), KConfig::SimpleConfig );
    QVERIFY(config);

    TabletProfile writeTabletProfile(QLatin1String("TABLET"));
    KConfigGroup configGroup = KConfigGroup( config, writeTabletProfile.getName() );

    DeviceProfile writeDeviceProfile1;
    DeviceProfile writeDeviceProfile2;
    DeviceProfileTestUtils::setValues(writeDeviceProfile1);
    DeviceProfileTestUtils::setValues(writeDeviceProfile2);
    writeDeviceProfile1.setName(QLatin1String("DEVICE1"));
    writeDeviceProfile2.setName(QLatin1String("DEVICE2"));

    writeTabletProfile.setDevice(writeDeviceProfile1);
    writeTabletProfile.setDevice(writeDeviceProfile2);

    TabletProfileConfigAdaptor writeAdaptor(writeTabletProfile);
    writeAdaptor.saveConfig(configGroup);
    
    config->sync();

    TabletProfile readTabletProfile;
    TabletProfileConfigAdaptor readAdaptor(readTabletProfile);
    readAdaptor.loadConfig(configGroup);

    QCOMPARE(readTabletProfile.getName(), writeTabletProfile.getName());
    QVERIFY(readTabletProfile.listDevices().size() == writeTabletProfile.listDevices().size());
    QVERIFY(readTabletProfile.hasDevice(writeDeviceProfile1.getName()));
    QVERIFY(readTabletProfile.hasDevice(writeDeviceProfile2.getName()));

    DeviceProfile readDeviceProfile1 = readTabletProfile.getDevice(writeDeviceProfile1.getName());
    DeviceProfile readDeviceProfile2 = readTabletProfile.getDevice(writeDeviceProfile2.getName());

    readDeviceProfile1.setName(QLatin1String("Name"));
    readDeviceProfile2.setName(QLatin1String("Name"));
    DeviceProfileTestUtils::assertValues(readDeviceProfile1);
    DeviceProfileTestUtils::assertValues(readDeviceProfile2);
}
