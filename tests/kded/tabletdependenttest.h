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

#ifndef TABLETDEPENDENTTEST_H
#define TABLETDEPENDENTTEST_H

#include "tabletinformation.h"

#include <QtCore>

// tablet settings this test suite expects
#define TESTTABLET_COMPANY_ID   "1234"
#define TESTTABLET_COMPANY_NAME "bla"

#define TESTTABLET_TABLET_ID    "1234"
#define TESTTABLET_TABLET_NAME  "Name"
#define TESTTABLET_TABLET_MODEL "Model"

#define TESTTABLET_HAS_BUTTONS  true

#define TESTTABLET_NAME_CURSOR  "cursor"
#define TESTTABLET_NAME_ERASER  "eraser"
#define TESTTABLET_NAME_PAD     "pad"
#define TESTTABLET_NAME_STYLUS  "stylus"
#define TESTTABLET_NAME_TOUCH   "touch"


namespace Wacom
{
/**
 * The base class for tablet dependent test suites.
 *
 * This class makes sure a tablet is actually connected to the system.
 */
class TabletDependentTest : public QObject
{
public:
    TabletDependentTest(QObject* parent = NULL);

    ~TabletDependentTest();

    //! Returns tablet information about the tablet which was detected.
    const TabletInformation& getTabletInformation() const;

    //! Returns if a tablet is available.
    bool isTabletAvailable() const;

private:

    void findTablet();

    TabletInformation m_tabletInformation;
    bool              m_isTabletAvailable;

}; // CLASS
}  // NAMEPSPACE
#endif // HEADER PROTECTION
