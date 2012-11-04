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

#ifndef TESTDBUSTABLETSERVICE_H
#define TESTDBUSTABLETSERVICE_H

#include <QtTest>
#include <QtCore>

#include "dbustabletservice.h"
#include "tablethandlermock.h"

namespace Wacom
{
class TestDBusTabletService : public QObject
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

}; // CLASS
}  // NAMESPACE

QTEST_MAIN(Wacom::TestDBusTabletService)

#endif // HEADER PROTECTION
