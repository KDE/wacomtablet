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

    const TabletInformation& getInformation() const;

    const QString getProperty(const DeviceType& type, const Property& property) const;

    void setProfile(const TabletProfile& profile);

    void setProfile(const Wacom::DeviceType& deviceType, const Wacom::DeviceProfile& profile);

    void setProperty(const Wacom::DeviceType& type, const Property& property, const QString& value);


private:
    typedef QList<PropertyAdaptor*>       AdaptorList;
    typedef QMap<DeviceType, AdaptorList> DeviceMap;

    Q_DECLARE_PRIVATE(TabletBackend);
    TabletBackendPrivate *const d_ptr; //!< D-Pointer which gives access to private members.

}; // CLASS
}  // NAMESPACE
#endif // HEADER PROTECTION
