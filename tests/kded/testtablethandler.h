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

#ifndef TESTTABLETHANDLER_H
#define TESTTABLETHANDLER_H

#include <QtTest>
#include <QtCore>

#include "tabletbackendmock.h"
#include "tablethandler.h"
#include "tabletinformation.h"

namespace Wacom
{
class TestTabletHandler : public QObject
{
    Q_OBJECT

public Q_SLOTS:

    void onNotify (const QString& eventId, const QString& title, const QString& message);

    void onProfileChanged (const QString& profile);

    void onTabletAdded (const TabletInformation& info);

    void onTabletRemoved();


private slots:

    void initTestCase();

    void test();


private:

    void testOnTabletAdded();

    void testOnTabletRemoved();

    void testSetProperty();


    QString            m_notifyEventId;
    QString            m_notifyTitle;
    QString            m_notifyMessage;

    QString            m_profileChanged;

    TabletInformation  m_tabletAddedInformation;
    bool               m_tabletAdded;
    bool               m_tabletRemoved;

    TabletHandler      m_tabletHandler;
    TabletBackendMock* m_backendMock;
};
}

QTEST_MAIN(Wacom::TestTabletHandler)

#endif
