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

#include "tabletdependenttest.h"
#include "x11utils.h"

using namespace Wacom;

TabletDependentTest::TabletDependentTest(QObject* parent) : QObject(parent)
{
    m_isTabletAvailable = false;
    findTablet();
}

TabletDependentTest::~TabletDependentTest() { }


const TabletInformation& TabletDependentTest::getTabletInformation() const
{
    return m_tabletInformation;
}


bool TabletDependentTest::isTabletAvailable() const
{
    return m_isTabletAvailable;
}


void TabletDependentTest::findTablet()
{
    if (!X11Utils::findTabletDevice(m_tabletInformation)) {
        return;
    }

    if (QString::fromLatin1(TESTTABLET_COMPANY_ID).compare(m_tabletInformation.companyId) != 0) {
        return;
    }

    if (QString::fromLatin1(TESTTABLET_COMPANY_NAME).compare(m_tabletInformation.companyName) != 0) {
        return;
    }

    if (QString::fromLatin1(TESTTABLET_TABLET_ID).compare(m_tabletInformation.tabletId) != 0) {
        return;
    }

    if (QString::fromLatin1(TESTTABLET_TABLET_NAME).compare(m_tabletInformation.tabletName) != 0) {
        return;
    }

    if (QString::fromLatin1(TESTTABLET_TABLET_MODEL).compare(m_tabletInformation.tabletModel) != 0) {
        return;
    }

    if (QString::fromLatin1(TESTTABLET_NAME_CURSOR).compare(m_tabletInformation.cursorName) != 0) {
        return;
    }

    if (QString::fromLatin1(TESTTABLET_NAME_ERASER).compare(m_tabletInformation.eraserName) != 0) {
        return;
    }

    if (QString::fromLatin1(TESTTABLET_NAME_PAD).compare(m_tabletInformation.padName) != 0) {
        return;
    }

    if (QString::fromLatin1(TESTTABLET_NAME_STYLUS).compare(m_tabletInformation.stylusName) != 0) {
        return;
    }

    if (QString::fromLatin1(TESTTABLET_NAME_TOUCH).compare(m_tabletInformation.touchName) != 0) {
        return;
    }

    if (TESTTABLET_HAS_BUTTONS != m_tabletInformation.hasButtons()) {
        return;
    }
    
    m_isTabletAvailable = true;
}
