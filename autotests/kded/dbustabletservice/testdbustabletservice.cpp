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

#include "../tablethandlermock.h"

#include "src/kded/dbustabletservice.h"

#include "src/common/dbustabletinterface.h"
#include "src/common/tabletinformation.h"

#include <QtTest>
#include <KDE/KDebug>

#include <qtest_kde.h>

using namespace Wacom;

/**
 * @file testdbustabletservice.cpp
 *
 * @test UnitTest for ...
 */
class TestDBusTabletService: public QObject
{
    Q_OBJECT

public:
    void assertTabletInformation(const TabletInformation& expectedInformation) const;

public slots:
    void onProfileChanged(const QString& profile);
    void onTabletAdded();
    void onTabletRemoved();


private slots:
    //! Run once before all tests.
    void initTestCase();

    void testListProfiles();
    void testOnTabletAdded();
    void testOnTabletRemoved();
    void testSetProfile();
    void testSetProperty();

    //! Run once after all tests.
    void cleanupTestCase();


private:

    TabletHandlerMock  m_tabletHandlerMock;
    DBusTabletService* m_tabletService;

    QString            m_profileWasChangedTo;
    bool               m_tabletWasAdded;
    bool               m_tabletWasRemoved;
};

QTEST_KDEMAIN(TestDBusTabletService, GUI)


void TestDBusTabletService::assertTabletInformation(const TabletInformation& expectedInformation) const
{
    QDBusReply<QString> actualString;
    QDBusReply<bool> actualBool;

    // make sure the device list is equal
    QStringList             expectedDeviceList = expectedInformation.getDeviceList();
    QDBusReply<QStringList> actualDeviceList   = DBusTabletInterface::instance().getDeviceList(QLatin1String("fakeId"));
    QVERIFY(actualDeviceList.isValid());

    for (int i = 0 ; i < expectedDeviceList.size() ; ++i) {
        QCOMPARE(expectedDeviceList.at(i), actualDeviceList.value().at(i));
    }

    // make sure the devices are equal
    foreach(const DeviceType& type, DeviceType::list()) {
        actualString = DBusTabletInterface::instance().getDeviceName(QLatin1String("fakeId"), type);
        QVERIFY(actualString.isValid());
        QCOMPARE(expectedInformation.getDeviceName(type), actualString.value());
    }

    // compare tablet information
    foreach(const TabletInfo& info, TabletInfo::list()) {
        actualString = DBusTabletInterface::instance().getInformation(QLatin1String("fakeId"), info);
        QVERIFY(actualString.isValid());
        QCOMPARE(expectedInformation.get(info), actualString.value());
    }

    // check pad buttons
    actualBool = DBusTabletInterface::instance().hasPadButtons(QLatin1String("fakeId"));
    QVERIFY(actualBool.isValid());
    QVERIFY(expectedInformation.hasButtons() == actualBool.value());

    // check availability
    actualBool = DBusTabletInterface::instance().isAvailable(QLatin1String("fakeId"));
    QVERIFY(actualBool.isValid());
    QVERIFY(expectedInformation.isAvailable() == actualBool.value());
}



void TestDBusTabletService::onProfileChanged(const QString& profile)
{
    m_profileWasChangedTo = profile;
}



void TestDBusTabletService::onTabletAdded()
{
    m_tabletWasAdded = true;
}



void TestDBusTabletService::onTabletRemoved()
{
    m_tabletWasRemoved = true;
}



void TestDBusTabletService::initTestCase()
{
    m_tabletService       = NULL;
    m_profileWasChangedTo.clear();
    m_tabletWasAdded      = false;
    m_tabletWasRemoved    = false;

    // make sure the D-Bus service is not already registered
    DBusTabletInterface::instance().resetInterface();

    if (DBusTabletInterface::instance().isValid()) {
        QSKIP("D-Bus service already running! Please shut it down to run this test!", SkipAll);
        return;
    }

    // register D-Bus service
    m_tabletService = new DBusTabletService(m_tabletHandlerMock);

    // reset D-Bus client
    DBusTabletInterface::instance().resetInterface();;

    // connect tablet service to us
    connect(m_tabletService, SIGNAL(profileChanged(const QString&)), this, SLOT(onProfileChanged(const QString&)));
    connect(m_tabletService, SIGNAL(tabletAdded()),                  this, SLOT(onTabletAdded()));
    connect(m_tabletService, SIGNAL(tabletRemoved()),                this, SLOT(onTabletRemoved()));

    // connect tablet handler to tablet service
    connect(&m_tabletHandlerMock, SIGNAL(profileChanged(const QString&)),        m_tabletService, SLOT(onProfileChanged(const QString&)));
    connect(&m_tabletHandlerMock, SIGNAL(tabletAdded(const TabletInformation&)), m_tabletService, SLOT(onTabletAdded(const TabletInformation&)));
    connect(&m_tabletHandlerMock, SIGNAL(tabletRemoved()),                       m_tabletService, SLOT(onTabletRemoved()));
}



void TestDBusTabletService::cleanupTestCase()
{
    if (m_tabletService) {
        delete m_tabletService;
        m_tabletService = NULL;
    }
}



void TestDBusTabletService::testListProfiles()
{
    m_tabletHandlerMock.m_profiles.clear();

    QDBusReply<QStringList> profileList = DBusTabletInterface::instance().listProfiles(QLatin1String("fakeId"));
    QVERIFY(profileList.isValid());
    QVERIFY(profileList.value().isEmpty());

    m_tabletHandlerMock.m_profiles.append(QLatin1String("Test Profile 1"));
    m_tabletHandlerMock.m_profiles.append(QLatin1String("Test Profile 2"));

    profileList = DBusTabletInterface::instance().listProfiles(QLatin1String("fakeId"));

    QVERIFY(profileList.isValid());
    QCOMPARE(profileList.value().at(0), QLatin1String("Test Profile 1"));
    QCOMPARE(profileList.value().at(1), QLatin1String("Test Profile 2"));
}



void TestDBusTabletService::testOnTabletAdded()
{
    TabletInformation expectedInformation;

    foreach(const TabletInfo& tabletInfo, TabletInfo::list()) {
        expectedInformation.set(tabletInfo, tabletInfo.key());
    }

    expectedInformation.setAvailable(false); // this should be set to true automatically

    m_tabletWasAdded = false;
    m_tabletHandlerMock.emitTabletAdded(expectedInformation);

    QVERIFY(m_tabletWasAdded);

    expectedInformation.setAvailable(true);
    assertTabletInformation(expectedInformation);

    QDBusReply<bool> isAvail = DBusTabletInterface::instance().isAvailable(QLatin1String("fakeId"));
    QVERIFY(isAvail.isValid());
    QVERIFY(isAvail.value());
}



void TestDBusTabletService::testOnTabletRemoved()
{
    TabletInformation expectedInformation;
    expectedInformation.setAvailable(false);

    m_tabletWasRemoved = false;
    m_tabletHandlerMock.emitTabletRemoved();
    assertTabletInformation(expectedInformation);
    QVERIFY(m_tabletWasRemoved);
}



void TestDBusTabletService::testSetProfile()
{
    QString             expectedProfile;
    QDBusReply<QString> actualProfile;

    // set new profile and make sure a signal was emitted by D-Bus
    expectedProfile = QLatin1String("Test Profile");
    DBusTabletInterface::instance().setProfile(QLatin1String("fakeId"), expectedProfile);
    QCOMPARE(expectedProfile, m_tabletHandlerMock.m_profile);
    QCOMPARE(expectedProfile, m_profileWasChangedTo);

    // the new profile should also be returned by getProfile
    actualProfile = DBusTabletInterface::instance().getProfile(QLatin1String("fakeId"));
    QVERIFY(actualProfile.isValid());
    QCOMPARE(expectedProfile, actualProfile.value());

    // when the tablet is removed, the profile should be reset
    // however the tablet handler should be unchanged and no signal should be emitted by D-Bus
    m_tabletHandlerMock.emitTabletRemoved();
    actualProfile = DBusTabletInterface::instance().getProfile(QLatin1String("fakeId"));
    QVERIFY(actualProfile.isValid());
    QVERIFY(actualProfile.value().isEmpty());
    QCOMPARE(expectedProfile, m_tabletHandlerMock.m_profile);
    QCOMPARE(expectedProfile, m_profileWasChangedTo);

    // set another profile
    expectedProfile = QLatin1String("New Profile");
    DBusTabletInterface::instance().setProfile(QLatin1String("fakeId"), expectedProfile);
    QCOMPARE(expectedProfile, m_tabletHandlerMock.m_profile);
    QCOMPARE(expectedProfile, m_profileWasChangedTo);

    // the new profile should also be returned by getProfile
    actualProfile = DBusTabletInterface::instance().getProfile(QLatin1String("fakeId"));
    QVERIFY(actualProfile.isValid());
    QCOMPARE(expectedProfile, actualProfile.value());

    // clear the profile manually
    expectedProfile.clear();
    DBusTabletInterface::instance().setProfile(QLatin1String("fakeId"), expectedProfile);
    QCOMPARE(expectedProfile, m_tabletHandlerMock.m_profile);
    QCOMPARE(expectedProfile, m_profileWasChangedTo);

    // the new profile should also be returned by getProfile
    actualProfile = DBusTabletInterface::instance().getProfile(QLatin1String("fakeId"));
    QVERIFY(actualProfile.isValid());
    QCOMPARE(expectedProfile, actualProfile.value());
}



void TestDBusTabletService::testSetProperty()
{
    DeviceType expectedDevice   = DeviceType::Stylus;
    Property   expectedProperty = Property::Button1;
    QString    expectedValue    = QLatin1String("My Value");

    // set the property
    DBusTabletInterface::instance().setProperty(QLatin1String("fakeId"), expectedDevice, expectedProperty, expectedValue);

    QCOMPARE(expectedDevice.key(), m_tabletHandlerMock.m_deviceType);
    QCOMPARE(expectedProperty.key(), m_tabletHandlerMock.m_property);
    QCOMPARE(expectedValue, m_tabletHandlerMock.m_propertyValue);

    // try to get the property
    QDBusReply<QString> actualValue = DBusTabletInterface::instance().getProperty(QLatin1String("fakeId"), expectedDevice, expectedProperty);

    QVERIFY(actualValue.isValid());
    QCOMPARE(expectedValue, actualValue.value());
}



#include "testdbustabletservice.moc"
