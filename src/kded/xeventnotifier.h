/*
 * This file is part of the KDE wacomtablet project. For copyright
 * information and license terms see the AUTHORS and COPYING files
 * in the top-level directory of this distribution.
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

#ifndef XEVENTNOTIFIER_H
#define XEVENTNOTIFIER_H

#include "eventnotifier.h"

namespace Wacom
{

class XEventNotifierPrivate;

class XEventNotifier : public EventNotifier
{
    Q_OBJECT

public:

    explicit XEventNotifier(QWidget* parent = 0);
    virtual ~XEventNotifier();

    /**
     * @see EventNotifier::scan()
     */
    void scan();

    /**
     * @see EventNotifier::start()
     */
    void start();

    /**
     * @see EventNotifier::stop()
     */
    void stop();


protected:

    /**
      * Called by Qt when a new X11 event is detected.
      */
    bool x11Event(XEvent* event);


private:

    /**
     * Handles X11 input events which signal adding or removal of a device.
     * This method should not be called directly, but only by our X11 event
     * handler method.
     */
    void handleX11InputEvent(XEvent* event);

    /**
     * Handles X11 screen events which signal rotation of the screen.
     * This method should not be called directly, but only by our X11 event
     * handler method.
     */
    void handleX11ScreenEvent(XEvent* event);

    /**
      * Register the eventhandler with the X11 system
      */
    int registerForNewDeviceEvent(Display* display);


    Q_DECLARE_PRIVATE( XEventNotifier )
    XEventNotifierPrivate *const d_ptr;

}; // CLASS
}  // NAMESPACE
#endif // HEADER PROTECTION
