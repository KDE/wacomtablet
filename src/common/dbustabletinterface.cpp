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
#include "stringutils.h"

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


QDBusMessage DBusTabletInterface::getTabletList()
{
    return call( QLatin1String( "getTabletList" ) );
}

QDBusMessage DBusTabletInterface::getDeviceList(const QString &tabletId)
{
    return call( QLatin1String( "getDeviceList" ), tabletId );
}

QDBusMessage DBusTabletInterface::getDeviceName(const QString &tabletId, const DeviceType& device)
{
    return call( QLatin1String( "getDeviceName" ), tabletId, device.key() );
}

QDBusMessage DBusTabletInterface::getInformation(const QString &tabletId, const TabletInfo& info)
{
    return call( QLatin1String( "getInformation" ), tabletId, info.key() );
}


bool DBusTabletInterface::getInformationAsBool(const QString &tabletId, const TabletInfo& info)
{
    QDBusReply<QString> value = getInformation(tabletId, info);

    if (!value.isValid()) {
        return false;
    }

    return (StringUtils::asBool(value.value()));
}



int DBusTabletInterface::getInformationAsInt(const QString &tabletId, const TabletInfo& info)
{
    QDBusReply<QString> value = getInformation(tabletId, info);

    if (!value.isValid()) {
        return 0;
    }

    return value.value().toInt();
}



QString DBusTabletInterface::getInformationAsString(const QString &tabletId, const TabletInfo& info)
{
    QDBusReply<QString> value = getInformation(tabletId, info);

    if (!value.isValid()) {
        return QString();
    }

    return value.value();
}


QDBusMessage DBusTabletInterface::getProfile(const QString &tabletId)
{
    return call( QLatin1String( "getProfile" ), tabletId );
}



QDBusMessage DBusTabletInterface::getProperty(const QString &tabletId, const DeviceType& device, const Property& property)
{
    return call (QLatin1String("getProperty"), tabletId, device.key(), property.key());
}



QDBusMessage DBusTabletInterface::hasPadButtons(const QString &tabletId)
{
    return call( QLatin1String( "hasPadButtons" ), tabletId );
}



QDBusMessage DBusTabletInterface::isAvailable(const QString &tabletId)
{
    return call( QLatin1String( "isAvailable" ), tabletId );
}



QDBusMessage DBusTabletInterface::listProfiles(const QString &tabletId)
{
    return call( QLatin1String( "listProfiles" ), tabletId );
}



QDBusMessage DBusTabletInterface::setProfile(const QString &tabletId, const QString& profile)
{
    return call( QLatin1String( "setProfile" ), tabletId, profile );
}



QDBusMessage DBusTabletInterface::setProperty(const QString &tabletId, const DeviceType& device,
                                              const Property& property, const QString& value)
{
    return call (QLatin1String("setProperty"), tabletId, device.key(), property.key(), value);
}



QDBusMessage DBusTabletInterface::getProfileRotationList(const QString &tabletId) {

    return call ( QLatin1String("getProfileRotationList"), tabletId );
}

QDBusMessage DBusTabletInterface::setProfileRotationList(const QString &tabletId, const QStringList &rotationList) {

    return call ( QLatin1String("setProfileRotationList"), tabletId, rotationList );
}
