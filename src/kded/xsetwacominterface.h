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
class XsetwacomInterface : public DeviceInterface {
public:
    /**
      * Default constructor
      */
    explicit XsetwacomInterface();

    /**
      * Default destructor
      */
    ~XsetwacomInterface();


    /**
     * @see DeviceInterface::applyProfile(const QString&, const DeviceType&, const TabletProfile&)
     */
    void applyProfile (const QString &xdevice, const DeviceType& devtype, const TabletProfile& tabletProfile);


    /**
     * @see DeviceInterface::setProperty(const QString&, const Property&, const QString&, bool)
     */
    void setProperty (const QString& xdevice, const Property& property, const QString& value, bool activateButtonMapping = false);


    /**
     * @see DeviceInterface::getProperty(const QString&, const Property&) const
     */
    QString getProperty (const QString& xdevice, const Property& property) const;


    /**
     * @see DeviceHandler::toggleMode(const QString&)
     */
    void toggleMode (const QString& xdevice);


    /**
     * @see DeviceHandler::toggleTouch(const QString&)
     */
    void toggleTouch (const QString& xdevice);

}; // CLASS
}  // NAMESPACE
#endif // WACOMINTERFACE_H
