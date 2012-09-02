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
#include "tabletprofile.h"

#include <KDE/KConfigGroup>
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
      * activates a profile and sets all tablet parameters
      *
      * @param device name of the tablet device we set (pad/stylus/eraser/cursor)
      * @param section the sued section we apply (pad/stylus/eraser)
      * @param gtprofile the KConfig profile for the tablet that should be applied
      */
    virtual void applyProfile(const QString & device, const QString & section, const TabletProfile& gtprofile) = 0;

    /**
      * writes a single configuration for the tablet
      *
      * @param device   The name of the tablet device we set (pad/stylus/eraser/cursor)
      * @param property The property to set.
      * @param value    The new value of the property.
      */
    virtual void setConfiguration(const QString & device, const Property & property, const QString & value, bool activateButtonMapping = false) = 0;

    /**
      * returns the current value for a specific tablet setting
      *
      * @param device   name of the tablet device we set (pad/stylus/eraser/cursor)
      * @param property The property we are looking for
      *
      * @return the value as string
      */
    virtual QString getConfiguration(const QString & device, const Property & property) const = 0;

    /**
      * returns the factory default value for a specific tablet setting
      *
      * @param device name of the tablet device we set (pad/stylus/eraser/cursor)
      * @param property the property we are looking for
      *
      * @return the factory default value as string
      */
    virtual QString getDefaultConfiguration(const QString & device, const Property & property) const = 0;

    /**
      * Toggles the touch tool on/off
      */
    virtual void toggleTouch(const QString & touchDevice)= 0;

    /**
      * Toggles the stylus/eraser to absolute/relative mode
      */
    virtual void togglePenMode(const QString & device) = 0;

    void setButtonMapping(QMap<QString, QString> mapping) { m_buttonMapping = mapping; }

protected:
    QMap<QString, QString> m_buttonMapping; /**< @see DeviceHandlerPrivate::buttonMapping */
};

}

#endif // DEVICEINTERFACE_H
