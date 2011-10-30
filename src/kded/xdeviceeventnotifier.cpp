/*
 * Copyright 2010, 2011 Jörg Ehrichs <joerg.ehichs@gmx.de>
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

#include "xdeviceeventnotifier.h"

// KDE includes
#include <KDE/KApplication>
#include <KDE/KDebug>

// Ot includes
#include <QtGui/QX11Info>

// X11 includes
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/XInput.h>
#include <X11/extensions/XInput2.h>
#include <X11/extensions/Xrandr.h>

using namespace Wacom;

Rotation r = 0;

XDeviceEventNotifier::XDeviceEventNotifier(QWidget *parent) :
        QWidget(parent)
{
}

void XDeviceEventNotifier::start()
{
    if( KApplication::kApplication() != NULL ) {
        registerForNewDeviceEvent(QX11Info::display());

        KApplication::kApplication()->installX11EventFilter(this);
    }
}

void XDeviceEventNotifier::stop()
{
    if( KApplication::kApplication() != NULL ) {
        KApplication::kApplication()->removeX11EventFilter(this);
    }
}

bool XDeviceEventNotifier::x11Event(XEvent * event)
{
    XGenericEventCookie *cookie = &event->xcookie;

    if (cookie->type == GenericEvent && cookie->evtype == XI_HierarchyChanged)
    {
        bool ownEventData = false;
        ownEventData = XGetEventData(QX11Info::display(), cookie);

        if(cookie->data)
        {
            XIHierarchyEvent * hev = (XIHierarchyEvent *)cookie->data;
            XIHierarchyInfo *info = (XIHierarchyInfo *)hev->info;

            for (int i = 0; i < hev->num_info; i++)
            {
                if (info[i].flags & XISlaveRemoved) {
                    kDebug() << "Device removed with id: " << info[i].deviceid;
                    emit deviceRemoved(info[i].deviceid);
                }

                if (info[i].flags & XISlaveAdded) {
                    if(isTabletDevice(info[i].deviceid)) {
                        kDebug() << "Wacom Tablet Device added with id: " << info[i].deviceid;
                        emit deviceAdded(info[i].deviceid);
                    }
                }
            }
            
            // only free event data if I own the resource if a different resource called XGetEventData the data will not be set free again here
            if(ownEventData) {
	        XFreeEventData(QX11Info::display(), cookie);
	    }
        }
        else {
            kDebug() << "Error couldn't retrieve XGetEventData";
        }
    }
    else {
      
      int m_eventBase;
      int m_errorBase;
      
      XRRQueryExtension(QX11Info::display(), &m_eventBase, &m_errorBase);
      
      if (event->type == m_eventBase + RRScreenChangeNotify) {
            XRRUpdateConfiguration(event);
            Rotation old_r = r;

            XRRRotations(QX11Info::display(), DefaultScreen(QX11Info::display()), &r);

            if (old_r != r) {
                switch (r) {
                        case RR_Rotate_0:
                                emit screenRotated(NONE);
                                break;
                        case RR_Rotate_90:
                               emit screenRotated(CCW);
                               break;
                        case RR_Rotate_180:
                               emit screenRotated(HALF);
                               break;
                        case RR_Rotate_270:
                               emit screenRotated(CW);
                               break;
                }
            }
        }
    }

    return QWidget::x11Event(event);
}

int XDeviceEventNotifier::registerForNewDeviceEvent(Display* display)
{

    XIEventMask evmask;
    unsigned char mask[2] = { 0, 0 };

    XISetMask(mask, XI_HierarchyChanged);
    evmask.deviceid = XIAllDevices;
    evmask.mask_len = sizeof(mask);
    evmask.mask = mask;

    XISelectEvents(display, DefaultRootWindow(display), &evmask, 1);
    
    //register RandR events
    int rrmask = RRScreenChangeNotifyMask;

    XRRSelectInput(display, DefaultRootWindow(display), 0);
    XRRSelectInput(display, DefaultRootWindow(display), rrmask); 

    return 0;
}

bool XDeviceEventNotifier::isTabletDevice(int deviceid)
{
    bool isWacomtablet = false;
    Atom wacom_prop = XInternAtom(QX11Info::display(), "Wacom Tool Type", True);

    XDevice *dev = XOpenDevice(QX11Info::display(), deviceid);

    if (dev)
    {
        int natoms;
        Atom *atoms = XListDeviceProperties(QX11Info::display(), dev, &natoms);
        if (natoms) {
            int j;
            for (j = 0; j < natoms; j++) {
                if (atoms[j] == wacom_prop) {
                    isWacomtablet = true;
                    break;
                }
            }
        }

        XFree(atoms);
        XCloseDevice(QX11Info::display(), dev);
    }

    return isWacomtablet;
}
