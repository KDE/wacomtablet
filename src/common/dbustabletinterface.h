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

#ifndef DBUSTABLETINTERFACE_H
#define DBUSTABLETINTERFACE_H

#include "tabletinfo.h"
#include "tabletinformation.h"
#include "devicetype.h"
#include "property.h"

#include <QtCore/QString>
#include <QtDBus/QDBusArgument>
#include <QtDBus/QDBusInterface>

namespace Wacom
{

//! Helper method for D-Bus to copy tablet information.
QDBusArgument &operator<< (QDBusArgument &argument, const Wacom::TabletInformation &mystruct);

//! Helper method for D-Bus to copy tablet information.
const QDBusArgument &operator>> (const QDBusArgument &argument, Wacom::TabletInformation &mystruct);


/**
 * A singleton class to access the D-Bus tablet interface.
 */
class DBusTabletInterface : public QDBusInterface
{

public:

    /**
     * Returns a reference to the only instance of this class.
     */
    static DBusTabletInterface& instance();

    /**
     * Resets the D-Bus interface connection.
     * This will invalidate all pointers to the current instance!
     * It will also destroy all signal-slot connections.
     */
    static void resetInterface();


    QDBusMessage getDeviceList();

    QDBusMessage getDeviceName (const DeviceType& device);

    QDBusMessage getInformation();

    QDBusMessage getInformation (const TabletInfo& info);

    QDBusMessage getProfile();

    QDBusMessage getProperty (const QString& device, const Property& property);

    QDBusMessage hasPadButtons();

    QDBusMessage isAvailable();

    QDBusMessage listProfiles();

    QDBusMessage setProfile (const QString& profile);

    QDBusMessage setProperty (const QString& device, const Property& property, const QString& value);


protected:

    /**
     * Protected default constructor.
     * Use \a instance to get an instance of this class.
     */
    DBusTabletInterface();


private:

    /**
     * Copy constructor which does nothing.
     */
    DBusTabletInterface(const DBusTabletInterface&);

    /**
     * Copy operator which does nothing.
     */
    DBusTabletInterface& operator= (const DBusTabletInterface&);

    static DBusTabletInterface* m_instance;

};     // CLASS
}      // NAMESPACE
#endif // HEADER PROTECTION
