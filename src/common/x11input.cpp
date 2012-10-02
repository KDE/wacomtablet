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

#include "debug.h"
#include "x11input.h"
#include "x11inputdevice.h"
#include "x11tabletfinder.h"

#include <QtCore/QString>
#include <QtCore/QList>

#include <QtGui/QX11Info>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/XInput.h>
//#include <X11/extensions/XInput2.h>
#include <X11/Xutil.h>

using namespace Wacom;


bool X11Input::findDevice(const QString& deviceName, X11InputDevice& device)
{
    if (deviceName.isEmpty()) {
        return false;
    }

    bool     found    = false;
    int      ndevices = 0;
    Display *display  = QX11Info::display();

    X11InputDevice::XDeviceInfo *info = XListInputDevices (display, &ndevices);

    for( int i = 0; i < ndevices; ++i ) {
        if (deviceName.compare (QLatin1String (info[i].name), Qt::CaseInsensitive) == 0) {
            found = device.open (display, info[i]);
            break;
        }
    }

    if (info) {
        XFreeDeviceList (info);
    }

    return found;
}



TabletInformation X11Input::findTablet()
{
    X11TabletFinder tabletFinder;

    scanDevices (tabletFinder);

    return tabletFinder.getInformation();
}



void X11Input::scanDevices(X11InputVisitor& visitor)
{
    int      ndevices = 0;
    Display *dpy      = QX11Info::display();

    X11InputDevice::XDeviceInfo *info = XListInputDevices (dpy, &ndevices);

    for( int i = 0; i < ndevices; ++i ) {

        X11InputDevice device (dpy, info[i]);

        if (!visitor.visit (device)) {
            break;
        }
    }

    if (info) {
        XFreeDeviceList (info);
    }
}



bool X11Input::setCoordinateTransformationMatrix(const QString& deviceName, qreal offsetX, qreal offsetY, qreal width, qreal height)
{
    X11InputDevice device;

    if (!findDevice(deviceName, device)) {
        return false;
    }

    /*
     *  | width  0       offsetX |
     *  | 0      height  offsetY |
     *  | 0      0          1    |
     */
    QList<float> matrix;
    matrix.append(width);
    matrix.append(0);
    matrix.append(offsetX);
    matrix.append(0);
    matrix.append(height);
    matrix.append(offsetY);
    matrix.append(0);
    matrix.append(0);
    matrix.append(1);

    return device.setFloatProperty(QLatin1String("Coordinate Transformation Matrix"), matrix);
}

