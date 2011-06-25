/*
 * Copyright 2010 Jörg Ehrichs <joerg.ehichs@gmx.de>
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

#ifndef XDEVICEEVENTNOTIFIER_H
#define XDEVICEEVENTNOTIFIER_H

#include <QtGui/QWidget>

namespace Wacom
{

/**
  * Simple enumeration to save the tablet rotation
  */
enum TabletRotation {
    NONE,       /**< no rotation */
    CCW,        /**< rotate counter clockwise -90° */
    CW,         /**< rotate clockwise +90° */
    HALF        /**< rotate half 180° */
};

/**
  * This eventhandler is used to catch X11 events for conected and removed devices
  *
  * It will detect if a new device is a wacom tablet and sends a signal with the X11 device id.
  */
class XDeviceEventNotifier : public QWidget
{
    Q_OBJECT
public:
    /**
     * ctor
     */
    explicit XDeviceEventNotifier(QWidget *parent = 0);

    /**
      * Connects the event into the kapplication event system
      */
    void start();

    /**
      * Removes the event from the Kapplication event system
      */
    void stop();

protected:
    /**
      * Called when a new X11 event is detected.
      * Checks for new connected or removed tablet devices
      */
    bool x11Event(XEvent * e);

signals:
    /**
      * This signal is send when a new wacom tablet is found
      *
      * The @p deviceid is the X11 deviceid for the tablet device.
      * This might be called several times for one tablet, as each tablet has several devices (pad, stylus, cursor)
      * @param deviceid X11 id of the tablet device
      */
    void deviceAdded(int deviceid);

    /**
      * Will be called everytime a device is removed from the system
      *
      * It is the responsibility of the tabletdemon to check if the deviceid is the connected wacom tablet that it manages.
      * @param deviceid X11 id of the device
      */
    void deviceRemoved(int deviceid);
    
    /**
     * When the screen is rotated by RandR this signal is emitted
     * Connect to this signal to rotate the tablet together with it
     *
     * @param tabletRotation @arg 0 none
     *                       @arg 1 ccw
     *                       @arg 2 cw
     *                       @arg 3 half
     * */
    void screenRotated(TabletRotation tabletRotation);

private:
        /**
          * Register the eventhandler with the X11 system
          */
        int registerForNewDeviceEvent(Display* display);

        /**
          * Checks if the device with the @p deviceid is a wacom tablet.
          *
          * @param deviceid x11 id of the device that will be checked
          * @return bool true if the device is a wacom tablet, false otherwise
          */
        bool isTabletDevice(int deviceid);
};
};
#endif // XDEVICEEVENTNOTIFIER_H
