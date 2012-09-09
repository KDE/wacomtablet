/*
 * Copyright 2009,2010 Jörg Ehrichs <joerg.ehichs@gmx.de>
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

#ifndef DEVICEINTERFACE_H
#define DEVICEINTERFACE_H

#include "property.h"
#include "devicetype.h"
#include "tabletprofile.h"

#include <QtCore/QString>

namespace Wacom
{

/**
  * This is the interface for all device backends that can be used to change the tablet settings
  * Implement the virtual methods that set / get the tablet parameters to make it work
  */
class DeviceInterface
{
public:
    /**
      * default destructor
      */
    virtual ~DeviceInterface() {}

    /**
      * Activates a profile and sets all tablet parameters for the given device.
      *
      * @param xdevice  The Xinput device name of the tablet device (stylus/eraser/pad/...).
      * @param devtype  The device type to apply the profile to.
      * @param tprofile The tablet profile to apply.
      */
    virtual void applyProfile(const QString & xdevice, const DeviceType& devtype, const TabletProfile& tprofile) = 0;


    /**
      * Gets the current value of a device property.
      *
      * @param xdevice  The Xinput device name of the tablet device (stylus/eraser/pad/...).
      * @param property The property to get.
      *
      * @return The property value as string.
      */
    virtual QString getProperty(const QString& xdevice, const Property& property) const = 0;


    /**
      * Sets a device property.
      *
      * @param xdevice  The Xinput device name of the tablet device (stylus/eraser/pad/...).
      * @param property The property to set.
      * @param value    The new value of the property.
      */
    virtual void setProperty(const QString& xdevice, const Property& property, const QString& value, bool activateButtonMapping = false) = 0;


    /**
      * Toggles the touch tool on/off.
      *
      * @param xdevice The Xinput device name of the touch device.
      */
    virtual void toggleTouch (const QString& xdevice)= 0;


    /**
      * Toggles the stylus/eraser/touch to absolute/relative mode.
      *
      * @param xdevice The Xinput device name of the stylus/eraser/touch device.
      */
    virtual void toggleMode (const QString& xdevice) = 0;


    /**
     * Sets the button mapping to apply when setting pad buttons.
     */
    void setButtonMapping(QMap<QString, QString> mapping) { m_buttonMapping = mapping; }

    
protected:
    QMap<QString, QString> m_buttonMapping;

}; // CLASS
}  // NAMESPACE
#endif // HEADER PROTECTION
