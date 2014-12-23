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

#include "x11inputdevice.h"
#include "debug.h"

#include <QtCore/QStringList>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/XInput.h>
//#include <X11/extensions/XInput2.h>
#include <X11/Xutil.h>

using namespace Wacom;

/**
 * Helper struct which allows us to forward declare XDevice.
 */
struct X11InputDevice::XDevice : public ::XDevice {};

/**
 * Class for private members.
 */
namespace Wacom {
    class X11InputDevicePrivate
    {
        public:
            X11InputDevice::XDevice* device;
            Display*                 display;
            QString                  name;
    };
}


X11InputDevice::X11InputDevice() : d_ptr(new X11InputDevicePrivate)
{
    Q_D(X11InputDevice);
    d->device  = NULL;
    d->display = NULL;
}


X11InputDevice::X11InputDevice(Display* dpy, const X11InputDevice::XDeviceInfo& deviceInfo) : d_ptr(new X11InputDevicePrivate)
{
    Q_D(X11InputDevice);
    d->device  = NULL;
    d->display = NULL;

    open(dpy, deviceInfo);
}


X11InputDevice::X11InputDevice(Display* dpy, X11InputDevice::XID id, const QString& name) : d_ptr(new X11InputDevicePrivate)
{
    Q_D(X11InputDevice);
    d->device  = NULL;
    d->display = NULL;

    open(dpy, id, name);
}



X11InputDevice::X11InputDevice(const X11InputDevice& device) : d_ptr(new X11InputDevicePrivate)
{
    Q_D(X11InputDevice);
    d->device  = NULL;
    d->display = NULL;

    operator=(device);
}


X11InputDevice::~X11InputDevice()
{
    close();
    delete d_ptr;
}



X11InputDevice& X11InputDevice::operator= (const X11InputDevice& that)
{
    // close current device
    close();

    // connect new device
    if (that.d_ptr->display && that.d_ptr->device) {
        open(that.d_ptr->display, that.d_ptr->device->device_id, that.d_ptr->name);
    }

    return *this;
}



bool X11InputDevice::close()
{
    Q_D(X11InputDevice);

    if (d->device == NULL) {
        assert(d->display == NULL);
        assert(d->name.isEmpty());
        return false;
    }

    XCloseDevice(d->display, d->device);

    d->display = NULL;
    d->device  = NULL;
    d->name.clear();

    return true;
}



bool X11InputDevice::getAtomProperty(const QString& property, QList< long int >& values, long int nelements) const
{
    return getProperty<long>(property, XA_ATOM, nelements, values);
}


const QList< int > X11InputDevice::getDeviceButtonMapping() const
{
    Q_D(const X11InputDevice);

    QList<int> buttonMap;

    if (!isOpen()) {
        return buttonMap;
    }

    static const int nmap = 100; // maximum number of buttons this method will fetch
    unsigned char    map_return[nmap];
    int              buttonCount = 0;

    if ((buttonCount = XGetDeviceButtonMapping(d->display, d->device, map_return, nmap)) <= 0) {
        return buttonMap; // the device has no buttons
    }

    for (int i = 0 ; i < buttonCount ; ++i) {
        buttonMap.append((int)map_return[i]);
    }

    return buttonMap;
}



long X11InputDevice::getDeviceId() const
{
    Q_D(const X11InputDevice);

    if (!isOpen()) {
        return 0;
    }

    return d->device->device_id;
}



Display* X11InputDevice::getDisplay() const
{
    Q_D(const X11InputDevice);
    return d->display;
}



bool X11InputDevice::getFloatProperty(const QString& property, QList< float >& values, long int nelements) const
{
    Q_D(const X11InputDevice);

    if (!isOpen()) {
        return false;
    }

    Atom expectedType = XInternAtom (d->display, "FLOAT", False);

    if (expectedType == None) {
        qCritical() << QLatin1String("Float values are unsupported by this XInput implementation!");
        return false;
    }

    return getProperty<float>(property, expectedType, nelements, values);
}



bool X11InputDevice::getLongProperty(const QString& property, QList< long int >& values, long int nelements) const
{
    return getProperty<long>(property, XA_INTEGER, nelements, values);
}



const QString& X11InputDevice::getName() const
{
    Q_D(const X11InputDevice);
    return d->name;
}



bool X11InputDevice::getStringProperty(const QString& property, QList< QString >& values, long int nelements) const
{
    // get property data & values
    unsigned char* data           = NULL;
    unsigned long  nitems         = 0;
    int            expectedFormat = 8;

    if (!getPropertyData(property, XA_STRING, expectedFormat, nelements, &data, &nitems)) {
        return false;
    }

    unsigned char* strData = data;

    for (unsigned long i = 0 ; i < nitems ; ++i) {
        // add first string value up to '\0'
        QString value = QLatin1String ((const char*)strData);
        values.append(value);

        // ++i will jump over '\0'
        i       += value.length();
        strData += value.length();
    }

    XFree(data);
    return true;
}



bool X11InputDevice::hasProperty(const QString& property) const
{
    Q_D(const X11InputDevice);

    if (!isOpen()) {
        // some devices like the virtual core keyboard/pointer can not be opened
        qDebug() << QString::fromLatin1("Can not check property '%1' on a device which is not open!").arg(property);
        return false;
    }

    Atom atom;
    if (!lookupProperty(property, &atom)) {
        return false;
    }

    bool  found  = false;
    int   natoms = 0;
    Atom* atoms = XListDeviceProperties (d->display, d->device, &natoms);

    for (int i = 0 ; i < natoms ; ++i) {
        if (atoms[i] == atom) {
            found = true;
            break;
        }
    }

    XFree(atoms);

    return found;
}



bool X11InputDevice::isOpen() const
{
    Q_D(const X11InputDevice);
    return (d->device != NULL && d->display != NULL);
}



bool X11InputDevice::isTabletDevice()
{
    return hasProperty(QLatin1String("Wacom Tool Type"));
}




bool X11InputDevice::open(Display* display, const X11InputDevice::XDeviceInfo& deviceInfo)
{
    if (!open (display, deviceInfo.id, QLatin1String (deviceInfo.name))) {
        return false;
    }

    return true;
}



bool X11InputDevice::open(Display* display, X11InputDevice::XID id, const QString& name)
{
    Q_D(X11InputDevice);

    if (isOpen()) {
        close();
    }

    if (display == NULL || id == 0) {
        qCritical() << QString::fromLatin1("Unable to open device '%1' as invalid parameters were provided!").arg(name);
        return false;
    }

    XDevice* device = (XDevice*) XOpenDevice(display, id);

    if (device == NULL) {
        // some virtual devices can not be opened
        qDebug() << QString::fromLatin1("XOpenDevice failed on device id '%1'!").arg(id);
        return false;
    }

    d->device  = device;
    d->display = display;
    d->name    = name;

    return true;
}



bool X11InputDevice::setDeviceButtonMapping(const QList< int >& buttonMap) const
{
    Q_D(const X11InputDevice);

    if (!isOpen() || buttonMap.count() == 0) {
        return false;
    }

    const int      nmap = buttonMap.count();
    unsigned char* map  = new unsigned char[nmap];

    for (int i = 0 ; i < nmap ; ++i) {
        map[i] = (unsigned char)buttonMap.at(i);
    }

    int result = XSetDeviceButtonMapping(d->display, d->device, map, nmap);

    delete map;

    return (result == MappingSuccess);
}



bool X11InputDevice::setFloatProperty(const QString& property, const QString& values) const
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
            qCritical() << QString::fromLatin1("Could not convert value '%1' to float!").arg(svalue);
            return false;
        }

        fvalues.append(fvalue);
    }

    return setFloatProperty(property, fvalues);
}



bool X11InputDevice::setFloatProperty(const QString& property, const QList< float >& values) const
{
    Q_D(const X11InputDevice);

    if (!isOpen()) {
        return false;
    }

    Atom expectedType = XInternAtom (d->display, "FLOAT", False);

    if (expectedType == None) {
        qCritical() << QLatin1String("Float values are unsupported by this XInput implementation!");
        return false;
    }

    return setProperty<float>(property, expectedType, values);
}



bool X11InputDevice::setLongProperty(const QString& property, const QString& values) const
{
    QStringList valueList = values.split (QLatin1String(" "));

    bool        ok;
    QString     svalue;
    long        lvalue = 0;
    QList<long> lvalues;

    for (int i = 0  ; i < valueList.size() ; ++i) {

        svalue = valueList.at(i);

        if (svalue.isEmpty()) {
            continue;
        }

        lvalue = svalue.toLong(&ok, 10);

        if (!ok) {
            qCritical() << QString::fromLatin1("Could not convert value '%1' to long!").arg(svalue);
            return false;
        }

        lvalues.append(lvalue);
    }

    return setLongProperty(property, lvalues);
}



bool X11InputDevice::setLongProperty(const QString& property, const QList< long int >& values) const
{
    return setProperty<long>(property, XA_INTEGER, values);
}




template<typename T>
bool X11InputDevice::getProperty(const QString& property, X11InputDevice::Atom expectedType, long int nelements, QList< T >& values) const
{
    // get property data & values
    long*          data           = NULL;
    unsigned long  nitems         = 0;
    int            expectedFormat = 32;

    if (!getPropertyData(property, expectedType, expectedFormat, nelements, (unsigned char**)&data, &nitems)) {
        return false;
    }

    for (unsigned long i = 0 ; i < nitems ; ++i) {
        values.append(*((T*)(data + i)));
    }

    XFree(data);

    return true;
}



bool X11InputDevice::getPropertyData (const QString& property, X11InputDevice::Atom expectedType, int expectedFormat, long int nelements, unsigned char** data, long unsigned int* nitems) const
{
    Q_D(const X11InputDevice);

    // check parameters
    if (!isOpen()) {
        qCritical() << QString::fromLatin1 ("Can not get XInput property '%1' as no device was opened!").arg(property);
        return false;
    }

    if (nelements < 1) {
        qCritical() << QString::fromLatin1 ("Can not get XInput property '%1' as less than one element was requested!").arg(property);
        return false;
    }

    // lookup property atom
    Atom propertyAtom = None;

    if (!lookupProperty(property, &propertyAtom)) {
        qCritical() << QString::fromLatin1("Can not get unsupported XInput property '%1'!").arg(property);
        return false;
    }

    // get device property and validate it
    Atom           actualType   = None;
    int            actualFormat = 0;
    unsigned long  bytes_after  = 0;

    if (XGetDeviceProperty (d->display, d->device, propertyAtom, 0, nelements, False, AnyPropertyType, &actualType, &actualFormat, nitems, &bytes_after, data) != Success) {
        qCritical() << QString::fromLatin1("Could not get XInput property '%1'!").arg(property);
        return false;
    }

    if (actualFormat != expectedFormat || actualType != expectedType) {
        qCritical() << QString::fromLatin1("Can not process incompatible Xinput property '%1': Format is '%2', expected was '%3'. Type is '%4', expected was '%5'.").arg(property).arg(actualFormat).arg(expectedFormat).arg(actualType).arg(expectedType);
        XFree(data);
        return false;
    }

    return true;
}



bool X11InputDevice::lookupProperty(const QString& property, X11InputDevice::Atom* atom) const
{
    Q_D(const X11InputDevice);

    if (!isOpen() || property.isEmpty() || atom == NULL) {
        return false;
    }

    *atom = XInternAtom (d->display, property.toLatin1().constData(), True);

    if (*atom == None) {
        qDebug() << QString::fromLatin1("The X server does not support XInput property '%1'!").arg(property);
        return false;
    }

    return true;
}



template<typename T>
bool X11InputDevice::setProperty(const QString& property, X11InputDevice::Atom expectedType, const QList< T >& values) const
{
    Q_D(const X11InputDevice);

    int expectedFormat = 32; // XInput1 uses 32 bit for each property

    // check parameters
    if (!isOpen()) {
        qCritical() << QString::fromLatin1 ("Can not set XInput property '%1' as no device was opened!").arg(property);
        return false;
    }

    if (values.size() == 0) {
        qCritical() << QString::fromLatin1 ("Can not set XInput property '%1' as no values were provided!").arg(property);
        return false;
    }

    // lookup property atom
    Atom propertyAtom = None;

    if (!lookupProperty(property, &propertyAtom)) {
        qCritical() << QString::fromLatin1("Can not set unsupported XInput property '%1'!").arg(property);
        return false;
    }

    // get property and validate format and type
    Atom           actualType;
    int            actualFormat;
    unsigned long  nitems, bytes_after;
    unsigned char *actualData  = NULL;

    if (XGetDeviceProperty (d->display, d->device, propertyAtom, 0, values.size(), False, AnyPropertyType, &actualType, &actualFormat, &nitems, &bytes_after, (unsigned char **)&actualData) != Success) {
        qCritical() << QString::fromLatin1("Could not get XInput property '%1' for type and format validation!").arg(property);
        return false;
    }

    XFree(actualData);

    if (actualFormat != expectedFormat || actualType != expectedType) {
        qCritical() << QString::fromLatin1("Can not process incompatible Xinput property '%1': Format is '%2', expected was '%3'. Type is '%4', expected was '%5'.").arg(property).arg(actualFormat).arg(expectedFormat).arg(actualType).arg(expectedType);
        return false;
    }

    // create new data array - for XInput1 the data always to be of type long
    long *data = new long[values.size()];

    for (int i = 0 ; i < values.size() ; ++i) {
        *((T*)(data + i)) = values.at(i);
    }

    // replace the current data of the property
    XChangeDeviceProperty (d->display, d->device, propertyAtom, expectedType, 32, PropModeReplace, (unsigned char*)data, values.size());

    // cleanup
    delete[] data;

    // flush the output buffer to make sure all properties are updated
    XFlush (d->display);

    return true;
}

