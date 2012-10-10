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

#include "dbustabletinterface.h"

#include <QtCore/QString>
#include <QtCore/QMetaType>
#include <QtCore/QMutex>
#include <QtDBus/QtDBus>

using namespace Wacom;

// instanciate static class members
DBusTabletInterface* DBusTabletInterface::m_instance = NULL;


DBusTabletInterface::DBusTabletInterface()
    : QDBusInterface( QLatin1String( "org.kde.Wacom" ), QLatin1String( "/Tablet" ), QLatin1String( "org.kde.Wacom" ) )
{
    DBusTabletInterface::registerMetaTypes();
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

void DBusTabletInterface::registerMetaTypes()
{
    // nothing to register for now
    // we keep this method so we have a central location to manage meta-types from

    //qDBusRegisterMetaType<Wacom::TabletInformation>();
}



QDBusMessage DBusTabletInterface::getDeviceList()
{
    return call( QLatin1String( "getDeviceList" ) );
}



QDBusMessage DBusTabletInterface::getDeviceName(const DeviceType& device)
{
    return call( QLatin1String( "getDeviceName" ), device.key() );
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

