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

#ifndef DBUSTABLETINTERFACE_H
#define DBUSTABLETINTERFACE_H

#include "tabletinfo.h"
#include "devicetype.h"
#include "property.h"

#include <QtCore/QString>
#include <QtDBus/QDBusArgument>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>

// D-Bus interface metatypes
//Q_DECLARE_METATYPE(Wacom::TabletInformation)

namespace Wacom
{

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

    /**
     * Registers d-bus metatypes.
     *
     * This method should not be called directly. It is used by this
     * interface and the d-bus service class to register the meta
     * types. This is just to have one single location where all
     * metatypes are managed.
     */
    static void registerMetaTypes();

    /**
     * @see DBusTabletService::getDeviceList()
     */
    QDBusMessage getDeviceList();

    /**
     * @see DBusTabletService::getDeviceName(const DeviceType&)
     */
    QDBusMessage getDeviceName (const DeviceType& device);

    /**
     * @see DBusTabletService::getInformation(const TabletInfo&)
     */
    QDBusMessage getInformation (const TabletInfo& info);

    /**
     * Gets a tablet information value as bool. If there is a problem
     * getting the value or if the value is not of boolean type, false
     * will be returned.
     *
     * @param info The information property to get.
     *
     * @return True if the value is true, else false.
     */
    bool getInformationAsBool (const TabletInfo& info);

    /**
     * Gets a tablet information value as integer. If there is a problem
     * getting the value or if the value is not an integer, 0 will be returned.
     *
     * @param info The information property to get.
     *
     * @return The requested value as integer, else 0.
     */
    int getInformationAsInt (const TabletInfo& info);

    /**
     * Gets a tablet information value as string. If there is a problem getting
     * the value, an empty string will be returned.
     *
     * @param info The information property to get.
     *
     * @return The value of the requested information property or an empty string.
     */
    QString getInformationAsString (const TabletInfo& info);

    /**
     * @see DBusTabletService::getProfile()
     */
    QDBusMessage getProfile();

    /**
     * @see DBusTabletService::getProperty(const DeviceType&, const Property&)
     */
    QDBusMessage getProperty (const DeviceType& device, const Property& property);

    /**
     * @see DBusTabletService::hasPadButtons()
     */
    QDBusMessage hasPadButtons();

    /**
     * @see DBusTabletService::isAvailable()
     */
    QDBusMessage isAvailable();

    /**
     * @see DBusTabletService::listProfiles()
     */
    QDBusMessage listProfiles();

    /**
     * @see DBusTabletService::setProfile(const QString&)
     */
    QDBusMessage setProfile (const QString& profile);

    /**
     * @see DBusTabletService::setProperty(const DeviceType&, const Property&, const QString&)
     */
    QDBusMessage setProperty (const DeviceType& device, const Property& property, const QString& value);


    QDBusMessage getProfileRotationList();

    QDBusMessage setProfileRotationList(const QStringList &rotationList);
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
