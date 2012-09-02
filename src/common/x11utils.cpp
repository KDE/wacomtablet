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
#include "x11utils.h"

// Qt includes
#include <QtGui/QX11Info>

// X11 includes
extern "C" {
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/XInput.h>
}

using namespace Wacom;

/**
 * Helper struct which allows us to forward declare XDevice.
 */
struct X11Utils::XDevice : public ::XDevice {};

bool X11Utils::isTabletDevice(int deviceId)
{
    uint property = XInternAtom( QX11Info::display(), "Wacom Tool Type", True );
    bool isTablet = false;

    XDevice *dev = (XDevice*)XOpenDevice(QX11Info::display(), deviceId);

    if (dev) {
        isTablet = hasXDeviceProperty(*dev, property);
        XCloseDevice(QX11Info::display(), dev);
    }

    return isTablet;
}


bool X11Utils::findTabletDevice(DeviceInformation& devinfo)
{
    bool         devFound = false;
    XDevice     *xdev     = NULL;
    XDeviceInfo *xdevinfo = NULL;
    int          ndevices = 0;

    xdevinfo = XListInputDevices( QX11Info::display(), &ndevices );

    for( int i = 0; i < ndevices; ++i ) {

        xdev = (XDevice*)XOpenDevice( QX11Info::display(), xdevinfo[i].id );

        if( xdev == NULL ) {
            continue;
        }

        devFound = parseXDevicePropertyToolType(devinfo, *xdev, xdevinfo[i]);

        if (devFound) {
            parseXDevicePropertySerialId(devinfo, *xdev);
        }

        XCloseDevice( QX11Info::display(), xdev );
    }

    XFreeDeviceList( xdevinfo );
    
    // if we just have no pad name only a name for a stylus...copy that, this is how the touchpc devices seem to work
    if( devinfo.padName.isEmpty() ) {
        devinfo.padName = devinfo.stylusName;
    }

    return devFound;
}


bool X11Utils::hasXDeviceProperty(XDevice& xdev, unsigned int property)
{
    bool propertyFound = false;

    int  natoms = 0;
    Atom *atoms = XListDeviceProperties( QX11Info::display(), &xdev, &natoms );

    if( natoms > 0 ) {
        for( int j = 0; j < natoms; ++j ) {
            if( atoms[j] == property ) {
                propertyFound = true;
                break;
            }
        }
    }

    if (atoms != NULL) {
        XFree( atoms );
    }

    return propertyFound;
}



bool X11Utils::parseXDevicePropertyToolType(DeviceInformation& devinfo, XDevice& xdev, XDeviceInfo& xdevinfo)
{
    uint           property = XInternAtom( QX11Info::display(), "Wacom Tool Type", True );
    bool           devFound = false;
    unsigned char *data     = NULL;
    Atom           type     = 0;
    int            format   = 0;
    unsigned long  nitems   = 0, bytes_after = 0;

    if (!hasXDeviceProperty(xdev, property)) {
        return false;
    }

    XGetDeviceProperty( QX11Info::display(), &xdev, property, 0, 1, False, AnyPropertyType, &type,
                        &format, &nitems, &bytes_after, &data );

    // if nitems is not 0 than atleast one wacom tool device exist
    if( nitems ) {
        devFound = true;

        char *type_name = XGetAtomName( QX11Info::display(), *(Atom*)data );

        parseXDeviceToolType(devinfo, QString::fromLatin1(type_name), xdevinfo );

        XFree( type_name );
    }

    if (data != NULL) {
        XFree( data );
    }

    return devFound;
}


bool X11Utils::parseXDevicePropertySerialId(DeviceInformation& devinfo, XDevice& xdev)
{
    bool found = false;
    Atom prop  = XInternAtom( QX11Info::display(), "Wacom Serial IDs", True );

    if( !prop ) {
        kDebug() << "Property: Wacom Serial IDs not available";

    } else {
        Atom           type   = 0;
        int            format = 0;
        unsigned char *data   = NULL;
        unsigned long  nitems = 0, bytes_after = 0;

        XGetDeviceProperty( QX11Info::display(), &xdev, prop, 0, 1000, False, AnyPropertyType, &type, &format, &nitems, &bytes_after, &data );

        // the offset for the tablet id is 0 see wacom-properties.h in the xf86-input-wacom driver for more information on this
        long *ldata = ( long * )data;

        if( ldata ) {
            //transform tablet number into hex number (somehow .toInt(&ok,16) does not work
            QString tabletHexID = QString::number( ldata[0], 16 );

            for( int i = tabletHexID.count(); i < 4; i++ ) {
                tabletHexID.prepend( QLatin1String( "0" ) );
            }

            devinfo.deviceID = tabletHexID;
            found            = true;
        }

        if (data != NULL) {
            XFree( data );
        }
    }

    return found;
}



bool X11Utils::parseXDeviceToolType(DeviceInformation& devinfo, const QString& xdevtype, XDeviceInfo& xdevinfo)
{
    // add the device to the device list
    devinfo.deviceList.append(QString::fromLatin1(xdevinfo.name));
    
    //check what type we found (with Type is Wacom xxx) and save internal reference)
    if( xdevtype.contains( QLatin1String( "pad" ), Qt::CaseInsensitive ) ) {
        devinfo.padName = QString::fromLatin1(xdevinfo.name);
        return true;

    } else if( xdevtype.contains( QLatin1String( "eraser" ), Qt::CaseInsensitive ) ) {
        devinfo.eraserName = QString::fromLatin1(xdevinfo.name);
        return true;

    } else if( xdevtype.contains( QLatin1String( "cursor" ), Qt::CaseInsensitive ) ) {
        devinfo.cursorName = QString::fromLatin1(xdevinfo.name);
        return true;

    } else if( xdevtype.contains( QLatin1String( "touch" ),  Qt::CaseInsensitive ) ) {
        devinfo.touchName = QString::fromLatin1(xdevinfo.name);
        return true;

    } else if( xdevtype.contains( QLatin1String( "stylus" ), Qt::CaseInsensitive ) ) {
        devinfo.stylusName = QString::fromLatin1(xdevinfo.name);
        return true;
    }

    return false;
}
