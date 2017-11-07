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

#include "x11tabletfinder.h"
#include "deviceinformation.h"
#include "x11input.h"

#if defined(HAVE_XCB_XINPUT)
# include <xcb/xcb.h>
#else
# include <X11/Xlib.h>
# include <X11/Xatom.h>
# include <X11/extensions/XInput.h>
# include <X11/Xutil.h>
#endif

#include <QMap>

using namespace Wacom;

/**
 * Class for private members.
 */
namespace Wacom {
    class X11TabletFinderPrivate
    {
        public:
            typedef QMap<long,TabletInformation> TabletMap;

            TabletMap                tabletMap;   //!< A map which is used while visiting devices.
            QList<TabletInformation> scannedList; //!< A list which is build after scanning all devices.
    };
}


X11TabletFinder::X11TabletFinder() : d_ptr(new X11TabletFinderPrivate)
{
}


X11TabletFinder::~X11TabletFinder()
{
    delete d_ptr;
}



const QList< TabletInformation >& X11TabletFinder::getTablets() const
{
    Q_D (const X11TabletFinder);

    return d->scannedList;
}




bool X11TabletFinder::scanDevices()
{
    Q_D (X11TabletFinder);

    d->tabletMap.clear();
    d->scannedList.clear();

    X11Input::scanDevices(*this);

    X11TabletFinderPrivate::TabletMap::ConstIterator iter;

    for (iter = d->tabletMap.constBegin() ; iter != d->tabletMap.constEnd() ; ++iter) {
        d->scannedList.append(iter.value());
    }

    return (d->tabletMap.size() > 0);
}



bool X11TabletFinder::visit (X11InputDevice& x11device)
{
    if (!x11device.isTabletDevice()) {
        return false;
    }

    // gather basic device information which we need to create a device information structure
    QString           deviceName = x11device.getName();
    const DeviceType* deviceType = getDeviceType (getToolType (x11device));

    if (deviceName.isEmpty() || deviceType == NULL) {
        errWacom << QString::fromLatin1("Unsupported device '%1' detected!").arg(deviceName);
        return false;
    }

    // create device information and gather all information we can
    DeviceInformation deviceInfo (*deviceType, x11device.getName());

    gatherDeviceInformation(x11device, deviceInfo);

    // add device information to tablet map
    addDeviceInformation(deviceInfo);

    // true is only returned if device visiting should be aborted by X11Input
    return false;
}



void X11TabletFinder::addDeviceInformation (DeviceInformation& deviceInformation)
{
    Q_D(X11TabletFinder);

    long serial = deviceInformation.getTabletSerial();

    if (serial < 1) {
        qWarning() << QString::fromLatin1 ("Device '%1' has an invalid serial number '%2'!").arg (deviceInformation.getName()).arg (serial);
    }

    X11TabletFinderPrivate::TabletMap::iterator mapIter = d->tabletMap.find (serial);

    if (mapIter == d->tabletMap.end()) {
        mapIter = d->tabletMap.insert(serial, TabletInformation (serial));
    }

    mapIter.value().setDevice(deviceInformation);
}



void X11TabletFinder::gatherDeviceInformation(X11InputDevice& device, DeviceInformation& deviceInformation) const
{
    // get X11 device id
    deviceInformation.setDeviceId(device.getDeviceId());

    // get tablet serial
    deviceInformation.setTabletSerial(getTabletSerial(device));

    // get product and vendor id if set
    long vendorId = 0, productId = 0;

    if (getProductId(device, vendorId, productId)) {
        deviceInformation.setVendorId(vendorId);
        deviceInformation.setProductId(productId);
    }

    // get the device node which is the full path to the input device
    deviceInformation.setDeviceNode(getDeviceNode(device));
}



const QString X11TabletFinder::getDeviceNode(X11InputDevice& device) const
{
    QList<QString> values;

    if (!device.getStringProperty(X11Input::PROPERTY_DEVICE_NODE, values, 1000) || values.size() == 0) {
        qWarning() << QString::fromLatin1("Could not get device node from device '%1'!").arg(device.getName());
        return QString();
    }

    return values.at(0);
}



const DeviceType* X11TabletFinder::getDeviceType (const QString& toolType) const
{
    if (toolType.contains (QLatin1String ("pad"), Qt::CaseInsensitive)) {
        return &(DeviceType::Pad);

    } else if (toolType.contains(QLatin1String ("eraser"), Qt::CaseInsensitive)) {
        return &(DeviceType::Eraser);

    } else if (toolType.contains(QLatin1String ("cursor"), Qt::CaseInsensitive)) {
        return &(DeviceType::Cursor);

    } else if (toolType.contains(QLatin1String ("touch"),  Qt::CaseInsensitive)) {
        return &(DeviceType::Touch);

    } else if (toolType.contains(QLatin1String ("stylus"), Qt::CaseInsensitive)) {
        return &(DeviceType::Stylus);
    }

    return NULL;
}



bool X11TabletFinder::getProductId(X11InputDevice& device, long int& vendorId, long int& productId) const
{
    QList<long> values;

    if (!device.getLongProperty(X11Input::PROPERTY_DEVICE_PRODUCT_ID, values, 2)) {
        return false;
    }

    if (values.size() != 2) {
        errWacom << QString::fromLatin1("Unexpected number of values when fetching XInput property '%1'!").arg(X11Input::PROPERTY_DEVICE_PRODUCT_ID);
        return false;
    }

    long value;

    if ((value = values.at(0)) > 0) {
        vendorId = value;
    }

    if ((value = values.at(1)) > 0) {
        productId = value;
    }

    return true;
}



long int X11TabletFinder::getTabletSerial (X11InputDevice& device) const
{
    long        tabletId = 0;
    QList<long> serialIdValues;

    if (!device.getLongProperty(X11Input::PROPERTY_WACOM_SERIAL_IDS, serialIdValues, 1000)) {
        return tabletId;
    }

    // the offset for the tablet id is 0 see wacom-properties.h in the xf86-input-wacom driver for more information on this
    if (serialIdValues.size() > 0) {
        tabletId = serialIdValues.at(0);

        if (tabletId > 0) {
            return tabletId;
        }
    }

    return tabletId;
}



const QString X11TabletFinder::getToolType (X11InputDevice& device) const
{
    QList<long> toolTypeAtoms;

    if (!device.getAtomProperty(X11Input::PROPERTY_WACOM_TOOL_TYPE, toolTypeAtoms)) {
        return QString();
    }

    QString toolTypeName;

    if (toolTypeAtoms.size() == 1) {

#if defined(HAVE_XCB_XINPUT)
        xcb_get_atom_name_cookie_t cookie = xcb_get_atom_name(QX11Info::connection(), toolTypeAtoms.at(0));
        xcb_get_atom_name_reply_t* reply = xcb_get_atom_name_reply(QX11Info::connection(), cookie, NULL);
        if (reply) {
            toolTypeName = QString::fromLatin1(QByteArray(xcb_get_atom_name_name(reply), xcb_get_atom_name_name_length(reply)));
            free(reply);
        }

#else  // HAVE_XCB_XINPUT
        char *type_name = XGetAtomName (device.getDisplay(), (Atom)toolTypeAtoms.at(0));
        if (type_name != NULL) {
            toolTypeName = QLatin1String(type_name);
            XFree( type_name );
        } else {
            dbgWacom << "Could not get tool type of device" << device.getName();
        }

#endif // HAVE_XCB_XINPUT
    }

    return toolTypeName;
}

