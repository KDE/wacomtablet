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

#include "testtabletservice.moc"

#include "dbustabletservice.h"
#include "dbustabletinterface.h"
#include "tablethandlermock.h"

using namespace Wacom;

TestTabletService::TestTabletService(QObject* parent) : QObject(parent)
{

}


bool TestTabletService::isDBusAvailable()
{
    DBusTabletInterface::instance().resetInterface();
    return DBusTabletInterface::instance().isValid();
}


void TestTabletService::test()
{
    // make sure the dbus interface is not already connected
    if (isDBusAvailable()) {
        QSKIP("D-Bus service already running! Please shut it down to run this test!", SkipAll);
        return;
    }

    // initialize D-Bus service
    TabletHandlerMock tabletHandlerMock;
    DBusTabletService tabletService(tabletHandlerMock);

    // reset D-Bus client interface
    DBusTabletInterface::instance().resetInterface();;

    DBusTabletInterface::instance().setProfile(QLatin1String("TestProfile"));

    QCOMPARE(tabletHandlerMock.m_profile, QLatin1String("TestProfile"));
}

