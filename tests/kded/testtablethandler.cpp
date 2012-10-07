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

#include "testtablethandler.moc"

#include "kdedtestutils.h"
#include "tabletbackendfactory.h"


using namespace Wacom;


void TestTabletHandler::onNotify(const QString& eventId, const QString& title, const QString& message)
{
    m_notifyEventId = eventId;
    m_notifyTitle   = title;
    m_notifyMessage = message;
}



void TestTabletHandler::onProfileChanged(const QString& profile)
{
    m_profileChanged = profile;
}



void TestTabletHandler::onTabletAdded(const TabletInformation& info)
{
    m_tabletRemoved          = false;
    m_tabletAdded            = true;
    m_tabletAddedInformation = info;
}



void TestTabletHandler::onTabletRemoved()
{
    m_tabletAdded   = false;
    m_tabletRemoved = true;
}



void TestTabletHandler::initTestCase()
{
    m_tabletAdded   = false;
    m_tabletRemoved = false;
    m_backendMock   = NULL;

    QString profilePath = KdedTestUtils::getAbsolutePath(QLatin1String("testtablethandler.profilesrc"));
    QString configPath  = KdedTestUtils::getAbsolutePath(QLatin1String("testtablethandler.configrc"));
    m_tabletHandler = new TabletHandler(profilePath, configPath);

    TabletBackendFactory::setUnitTest(true);

    connect(m_tabletHandler, SIGNAL(notify(QString,QString,QString)), this, SLOT(onNotify(QString,QString,QString)));
    connect(m_tabletHandler, SIGNAL(profileChanged(QString)),         this, SLOT(onProfileChanged(QString)));
    connect(m_tabletHandler, SIGNAL(tabletAdded(TabletInformation)),  this, SLOT(onTabletAdded(TabletInformation)));
    connect(m_tabletHandler, SIGNAL(tabletRemoved()),                 this, SLOT(onTabletRemoved()));
}


void TestTabletHandler::cleanupTestCase()
{
    delete m_tabletHandler;
}



void TestTabletHandler::test()
{
    // only one test method as the test has to be executed in a specific order
    testOnTabletAdded();

    testSetProperty();

    testListProfiles();

    testSetProfile();

    testOnTogglePenMode();

    testOnToggleTouch();

    testOnScreenRotated();

    testOnTabletRemoved();
}



void TestTabletHandler::testListProfiles()
{
    QStringList profiles = m_tabletHandler->listProfiles();

    QCOMPARE(2, profiles.size());
    QVERIFY(profiles.contains(QLatin1String("default")));
    QVERIFY(profiles.contains(QLatin1String("test")));

    QWARN("testListProfiles(): PASSED!");
}


void TestTabletHandler::testOnScreenRotated()
{
    // reset screen rotation
    m_tabletHandler->setProperty(DeviceType::Stylus, Property::Rotate, TabletRotation::NONE.key());
    m_tabletHandler->setProperty(DeviceType::Eraser, Property::Rotate, TabletRotation::NONE.key());
    m_tabletHandler->setProperty(DeviceType::Touch, Property::Rotate, TabletRotation::NONE.key());

    // we need the test profile set as it has the required property
    QCOMPARE(m_profileChanged, QLatin1String("test"));

    // compare start paramters
    QCOMPARE(TabletRotation::NONE.key(), m_tabletHandler->getProperty(DeviceType::Eraser, Property::Rotate));
    QCOMPARE(TabletRotation::NONE.key(), m_tabletHandler->getProperty(DeviceType::Stylus, Property::Rotate));
    QCOMPARE(TabletRotation::NONE.key(), m_tabletHandler->getProperty(DeviceType::Touch, Property::Rotate));
    
    // rotate screen
    m_tabletHandler->onScreenRotated(TabletRotation::HALF);

    // validate result
    QCOMPARE(TabletRotation::HALF.key(), m_tabletHandler->getProperty(DeviceType::Eraser, Property::Rotate));
    QCOMPARE(TabletRotation::HALF.key(), m_tabletHandler->getProperty(DeviceType::Stylus, Property::Rotate));
    QCOMPARE(TabletRotation::HALF.key(), m_tabletHandler->getProperty(DeviceType::Touch, Property::Rotate));

    QWARN("testOnScreenRotated(): PASSED!");
}

void TestTabletHandler::testOnTabletAdded()
{
    QVERIFY(!m_tabletAdded);
    QVERIFY(!m_tabletRemoved);

    // Do not initialize the factory yet so it can not return a backend.
    TabletInformation basicInfo;
    m_notifyEventId = QString();
    m_notifyMessage = QString();
    m_notifyTitle   = QString();


    // if the tablet backend factory returns NULL adding a tablet should fail
    TabletBackendFactory::setUnitTest(true);
    TabletBackendFactory::setTabletBackendMock(NULL);

    m_tabletHandler->onTabletAdded(basicInfo);

    QVERIFY(!m_tabletAdded);
    QVERIFY(!m_tabletRemoved);
    QVERIFY(m_notifyEventId.isEmpty());
    QVERIFY(m_notifyMessage.isEmpty());
    QVERIFY(m_notifyTitle.isEmpty());
    QVERIFY(m_profileChanged.isEmpty());


    // create a valid backend for the factory to return
    m_backendMock = new TabletBackendMock();

    m_backendMock->m_tabletInformation.set(TabletInfo::CompanyId, QLatin1String("1234"));
    m_backendMock->m_tabletInformation.set(TabletInfo::CompanyName, QLatin1String("Company"));
    m_backendMock->m_tabletInformation.set(TabletInfo::TabletId, QLatin1String("4321"));
    m_backendMock->m_tabletInformation.set(TabletInfo::TabletModel, QLatin1String("Tablet Model"));
    m_backendMock->m_tabletInformation.set(TabletInfo::TabletName, QLatin1String("Bamboo Create"));
    m_backendMock->m_tabletInformation.setButtons(true);
    m_backendMock->m_tabletInformation.setAvailable(true);

    TabletBackendFactory::setTabletBackendMock(m_backendMock);


    // a device id of 0 should always fail

    m_tabletHandler->onTabletAdded(basicInfo);

    QVERIFY(!m_tabletAdded);
    QVERIFY(!m_tabletRemoved);
    QVERIFY(m_notifyEventId.isEmpty());
    QVERIFY(m_notifyMessage.isEmpty());
    QVERIFY(m_notifyTitle.isEmpty());
    QVERIFY(m_profileChanged.isEmpty());


    // add tablet again but this time with a valid device id
    // this should notify the user, emit a "tablet added" signal, notify the user and emit a "profile changed" signal

    m_tabletHandler->onTabletAdded(basicInfo);

    QVERIFY(!m_tabletRemoved);
    QVERIFY(m_tabletAdded);
    QVERIFY(!m_notifyEventId.isEmpty());
    QVERIFY(!m_notifyMessage.isEmpty());
    QVERIFY(!m_notifyTitle.isEmpty());
    QVERIFY(!m_profileChanged.isEmpty());
    QCOMPARE(QLatin1String("test"), m_profileChanged);
    QCOMPARE(QLatin1String("test"), m_backendMock->m_tabletProfile.getName());

    // TODO try to add the tablet again
    QWARN("testOnTabletAdded(): PASSED!");
}



void TestTabletHandler::testOnTabletRemoved()
{
    QVERIFY(!m_tabletRemoved);
    QVERIFY(m_tabletAdded);

    m_notifyEventId = QString();
    m_notifyMessage = QString();
    m_notifyTitle   = QString();

    TabletInformation info;

    // removing a different tablet then the one handled by the backend should do nothing

    m_tabletHandler->onTabletRemoved(info);

    QVERIFY(!m_tabletRemoved);
    QVERIFY(m_tabletAdded);
    QVERIFY(m_notifyEventId.isEmpty());
    QVERIFY(m_notifyMessage.isEmpty());
    QVERIFY(m_notifyTitle.isEmpty());

    // remove the currently managed tablet
    // this should emit a signal and notify the user

    m_tabletHandler->onTabletRemoved(info);

    QVERIFY(m_tabletRemoved);
    QVERIFY(!m_tabletAdded);
    QVERIFY(!m_notifyEventId.isEmpty());
    QVERIFY(!m_notifyMessage.isEmpty());
    QVERIFY(!m_notifyTitle.isEmpty());

    m_backendMock = NULL; // no longer valid, as tablet was removed

    // remove the device again.
    m_tabletRemoved = false;
    m_notifyEventId = QString();
    m_notifyMessage = QString();
    m_notifyTitle   = QString();

    m_tabletHandler->onTabletRemoved(info);

    QVERIFY(!m_tabletRemoved);
    QVERIFY(!m_tabletAdded);
    QVERIFY(m_notifyEventId.isEmpty());
    QVERIFY(m_notifyMessage.isEmpty());
    QVERIFY(m_notifyTitle.isEmpty());

    QWARN("testOnTabletRemoved(): PASSED!");
}



void TestTabletHandler::testOnTogglePenMode()
{
    m_backendMock->setProperty(DeviceType::Stylus, Property::Mode, QLatin1String("Absolute"));
    m_backendMock->setProperty(DeviceType::Eraser, Property::Mode, QLatin1String("Absolute"));

    QCOMPARE(QLatin1String("Absolute"), m_backendMock->getProperty(DeviceType::Eraser, Property::Mode));
    QCOMPARE(QLatin1String("Absolute"), m_backendMock->getProperty(DeviceType::Stylus, Property::Mode));

    m_tabletHandler->onTogglePenMode();

    QCOMPARE(QLatin1String("Relative"), m_backendMock->getProperty(DeviceType::Eraser, Property::Mode));
    QCOMPARE(QLatin1String("Relative"), m_backendMock->getProperty(DeviceType::Stylus, Property::Mode));

    m_tabletHandler->onTogglePenMode();

    QCOMPARE(QLatin1String("Absolute"), m_backendMock->getProperty(DeviceType::Eraser, Property::Mode));
    QCOMPARE(QLatin1String("Absolute"), m_backendMock->getProperty(DeviceType::Stylus, Property::Mode));

    QWARN("testOnTogglePenMode(): PASSED!");
}



void TestTabletHandler::testOnToggleTouch()
{
    m_tabletHandler->setProperty(DeviceType::Touch, Property::Touch, QLatin1String("off"));

    QCOMPARE(QLatin1String("off"), m_backendMock->getProperty(DeviceType::Touch, Property::Touch));
    QCOMPARE(QLatin1String("off"), m_tabletHandler->getProperty(DeviceType::Touch, Property::Touch));

    m_tabletHandler->onToggleTouch();

    QCOMPARE(QLatin1String("on"), m_backendMock->getProperty(DeviceType::Touch, Property::Touch));

    m_tabletHandler->onToggleTouch();

    QCOMPARE(QLatin1String("off"), m_backendMock->getProperty(DeviceType::Touch, Property::Touch));

    QWARN("testOnToggleTouch(): PASSED!");
}



void TestTabletHandler::testSetProfile()
{
    m_profileChanged.clear();
    m_notifyEventId.clear();
    m_notifyMessage.clear();
    m_notifyTitle.clear();

    // set invalid profile first
    m_tabletHandler->setProfile(QLatin1String("InvalidProfile"));

    QVERIFY(m_profileChanged.isEmpty());
    QVERIFY(!m_notifyEventId.isEmpty());
    QVERIFY(!m_notifyMessage.isEmpty());
    QVERIFY(!m_notifyTitle.isEmpty());

    // set a valid profile
    m_notifyEventId.clear();
    m_notifyMessage.clear();
    m_notifyTitle.clear();

    m_tabletHandler->setProfile(QLatin1String("default"));
    
    QVERIFY(m_notifyEventId.isEmpty());
    QVERIFY(m_notifyMessage.isEmpty());
    QVERIFY(m_notifyTitle.isEmpty());
    QCOMPARE(QLatin1String("default"), m_profileChanged);
    QCOMPARE(QLatin1String("default"), m_backendMock->m_tabletProfile.getName());


    // set the profile back to "test" so we can run this tests
    // multiple times in a row. Otherwise the test will fail as
    // cmake copies our test data only once.
    m_tabletHandler->setProfile(QLatin1String("test"));
    
    QWARN("testSetProfile(): PASSED!");
}



void TestTabletHandler::testSetProperty()
{
    QVERIFY(m_tabletAdded);

    m_tabletHandler->setProperty(DeviceType::Stylus, Property::Button1, Property::Button1.key());
    QCOMPARE(Property::Button1.key(), m_backendMock->getProperty(DeviceType::Stylus, Property::Button1));
    QCOMPARE(Property::Button1.key(), m_tabletHandler->getProperty(DeviceType::Stylus, Property::Button1));

    QWARN("testSetProperty(): PASSED!");
}


