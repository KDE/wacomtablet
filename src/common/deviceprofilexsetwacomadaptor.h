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

#ifndef DEVICEPROFILEXSETWACOMADAPTOR_H
#define DEVICEPROFILEXSETWACOMADAPTOR_H

#include "propertyadaptor.h"
#include "deviceprofile.h"
#include "xsetwacomproperty.h"

namespace Wacom {

class DeviceProfileXsetwacomAdaptorPrivate;

/**
 */
class DeviceProfileXsetwacomAdaptor : public PropertyAdaptor{
public:
    /**
      * Default constructor
      */
    DeviceProfileXsetwacomAdaptor(DeviceProfile& profile);

    /**
      * Default destructor
      */
    virtual ~DeviceProfileXsetwacomAdaptor();


    /**
     * Returns a list of xsetwacom properties which are available
     * from the current profile.
     * 
     * @return A list of xsetwacom properties which are currently available.
     * 
     * @see Wacom::PropertyAdaptor::getProperties() const
     */
    virtual const QList<Property> getProperties() const;

    const QList<XsetwacomProperty> getXsetwacomProperties() const;
    
    /**
     * Gets a xsetwacom property value from the managed profile. If the
     * property is not available, an empty string will be returned.
     * 
     * @return The current value of the xsetwacom property.
     * 
     * @see Wacom::PropertyAdaptor::getProperty(const QString&) const
     */
    virtual const QString getProperty(const Property& property) const;

    const QString getXsetwacomProperty(const XsetwacomProperty& property) const;
    
    /**
     * Sets a xsetwacom property on the managed profile. If the
     * property can not be set, it will be ignored.
     * 
     * @see Wacom::PropertyAdaptor::setProperty(const QString&, const QString&)
     */
    virtual bool setProperty(const Property& property, const QString& value);

    bool setXsetwacomProperty(const XsetwacomProperty& property, const QString& value);
    

private:
    Q_DECLARE_PRIVATE( DeviceProfileXsetwacomAdaptor )

    DeviceProfileXsetwacomAdaptorPrivate *const d_ptr; /**< d-pointer for this class */


};     // CLASS
}      // NAMESPACE
#endif // HEADER PROTECTION
