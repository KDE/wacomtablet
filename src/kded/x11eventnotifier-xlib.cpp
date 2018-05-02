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

#include <QCoreApplication>
#include <QX11Info>

#include "logging.h"
#include "x11eventnotifier.h"

#include "x11input.h"
#include "x11inputdevice.h"

#include <xcb/xcb.h>
#include <X11/extensions/XInput2.h>

/* XCB events have a slightly different layout, so they can't be directly
 * treated as XIGenericDeviceEvent. Instead of doing a memmove like
 * Qt does in qtbase/src/plugins/platforms/xcb/qxcbconnection_xi2.cpp,
 * we pull in the correct struct definition here. */

typedef enum xcb_input_hierarchy_mask_t {
    XCB_INPUT_HIERARCHY_MASK_MASTER_ADDED = 1,
    XCB_INPUT_HIERARCHY_MASK_MASTER_REMOVED = 2,
    XCB_INPUT_HIERARCHY_MASK_SLAVE_ADDED = 4,
    XCB_INPUT_HIERARCHY_MASK_SLAVE_REMOVED = 8,
    XCB_INPUT_HIERARCHY_MASK_SLAVE_ATTACHED = 16,
    XCB_INPUT_HIERARCHY_MASK_SLAVE_DETACHED = 32,
    XCB_INPUT_HIERARCHY_MASK_DEVICE_ENABLED = 64,
    XCB_INPUT_HIERARCHY_MASK_DEVICE_DISABLED = 128
} xcb_input_hierarchy_mask_t;

typedef struct xcb_input_hierarchy_event_t {
    uint8_t               response_type;
    uint8_t               extension;
    uint16_t              sequence;
    uint32_t              length;
    uint16_t              event_type;
    uint8_t               deviceid;
    xcb_timestamp_t       time;
    uint32_t              flags;
    uint16_t              num_infos;
    uint8_t               pad0[10];
    uint32_t              full_sequence;
} xcb_input_hierarchy_event_t;

typedef struct xcb_input_hierarchy_info_t {
    uint8_t               deviceid;
    uint8_t               attachment;
    uint8_t               type;
    uint8_t               enabled;
    uint8_t               pad0[2];
    uint32_t              flags;
} xcb_input_hierarchy_info_t;

namespace Wacom
{
    class X11EventNotifierPrivate
    {
        public:
            bool     isStarted = false;
    };
}

using namespace Wacom;

X11EventNotifier::X11EventNotifier()
    : EventNotifier(nullptr)
    , QAbstractNativeEventFilter()
    , d_ptr(new X11EventNotifierPrivate)
{

}

X11EventNotifier::~X11EventNotifier()
{
    delete d_ptr;
}

X11EventNotifier& X11EventNotifier::instance()
{
    static X11EventNotifier instance;
    return instance;
}




void X11EventNotifier::start()
{
    Q_D (X11EventNotifier);

    if (d->isStarted) {
        return;
    }

    if( QCoreApplication::instance() != nullptr ) {
        registerForNewDeviceEvent(QX11Info::connection());
        QCoreApplication::instance()->installNativeEventFilter(this);
        d->isStarted = true;
    }
}



void X11EventNotifier::stop()
{
    Q_D (X11EventNotifier);

    if( QCoreApplication::instance() != nullptr ) {
        QCoreApplication::instance()->removeNativeEventFilter(this);
        d->isStarted = false;
    }
}



bool X11EventNotifier::nativeEventFilter(const QByteArray &eventType, void *message, long int *result)
{
    Q_UNUSED(result);

    if (eventType != "xcb_generic_event_t") {
        return false;
    }

    xcb_generic_event_t *event = static_cast<xcb_generic_event_t *>(message);
    xcb_ge_generic_event_t *cookie = reinterpret_cast<xcb_ge_generic_event_t *>(message);

    if (event->response_type == XCB_GE_GENERIC && cookie->event_type == XI_HierarchyChanged) {
        handleX11InputEvent(cookie);
    }

    return false;
}



// Handle plug/unplug events
void X11EventNotifier::handleX11InputEvent(xcb_ge_generic_event_t* event)
{
    xcb_input_hierarchy_event_t *hev = reinterpret_cast<xcb_input_hierarchy_event_t *>(event);
    xcb_input_hierarchy_info_t *data = reinterpret_cast<xcb_input_hierarchy_info_t*>(hev + 1);

    while(hev->num_infos--) {
        if (data->flags & XCB_INPUT_HIERARCHY_MASK_SLAVE_REMOVED) {
            qCDebug(KDED) << QString::fromLatin1("X11 device with id '%1' removed.").arg(data->deviceid);
            emit tabletRemoved(data->deviceid);

        } else if (data->flags & XCB_INPUT_HIERARCHY_MASK_SLAVE_ADDED) {
            qCDebug(KDED) << QString::fromLatin1("X11 device with id '%1' added.").arg(data->deviceid);

            X11InputDevice device (data->deviceid, QLatin1String("Unknown X11 Device"));

            if (device.isOpen() && device.isTabletDevice()) {
                qCDebug(KDED) << QString::fromLatin1("Wacom tablet device with X11 id '%1' added.").arg(data->deviceid);
                emit tabletAdded(data->deviceid);
            }
        }
        data++;
    }
}



int X11EventNotifier::registerForNewDeviceEvent(xcb_connection_t *conn)
{
    Q_UNUSED(conn)

    Display *display = QX11Info::display();

    unsigned int bitmask = XI_HierarchyChangedMask;

    XIEventMask mask;
    mask.mask_len = sizeof(bitmask);
    mask.mask = reinterpret_cast<unsigned char *>(&bitmask);
    int status = XISelectEvents(display, DefaultRootWindow(display), &mask, 1);
    return status == Success ? 0 : 1;
}
