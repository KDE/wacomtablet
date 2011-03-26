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
    virtual void applyProfile(const QString & device, const QString & section, KConfigGroup *gtprofile) const = 0;

    /**
      * writes a single configuration for the tablet
      *
      * @param device name of the tablet device we set (pad/stylus/eraser/cursor)
      * @param param parameter name
      * @param value value the parameter is set to
      */
    virtual void setConfiguration(const QString & device, const QString & param, const QString & value) const = 0;

    /**
      * returns the current value for a specific tablet setting
      *
      * @param device name of the tablet device we set (pad/stylus/eraser/cursor)
      * @param param the parameter we are looking for
      *
      * @return the value as string
      */
    virtual QString getConfiguration(const QString & device, const QString & param) const = 0;

    /**
      * returns the factory default value for a specific tablet setting
      *
      * @param device name of the tablet device we set (pad/stylus/eraser/cursor)
      * @param param the parameter we are looking for
      *
      * @return the factory default value as string
      */
    virtual QString getDefaultConfiguration(const QString & device, const QString & param) const = 0;
};

}

#endif // DEVICEINTERFACE_H
