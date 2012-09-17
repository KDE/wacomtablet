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

#include "dbustabletinterface.h"

#include <QtCore/QString>
#include <QMutex>

using namespace Wacom;

// instanciate static class members
DBusTabletInterface* DBusTabletInterface::m_instance = NULL;

QDBusArgument &Wacom::operator<<( QDBusArgument &argument, const Wacom::TabletInformation &mystruct )
{
    argument.beginStructure();
    argument << mystruct.xdeviceId
             << mystruct.companyId
             << mystruct.companyName
             << mystruct.tabletId
             << mystruct.tabletName
             << mystruct.tabletModel
             << mystruct.padName
             << mystruct.stylusName
             << mystruct.eraserName
             << mystruct.cursorName
             << mystruct.touchName
             << mystruct.isTabletAvailable
             << mystruct.hasPadButtons;
    argument.endStructure();
    return argument;
}

const QDBusArgument &Wacom::operator>>( const QDBusArgument &argument, Wacom::TabletInformation &mystruct )
{
    argument.beginStructure();
    argument >> mystruct.xdeviceId
             >> mystruct.companyId
             >> mystruct.companyName
             >> mystruct.tabletId
             >> mystruct.tabletName
             >> mystruct.tabletModel
             >> mystruct.padName
             >> mystruct.stylusName
             >> mystruct.eraserName
             >> mystruct.cursorName
             >> mystruct.touchName
             >> mystruct.isTabletAvailable
             >> mystruct.hasPadButtons;
    argument.endStructure();
    return argument;
}



DBusTabletInterface::DBusTabletInterface()
    : QDBusInterface( QLatin1String( "org.kde.Wacom" ), QLatin1String( "/Tablet" ), QLatin1String( "org.kde.Wacom" ) )
{
    // nothing to do
}


DBusTabletInterface::DBusTabletInterface(const DBusTabletInterface&)
    : QDBusInterface( QLatin1String( "org.kde.Wacom" ), QLatin1String( "/Tablet" ), QLatin1String( "org.kde.Wacom" ) )
{
    // this class is a singleton - no copying allowed
}


DBusTabletInterface& DBusTabletInterface::operator= (const DBusTabletInterface&)
{
    // this class is a singleton - no copying allowed
    return (*this);
}


DBusTabletInterface& DBusTabletInterface::instance()
{
    if (!m_instance) {
        static QMutex mutex;
        mutex.lock();

        if (!m_instance) {
            resetInterface();
        }

        mutex.unlock();
    }

    return *m_instance;
}


void DBusTabletInterface::resetInterface()
{
    static QMutex mutex;
    mutex.lock();

    if (m_instance) {
        delete m_instance;
        m_instance = NULL;
    }

    m_instance = new DBusTabletInterface();

    mutex.unlock();
}


QDBusMessage DBusTabletInterface::getDeviceList()
{
    return call( QLatin1String( "getDeviceList" ) );
}



QDBusMessage DBusTabletInterface::getDeviceName(const DeviceType& device)
{
    return call( QLatin1String( "getDeviceName" ), device.key() );
}



QDBusMessage DBusTabletInterface::getInformation()
{
    return call( QLatin1String( "getInformation" ) );
}



QDBusMessage DBusTabletInterface::getInformation(const TabletInfo& info)
{
    return call( QLatin1String( "getInformation" ), info.key() );
}



QDBusMessage DBusTabletInterface::getProfile()
{
    return call( QLatin1String( "getProfile" ) );
}



QDBusMessage DBusTabletInterface::getProperty(const DeviceType& device, const Property& property)
{
    return call (QLatin1String("getProperty"), device.key(), property.key());
}



QDBusMessage DBusTabletInterface::hasPadButtons()
{
    return call( QLatin1String( "hasPadButtons" ) );
}



QDBusMessage DBusTabletInterface::isAvailable()
{
    return call( QLatin1String( "isAvailable" ) );
}



QDBusMessage DBusTabletInterface::listProfiles()
{
    return call( QLatin1String( "listProfiles" ) );
}



QDBusMessage DBusTabletInterface::setProfile(const QString& profile)
{
    return call( QLatin1String( "setProfile" ), profile );
}



QDBusMessage DBusTabletInterface::setProperty(const DeviceType& device, const Property& property, const QString& value)
{
    return call (QLatin1String("setProperty"), device.key(), property.key(), value);
}

