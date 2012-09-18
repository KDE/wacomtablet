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

#ifndef TABLETBACKEND_H
#define TABLETBACKEND_H

#include "property.h"
#include "propertyadaptor.h"
#include "devicetype.h"
#include "tabletprofile.h"
#include "tabletinformation.h"

#include <QtCore/QString>
#include <QtCore/QMap>
#include <QtCore/QList>

namespace Wacom
{

// forward declaration
class TabletBackendPrivate;

class TabletBackend
{
public:

    TabletBackend(const TabletInformation& tabletInformation);
    virtual ~TabletBackend();

    /**
     * Adds a property adaptor for the given device type. The property adaptor
     * will be deleted once this class is destroyed.
     *
     * @param deviceType The device type to add the property adaptor for.
     * @param adaptor    The property adaptor to add.
     */
    void addAdaptor(const DeviceType& deviceType, PropertyAdaptor* adaptor);

    /**
     * Returns tablet information about the tablet handled by this backend.
     *
     * @return TabletInformation
     */
    const TabletInformation& getInformation() const;

    /**
     * Gets a tablet property. If the property is not supported by any of the
     * adaptors which were added to this tablet, an empty string is returned.
     *
     * @param type     The device to read the property from.
     * @param property The property to get.
     *
     * @return The property value or an empty string.
     */
    const QString getProperty(const DeviceType& type, const Property& property) const;

    /**
     * Applies a profile to the tablet managed by this backend.
     *
     * @param profile The profile to apply.
     */
    void setProfile(const TabletProfile& profile);

    /**
     * Applies a profile to a device.
     *
     * @param deviceType The device to apply the profile to.
     * @param profile    The profile to apply.
     */
    void setProfile(const Wacom::DeviceType& deviceType, const Wacom::DeviceProfile& profile);

    /**
     * Sets a property on a device.
     *
     * @param type     The device to set the property on.
     * @param property The property to set on the device.
     * @param value    The property value to set.
     */
    bool setProperty(const Wacom::DeviceType& type, const Property& property, const QString& value);


private:
    typedef QList<PropertyAdaptor*>       AdaptorList;
    typedef QMap<DeviceType, AdaptorList> DeviceMap;

    Q_DECLARE_PRIVATE(TabletBackend);
    TabletBackendPrivate *const d_ptr; //!< D-Pointer which gives access to private members.

}; // CLASS
}  // NAMESPACE
#endif // HEADER PROTECTION
