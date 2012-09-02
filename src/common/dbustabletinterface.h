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

#include <QtDBus/QDBusInterface>

namespace Wacom
{

class QDbusMessage;

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


    // TODO these should return the correct data types and throw an exception if there is a problem

    QDBusMessage profile();

    QDBusMessage profileList();

    QDBusMessage setProfile(const QString& profile);

    QDBusMessage tabletAvailable();


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
