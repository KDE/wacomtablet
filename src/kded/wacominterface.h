/*
 * Copyright 2009, 2010 Jörg Ehrichs <joerg.ehichs@gmx.de>
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

#ifndef WACOMINTERFACE_H
#define WACOMINTERFACE_H

#include "deviceinterface.h"

namespace Wacom {

/**
  * This class implements the backend for the wacom-tools settings and thus the handling of xsetwacom.
  *
  * All profile key / value settings will be transformed into calls of the form @c xsetwacom set "Devicename" key value.
  * The usage of console calls via QProcess allows the backend to be independent of the implementation of the waom-tools xsetwacom functions.
  *
  * Therefore the change from the old wacom-tools package to the new X11 wacom driver is no problem as both use the same methods and same syntax
  * to set up the tablet device. No further dependencies are necessary here.
  *
  * @see Wacom::DeviceHandler
  * @see http://linuxwacom.sourceforge.net/index.php/howto/xsetwacom
  */
class WacomInterface : public DeviceInterface {
public:
    /**
      * Default constructor
      */
    explicit WacomInterface();

    /**
      * Default destructor
      */
    ~WacomInterface();

    /**
      * Activates a profile and sets all tablet parameters
      *
      * @param device name of the tablet device we set (pad/stylus/eraser/cursor)
      * @param section the used section we apply (pad/stylus/eraser/cursor)
      * @param gtprofile the KConfig profile for the tablet that should be applied
      */
    void applyProfile( const QString &device, const QString &section, const TabletProfile& gtprofile );

    /**
      * Writes a single configuration for the tablet
      *
      * @param device name of the tablet device we set (pad/stylus/eraser/cursor)
      * @param param parameter name
      * @param value value the parameter is set to
      */
    void setConfiguration( const QString &device, const Property &property, const QString &value, bool activateButtonMapping = false );

    /**
      * Returns the current value for a specific tablet setting
      *
      * @param device name of the tablet device we set (pad/stylus/eraser/cursor)
      * @param param the parameter we are looking for
      *
      * @return the value as string
      */
    QString getConfiguration( const QString &device, const Property &property ) const;

    /**
      * Returns the factory default value for a specific tablet setting
      *
      * @param device name of the tablet device we set (pad/stylus/eraser/cursor)
      * @param param the parameter we are looking for
      *
      * @return the factory default value as string
      */
    QString getDefaultConfiguration( const QString& device, const Property& property ) const;

    /**
      * Toggles the touch tool on/off
      */
    void toggleTouch(const QString & touchDevice);

    /**
      * Toggles the stylus/eraser to absolute/relative mode
      */
    void togglePenMode(const QString & device);

private:
    void mapTabletToScreen(const QString &device, const QString & screenArea);

    void setCursorSettings(const QString &deviceName, KConfigGroup *gtprofile);

};

}
#endif // WACOMINTERFACE_H
