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
#include <QtCore/QStringList>
#include <QtGui/QX11Info>

// X11 includes
extern "C" {
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/XInput.h>
//#include <X11/extensions/XInput2.h>
#include <X11/Xutil.h>
}

using namespace Wacom;

/**
 * Helper struct which allows us to forward declare XDevice.
 */
struct X11Utils::XDevice : public ::XDevice {};

bool X11Utils::findTabletDevice(TabletInformation& devinfo)
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
            devinfo.xdeviceId = QString::number(xdevinfo[i].id);

            // TODO do we really have to check all devices?
            //      break after closing device if possible.
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


X11Utils::XDevice* X11Utils::findXDevice(const QString& device)
{
    int         ndevices = 0;
    Display     *dpy     = QX11Info::display();
    XDevice     *xdev    = NULL;
    XDeviceInfo *info    = XListInputDevices( dpy, &ndevices );

    for( int i = 0; i < ndevices; ++i ) {
        if( info[i].name == device.toLatin1() ) {
            xdev = (XDevice*)XOpenDevice( dpy, info[i].id );
            break;
        }
    }

    if (info) {
        XFreeDeviceList( info );
    }

    return xdev;
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



bool X11Utils::getXinputFloatProperty(const QString& device, const QString& property, long nelements, QList<float>& values)
{
    if (device.isEmpty() || property.isEmpty() || nelements < 1) {
        return false;
    }

    long          *data;
    unsigned long  nitems;
    Atom           type;
    int            format;

    if (!getXinputProperty(device, property, 0, nelements, (unsigned char**)&data, &nitems, &type, &format)) {
        return false;
    }

    Display *dpy          = QX11Info::display();
    Atom     expectedType = XInternAtom (dpy, "FLOAT", False);

    if (format != 32 || type != expectedType) {
        kError() << QString::fromLatin1("Unsupported Xinput value size or type!");
        XFree(data);
        return false;
    }

    for (unsigned long i = 0 ; i < nitems ; i++) {
        values.append((float)*(data + i));
    }

    XFree(data);
    return true;
}


bool X11Utils::getXinputLongProperty(const QString& device, const QString& property, long nelements, QList<long>& values)
{
    if (device.isEmpty() || property.isEmpty() || nelements < 1) {
        return false;
    }

    long          *data;
    unsigned long  nitems;
    Atom           type;
    int            format;

    if (!getXinputProperty(device, property, 0, nelements, (unsigned char**)&data, &nitems, &type, &format)) {
        return false;
    }

    if (format != 32 || type != XA_INTEGER) {
        kError() << QString::fromLatin1("Unsupported Xinput value size or type!");
        XFree(data);
        return false;
    }

    for (unsigned long i = 0 ; i < nitems ; i++) {
        values.append(*(data + i));
    }

    XFree(data);
    return true;
}



bool X11Utils::setXinputFloatProperty(const QString& device, const QString& property, const QString& values)
{
    QStringList valueList = values.split (QLatin1String(" "));

    bool         ok;
    QString      svalue;
    float        fvalue;
    QList<float> fvalues;

    for (int i = 0  ; i < valueList.size() ; ++i) {
        svalue = valueList.at(i);

        if (svalue.isEmpty()) {
            continue;
        }

        fvalue = svalue.toFloat(&ok);

        if (!ok) {
            kError() << QString::fromLatin1("Could not convert value '%1' to float!").arg(svalue);
            return false;
        }

        fvalues.append(fvalue);
    }

    return setXinputFloatProperty(device, property, fvalues);
}



bool X11Utils::setXinputFloatProperty(const QString& device, const QString& property, const QList<float>& values)
{
    if (device.isEmpty() || property.isEmpty() || values.size() == 0) {
        kError() << QString::fromLatin1("Can not set property '%1' of device '%2' due to invalid parameters!").arg(property).arg(device);
        return false;
    }

    // convert values
    long *data = new long[values.size()];

    for (int i = 0 ; i < values.size() ; ++i) {
        *(float*)(data + i) = values.at(i);
    }

    // set property
    Display *dpy     = QX11Info::display();
    Atom type        = XInternAtom (dpy, "FLOAT", False);
    bool returnValue = setXinputProperty (device, property, type, (unsigned char*)data, values.size());

    // cleanup
    delete[] data;

    return returnValue;
}



bool X11Utils::setXinputLongProperty(const QString& device, const QString& property, const QString& values)
{
    QStringList valueList = values.split (QLatin1String(" "));

    bool        ok;
    QString     svalue;
    long        lvalue;
    QList<long> lvalues;

    for (int i = 0  ; i < valueList.size() ; ++i) {

        svalue = valueList.at(i);

        if (svalue.isEmpty()) {
            continue;
        }

        lvalue = svalue.toLong(&ok);

        if (!ok) {
            kError() << QString::fromLatin1("Could not convert value '%1' to long!").arg(svalue);
            return false;
        }

        lvalues.append(lvalue);
    }

    return setXinputLongProperty(device, property, lvalues);
}



bool X11Utils::setXinputLongProperty(const QString& device, const QString& property, const QList<long>& values)
{
    if (device.isEmpty() || property.isEmpty() || values.size() == 0) {
        kError() << QString::fromLatin1("Can not set property '%1' of device '%2' due to invalid parameters!").arg(property).arg(device);
        return false;
    }

    // convert values
    long *data = new long[values.size()];

    for (int i = 0 ; i < values.size() ; ++i) {
        data[i] = values.at(i);
    }

    // set property
    bool returnValue = setXinputProperty (device, property, XA_INTEGER, (unsigned char*)data, values.size());

    // cleanup
    delete[] data;

    return returnValue;
}



bool X11Utils::getXinputProperty(const QString& device, const QString& property,
                                 long offset, long length, unsigned char** data,
                                 unsigned long* nitems, Atom* type, int* format)
{
    if (device.isEmpty() || property.isEmpty() || offset < 0 || length < 1 || !data || !nitems || !type || !format) {
        return false;
    }

    *data   = NULL;
    *nitems = 0;
    *type   = None;
    *format = 0;

    // find property
    Display *dpy  = QX11Info::display();
    Atom     prop = XInternAtom (dpy, property.toLatin1(), True);

    if (!prop) {
        kError() << QString::fromLatin1("Can not set unsupported Xinput property '%1'!").arg(property);
        return false;
    }

    // find and open Xinput device
    XDevice *xdev = findXDevice(device);

    if (!xdev) {
        kError() << QString::fromLatin1("Can not get property '%1' from unknown device '%2'!").arg(property).arg(device);
        return false;
    }

    // get property
    unsigned long bytes_after = 0;
    int           retval      = XGetDeviceProperty (dpy, xdev, prop, offset, length, False, AnyPropertyType, type, format, nitems, &bytes_after, data);

    XCloseDevice( QX11Info::display(), xdev );

    return (retval == Success);
}



bool X11Utils::setXinputProperty(const QString& device, const QString& property, X11Utils::Atom type, unsigned char* data, int nelements)
{
    // find property
    Display *dpy   = QX11Info::display();
    Atom     aprop = XInternAtom (dpy, property.toLatin1(), True);

    if (!aprop) {
        kError() << QString::fromLatin1("Can not set unsupported Xinput property '%1'!").arg(property);
        return false;
    }

    // find and open Xinput device
    XDevice *xdev = findXDevice(device);

    if (!xdev) {
        kError() << QString::fromLatin1("Can not set property '%1' on unknown device '%2'!").arg(property).arg(device);
        return false;
    }

    // get & set property
    Atom           atype;
    int            aformat;
    unsigned long  nitems, bytes_after;
    unsigned char *adata  = NULL;

    XGetDeviceProperty (dpy, xdev, aprop, 0, nelements, False, AnyPropertyType, &atype, &aformat, &nitems, &bytes_after, (unsigned char **)&adata);

    bool returnValue = false;

    // all values need to be 32bit in Xinput1
    if (aformat == 32 && atype == type) {
        XChangeDeviceProperty (dpy, xdev, aprop, type, 32, PropModeReplace, data, nelements);
        returnValue = true;
    } else {
        kError() << QString::fromLatin1("Can not set incompatible Xinput property!");
    }

    // cleanup
    XFree(adata);
    XFlush( dpy );
    XCloseDevice( QX11Info::display(), xdev );

    return returnValue;
}



bool X11Utils::mapTabletToScreen(const QString& device, qreal offsetX, qreal offsetY, qreal width, qreal height)
{
    /* XI1 expects 32 bit properties (including float) as long, regardless of architecture */
    long  matrix[9]  = {0};
    float fmatrix[9] = { 1, 0, 0,
                         0, 1, 0,
                         0, 0, 1
                       };

    fmatrix[2] = offsetX;
    fmatrix[5] = offsetY;
    fmatrix[0] = width;
    fmatrix[4] = height;

    // assign float values to long matrix
    for( unsigned int i = 0 ; i < sizeof( matrix ) / sizeof( matrix[0] ) ; ++i ) {
        *(float*)( matrix + i ) = fmatrix[i];
    }

    Atom           type = XInternAtom(QX11Info::display(), "FLOAT", True);
    QLatin1String  property("Coordinate Transformation Matrix");

    return setXinputProperty(device, property, type, (unsigned char*)matrix, 9);
}



bool X11Utils::parseXDevicePropertyToolType(TabletInformation& devinfo, XDevice& xdev, XDeviceInfo& xdevinfo)
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



bool X11Utils::parseXDevicePropertySerialId(TabletInformation& devinfo, XDevice& xdev)
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

            devinfo.tabletId = tabletHexID;
            found            = true;
        }

        if (data != NULL) {
            XFree( data );
        }
    }

    return found;
}



bool X11Utils::parseXDeviceToolType(TabletInformation& devinfo, const QString& xdevtype, XDeviceInfo& xdevinfo)
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
