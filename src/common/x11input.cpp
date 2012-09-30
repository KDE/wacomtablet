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

#include "debug.h"
#include "x11input.h"
#include "x11inputdevice.h"

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/XInput.h>
//#include <X11/extensions/XInput2.h>
#include <X11/Xutil.h>

using namespace Wacom;

void X11Input::scanDevices(X11InputVisitor& visitor)
{
    int         ndevices = 0;
    Display     *dpy     = QX11Info::display();
    XDeviceInfo *info    = XListInputDevices( dpy, &ndevices );

    for( int i = 0; i < ndevices; ++i ) {

        X11InputDevice device(dpy, info[i].id);

        if (!visitor.visit(device)) {
            break;
        }
    }

    if (info) {
        XFreeDeviceList( info );
    }
}
