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

#include "debug.h"
#include "xeventnotifier.h"

#include "x11utils.h"

#include <KDE/KApplication>
#include <KDE/KDebug>

#include <QtGui/QX11Info>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/XInput.h>
#include <X11/extensions/XInput2.h>
#include <X11/extensions/Xrandr.h>

namespace Wacom
{
    class XEventNotifierPrivate
    {
        public:
            Rotation currentRotation;
    };
}

using namespace Wacom;

XEventNotifier::XEventNotifier(QWidget* parent)
    : EventNotifier(parent), d_ptr(new XEventNotifierPrivate)
{
    Q_D( XEventNotifier );
    d->currentRotation = 0;
}

XEventNotifier::~XEventNotifier()
{
    delete d_ptr;
}



void XEventNotifier::scan()
{
    TabletInformation tabletInformation;

    if (!X11Utils::findTabletDevice(tabletInformation)) {
        kDebug() << "No input devices found via xinput!";
        return;
    }

    emit tabletAdded(tabletInformation);
}



void XEventNotifier::start()
{
    if( KApplication::kApplication() != NULL ) {
        registerForNewDeviceEvent(QX11Info::display());
        KApplication::kApplication()->installX11EventFilter(this);
    }
}



void XEventNotifier::stop()
{
    if( KApplication::kApplication() != NULL ) {
        KApplication::kApplication()->removeX11EventFilter(this);
    }
}



bool XEventNotifier::x11Event(XEvent* event)
{
    XGenericEventCookie *cookie = &event->xcookie;

    if (cookie->type == GenericEvent && cookie->evtype == XI_HierarchyChanged) {
        handleX11InputEvent(event);

    } else {
        handleX11ScreenEvent(event);
    }

    return QWidget::x11Event(event);
}



void XEventNotifier::handleX11InputEvent(XEvent* event)
{
    XGenericEventCookie *cookie       = &event->xcookie;
    bool                 ownEventData = XGetEventData(QX11Info::display(), cookie);
    TabletInformation    tabletInfo;

    if(cookie->data)
    {
        XIHierarchyEvent * hev = (XIHierarchyEvent *)cookie->data;
        XIHierarchyInfo *info = (XIHierarchyInfo *)hev->info;

        for (int i = 0; i < hev->num_info; i++)
        {
            if (info[i].flags & XISlaveRemoved) {
                kDebug() << "Device removed with id: " << info[i].deviceid;
                tabletInfo.xdeviceId = info[i].deviceid;
                emit tabletRemoved(tabletInfo);

            } else if (info[i].flags & XISlaveAdded && X11Utils::isTabletDevice(info[i].deviceid)) {
                kDebug() << "Wacom Tablet Device added with id: " << info[i].deviceid;
                tabletInfo.xdeviceId = info[i].deviceid;

                if (!X11Utils::findTabletDevice(tabletInfo)) {
                    kError() << "No input devices found via xinput altough an X event was fired!";
                    return;
                }

                emit tabletAdded(tabletInfo);
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



void XEventNotifier::handleX11ScreenEvent(XEvent* event)
{
    Q_D( XEventNotifier );
    
    int m_eventBase;
    int m_errorBase;

    XRRQueryExtension(QX11Info::display(), &m_eventBase, &m_errorBase);

    if (event->type == m_eventBase + RRScreenChangeNotify) {
        XRRUpdateConfiguration(event);
        Rotation old_r = d->currentRotation;

        XRRRotations(QX11Info::display(), DefaultScreen(QX11Info::display()), &(d->currentRotation));

        if (old_r != d->currentRotation) {
            switch (d->currentRotation) {
                    case RR_Rotate_0:
                        emit screenRotated(TabletRotation::NONE);
                        break;
                    case RR_Rotate_90:
                        emit screenRotated(TabletRotation::CCW);
                        break;
                    case RR_Rotate_180:
                        emit screenRotated(TabletRotation::HALF);
                        break;
                    case RR_Rotate_270:
                        emit screenRotated(TabletRotation::CW);
                        break;
            }
        }
    }
}



int XEventNotifier::registerForNewDeviceEvent(Display* display)
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
