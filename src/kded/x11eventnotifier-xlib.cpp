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

#include "debug.h" // always needs to be first include

#include <QCoreApplication>
#include <QX11Info>


#include "x11eventnotifier.h"

#include "x11input.h"
#include "x11inputdevice.h"

#include <xcb/xcb.h>
#include <xcb/randr.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/XInput.h>
#include <X11/extensions/XInput2.h>

namespace Wacom
{
    class X11EventNotifierPrivate
    {
        public:
            bool     isStarted;
    };
}

using namespace Wacom;

X11EventNotifier::X11EventNotifier()
    : EventNotifier(NULL), QAbstractNativeEventFilter(), d_ptr(new X11EventNotifierPrivate)
{
    Q_D( X11EventNotifier );
    d->isStarted       = false;
}

X11EventNotifier::X11EventNotifier(const X11EventNotifier& notifier)
    : EventNotifier(NULL), QAbstractNativeEventFilter(), d_ptr(new X11EventNotifierPrivate)
{
    Q_UNUSED(notifier);
    Q_D( X11EventNotifier );
    d->isStarted       = false;
}

X11EventNotifier::~X11EventNotifier()
{
    delete d_ptr;
}


X11EventNotifier& X11EventNotifier::operator=(const X11EventNotifier& notifier)
{
    // nothing to do - this class is a singleton and must not be copied
    Q_UNUSED(notifier);
    return *this;
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
    Q_UNUSED(eventType);
    Q_UNUSED(result);

    xcb_generic_event_t *event = static_cast<xcb_generic_event_t *>(message);
    xcb_ge_generic_event_t *cookie = reinterpret_cast<xcb_ge_generic_event_t *>(message);

    if (event->response_type == XCB_GE_GENERIC && cookie->event_type == XI_HierarchyChanged) {
        // handleX11InputEvent(cookie);
    } else {
        // handleX11ScreenEvent(event);
    }

    // return QWidget::x11Event(event);
    return false;
}



// Handle plug/unplug events
void X11EventNotifier::handleX11InputEvent(xcb_ge_generic_event_t* event)
{
    Q_UNUSED(event)
    // Sadly there does not seem to be a way to use this functionality :(
}



int X11EventNotifier::registerForNewDeviceEvent(xcb_connection_t* conn)
{
    // This is already done by xcb plugin with more flags, doing this ourselves
    // will break Qt's xrandr functionality because connection is shared with Qt.
    // TODO: uncomment this again when we use our private connection.
#if 0
    //register RandR events
    int rrmask = XCB_RANDR_NOTIFY_MASK_SCREEN_CHANGE;

    xcb_randr_select_input(conn, QX11Info::appRootWindow(), 0);
    xcb_randr_select_input(conn, QX11Info::appRootWindow(), rrmask);
#endif

    return 0;
}
