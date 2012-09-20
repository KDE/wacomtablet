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

#ifndef TABLETBACKENDMOCK_H
#define TABLETBACKENDMOCK_H

#include "tabletbackendinterface.h"

#include "deviceproperty.h"
#include "propertyadaptormock.h"

#include <QtCore/QMap>
#include <QtCore/QString>

namespace Wacom
{
class TabletBackendMock 
{
public:

    TabletBackendMock();

    virtual ~TabletBackendMock();

    void addAdaptor(const DeviceType& deviceType, PropertyAdaptor* adaptor);

    const TabletInformation& getInformation() const;

    const QString getProperty(const DeviceType& type, const Property& property) const;

    void setProfile(const TabletProfile& profile);

    void setProfile(const DeviceType& deviceType, const DeviceProfile& profile);

    bool setProperty(const DeviceType& type, const Property& property, const QString& value);


    const DeviceType* m_propertyAdaptorType; //!< The device type of the property adaptor.
    PropertyAdaptor*  m_propertyAdaptor;     //!< The property adaptor which was set by addAdaptor()

    TabletInformation m_tabletInformation;   //!< The information returned by getInformation()

    TabletProfile     m_tabletProfile;       //!< The last tablet profile which was set.
    DeviceProfile     m_deviceProfile;       //!< The last device profile which was set.
    const DeviceType* m_deviceProfileType;   //!< The last device profile type which was set.

    QMap<QString, PropertyAdaptorMock<DeviceProperty> > m_properties; //!< Properties which were set.

}; // CLASS
}  // NAMESPACE
#endif // HEADER PROTECTION
