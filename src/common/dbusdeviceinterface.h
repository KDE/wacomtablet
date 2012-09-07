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

#ifndef DBUSDEVICEINTERFACE_H
#define DBUSDEVICEINTERFACE_H

#include "deviceinfo.h"
#include "deviceinformation.h"
#include "devicetype.h"

#include <QtDBus/QDBusArgument>
#include <QtDBus/QDBusInterface>

namespace Wacom
{

QDBusArgument &operator<< (QDBusArgument &argument, const Wacom::DeviceInformation &mystruct);

const QDBusArgument &operator>> (const QDBusArgument &argument, Wacom::DeviceInformation &mystruct);

/**
 * A singleton class to access the D-Bus device interface.
 */
class DBusDeviceInterface : public QDBusInterface
{

public:

    /**
     * Returns a reference to the only instance of this class.
     */
    static DBusDeviceInterface& instance();

    /**
     * Resets the D-Bus interface connection.
     * This will invalidate all pointers to the current instance!
     * It will also destroy all signal-slot connections.
     */
    static void resetInterface();

    QDBusMessage getAllInformation();

    QDBusMessage getConfiguration(const QString& device, const QString& property);

    QDBusMessage getInformation(const DeviceInfo& info);

    QDBusMessage getDeviceList();

    QDBusMessage getDeviceName(const DeviceType& device);
    
    QDBusMessage hasPadButtons();

    QDBusMessage isTabletAvailable();

    QDBusMessage setConfiguration(const QString& device, const QString& property, const QString& value);


protected:

    /**
     * Protected default constructor.
     * Use \a instance to get an instance of this class.
     */
    DBusDeviceInterface();


private:

    /**
     * Copy constructor which does nothing.
     */
    DBusDeviceInterface(const DBusDeviceInterface&);

    /**
     * Copy operator which does nothing.
     */
    DBusDeviceInterface& operator= (const DBusDeviceInterface&);


    static DBusDeviceInterface* m_instance;

};     // CLASS
}      // NAMESPACE
#endif // HEADER PROTECTION
