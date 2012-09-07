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

#include "dbusdeviceinterface.h"

#include <QtCore/QString>
#include <QMutex>
#include <qtextstream.h>

using namespace Wacom;

// instanciate static class variable
DBusDeviceInterface* DBusDeviceInterface::m_instance = NULL;


QDBusArgument &Wacom::operator<<( QDBusArgument &argument, const Wacom::DeviceInformation &mystruct )
{
    argument.beginStructure();
    argument << mystruct.companyId
             << mystruct.companyName
             << mystruct.tabletId
             << mystruct.tabletName
             << mystruct.tabletModel
             << mystruct.deviceList
             << mystruct.padName
             << mystruct.stylusName
             << mystruct.eraserName
             << mystruct.cursorName
             << mystruct.touchName
             << mystruct.isDeviceAvailable
             << mystruct.hasPadButtons;
    argument.endStructure();
    return argument;
}

const QDBusArgument &Wacom::operator>>( const QDBusArgument &argument, Wacom::DeviceInformation &mystruct )
{
    argument.beginStructure();
    argument >> mystruct.companyId
             >> mystruct.companyName
             >> mystruct.tabletId
             >> mystruct.tabletName
             >> mystruct.tabletModel
             >> mystruct.deviceList
             >> mystruct.padName
             >> mystruct.stylusName
             >> mystruct.eraserName
             >> mystruct.cursorName
             >> mystruct.touchName
             >> mystruct.isDeviceAvailable
             >> mystruct.hasPadButtons;
    argument.endStructure();
    return argument;
}



DBusDeviceInterface::DBusDeviceInterface()
    : QDBusInterface( QLatin1String( "org.kde.Wacom" ), QLatin1String( "/Device" ), QLatin1String( "org.kde.WacomDevice" ) )
{
    // nothing to do
}


DBusDeviceInterface::DBusDeviceInterface(const DBusDeviceInterface&)
    : QDBusInterface( QLatin1String( "org.kde.Wacom" ), QLatin1String( "/Device" ), QLatin1String( "org.kde.WacomDevice" ) )
{
    // this class is a singleton - no copying allowed
}


DBusDeviceInterface& DBusDeviceInterface::operator= (const DBusDeviceInterface&)
{
    // this class is a singleton - no copying allowed
    return *this;
}


DBusDeviceInterface& DBusDeviceInterface::instance()
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

void DBusDeviceInterface::resetInterface()
{
    static QMutex mutex;
    mutex.lock();
    
    if (m_instance) {
        delete m_instance;
        m_instance = NULL;
    }

    m_instance = new DBusDeviceInterface();

    mutex.unlock();
}



QDBusMessage DBusDeviceInterface::getAllInformation()
{
    return call( QLatin1String( "getAllInformation" ) );
}


QDBusMessage DBusDeviceInterface::getDeviceList()
{
    return call( QLatin1String( "getDeviceList" ) );
}


QDBusMessage DBusDeviceInterface::getDeviceName(const DeviceType& device)
{
    return call( QLatin1String( "getDeviceName" ), device.key() );
}


QDBusMessage DBusDeviceInterface::getInformation(const DeviceInfo& info)
{
    return call( QLatin1String( "getInformation" ), info.key() );
}


QDBusMessage DBusDeviceInterface::getConfiguration(const QString& device, const QString& property)
{
    return call( QLatin1String( "getConfiguration" ), device, property );
}


QDBusMessage DBusDeviceInterface::hasPadButtons()
{
    return call( QLatin1String( "hasPadButtons" ) );
}


QDBusMessage DBusDeviceInterface::isTabletAvailable()
{
    return call( QLatin1String( "isTabletAvailable" ) );
}


QDBusMessage DBusDeviceInterface::setConfiguration(const QString& device, const QString& property, const QString& value)
{
    return call( QLatin1String( "setConfiguration" ), device, property, value );
}


