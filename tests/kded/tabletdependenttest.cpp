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

#include "debug.h"
#include "tabletdependenttest.h"
#include "x11tabletfinder.h"

using namespace Wacom;

TabletDependentTest::TabletDependentTest(QObject* parent) : QObject(parent)
{
    m_isTabletAvailable = false;
}



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
    X11TabletFinder tabletFinder;
    m_isTabletAvailable = false;

    if (tabletFinder.scanDevices()) {
        m_tabletInformation = tabletFinder.getDevices().at(0);
        m_isTabletAvailable = true;
    }
}


void TabletDependentTest::printTabletInformation(const TabletInformation& info) const
{
    kError() << QLatin1String("\nTablet Information: ")
             << QString::fromLatin1("\n  Stylus Name  : '%1'").arg(info.getDeviceName(DeviceType::Stylus))
             << QString::fromLatin1("\n  Eraser Name  : '%1'").arg(info.getDeviceName(DeviceType::Eraser))
             << QString::fromLatin1("\n  Touch Name   : '%1'").arg(info.getDeviceName(DeviceType::Touch))
             << QString::fromLatin1("\n  Pad Name     : '%1'").arg(info.getDeviceName(DeviceType::Pad))
             << QString::fromLatin1("\n  Cursor Name  : '%1'").arg(info.getDeviceName(DeviceType::Cursor))

             << QString::fromLatin1("\n  Company ID   : '%1'").arg(info.get(TabletInfo::CompanyId))
             << QString::fromLatin1("\n  Company Name : '%1'").arg(info.get(TabletInfo::CompanyName))
             << QString::fromLatin1("\n  Tablet ID    : '%1'").arg(info.get(TabletInfo::TabletId))
             << QString::fromLatin1("\n  Tablet Name  : '%1'").arg(info.get(TabletInfo::TabletName))
             << QString::fromLatin1("\n  Tablet Model : '%1'").arg(info.get(TabletInfo::TabletModel))

             << QString::fromLatin1("\n");
}


void TabletDependentTest::initTestCase()
{
    findTablet();

    if (!isTabletAvailable()) {
        QSKIP("No tablet found! Can not run this tablet dependent test case!", SkipAll);
        return;
    }

    //printTabletInformation(getTabletInformation());

    initTestCaseDependent();
}


