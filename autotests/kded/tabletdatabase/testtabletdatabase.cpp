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

#include "../kdedtestutils.h"
#include "src/common/tabletinformation.h"
#include "src/kded/tabletdatabase.h"

#include <QtCore/QList>
#include <QtCore/QMap>

#include <QtTest>
#include <KDE/KDebug>

#include <qtest_kde.h>

using namespace Wacom;

/**
 * @file testtabletdatabase.cpp
 *
 * @test UnitTest for ...
 */
class TestTabletDatabase: public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();

    void testLookupBackend();
    void testLookupDevice();
};


QTEST_KDEMAIN(TestTabletDatabase, GUI)

void TestTabletDatabase::initTestCase()
{
    QString companyFile   = QLatin1String("testtabletdatabase.companylist");
    QString dataDirectory = KdedTestUtils::getAbsoluteDir(companyFile);

    TabletDatabase::instance().setDatabase(dataDirectory, companyFile);
}


void TestTabletDatabase::testLookupBackend()
{
    QCOMPARE(TabletDatabase::instance().lookupBackend(QLatin1String("056A")), QLatin1String("wacom-tools"));
    QCOMPARE(TabletDatabase::instance().lookupBackend(QLatin1String("08ca")), QLatin1String("aiptek"));
}



void TestTabletDatabase::testLookupDevice()
{
    TabletInformation info;

    QVERIFY(TabletDatabase::instance().lookupTablet(QLatin1String("00df"), info));

    QCOMPARE(info.get(TabletInfo::CompanyId),   QLatin1String("056A"));
    QCOMPARE(info.get(TabletInfo::CompanyName), QLatin1String("Wacom Co., Ltd"));

    QCOMPARE(info.get(TabletInfo::TabletId),    QLatin1String("00DF"));
    QCOMPARE(info.get(TabletInfo::TabletModel), QLatin1String("CTH-670/K"));
    QCOMPARE(info.get(TabletInfo::TabletName),  QLatin1String("Bamboo Create"));

    QVERIFY(info.hasButtons());


    QMap<QString,QString> buttonMap = info.getButtonMap();

    QVERIFY(buttonMap.size() == 4);

    QList<QString> keys = buttonMap.keys();
    foreach (const QString& key, keys) {
        QVERIFY(!buttonMap.value(key).isEmpty());
    }
}



#include "testtabletdatabase.moc"
