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

#ifndef X11INPUTDEVICE_H
#define X11INPUTDEVICE_H

#include <QtCore/QString>
#include <QtCore/QList>

#include <QtGui/QX11Info>

// X11 forward declarations
struct _XDeviceInfo;

namespace Wacom
{

class X11InputDevicePrivate;

/**
 * XInput device implementation. It offers access to all X11 input
 * properties and some helper methods for tablet detection.
 */
class X11InputDevice
{
public:
    /*
     * Typedefs & Forward Declarations
     */
    typedef long unsigned int Atom;
    typedef long unsigned int XID;
    typedef struct _XDeviceInfo XDeviceInfo;

    /*
     * We can not forward declare X11's' anonymous XDevice struct.
     * To keep the header clean from includes we only need for private
     * methods, we just define our own version of this struct.
     */
    struct XDevice;

    /**
     * Default Constructor
     */
    X11InputDevice();

    /**
     * Constructor which opens a device directly.
     */
    X11InputDevice (Display* dpy, const XDeviceInfo& deviceInfo);

    /**
     * Constructor which opens a device directly.
     */
    X11InputDevice (Display* dpy, XID id, const QString& name);

    /**
     * Copy Constructor
     */
    X11InputDevice (const X11InputDevice& device);

    /**
     * Default Destructor
     */
    virtual ~X11InputDevice();

    /**
     * Copy Operator
     */
    X11InputDevice& operator= (const X11InputDevice& that);

    /**
     * Closes this device.
     *
     * @return True if the device was successfully closed, else false.
     */
    bool close();

    /**
     * Gets an Atom property.
     *
     * @param property  The property to get.
     * @param nelements The maximum number of elements to get.
     * @param values    A reference to a QList which will contain the values on success.
     *
     * @return True if the property could be retrieved, else false.
     */
    bool getAtomProperty (const QString& property, QList<long>& values, long nelements = 1) const;

    /**
     * Gets the button map of the device.
     *
     * @return A list of buttons numbers.
     */
    const QList<int> getDeviceButtonMapping() const;

    /**
     * Gets the device id of this device.
     *
     * @return The device id or 0.
     */
    long int getDeviceId() const;

    /**
     * Returns the display of this device or NULL.
     */
    Display* getDisplay() const;

    /**
     * Gets a float property.
     *
     * @param property  The property to get.
     * @param nelements The maximum number of elements to get.
     * @param values    A reference to a QList which will contain the values on success.
     *
     * @return True if the property could be retrieved, else false.
     */
    bool getFloatProperty (const QString& property, QList<float>& values, long nelements = 1) const;

    /**
     * Gets a long property.
     *
     * @param property  The property to get.
     * @param nelements The maximum number of elements to get.
     * @param values    A reference to a QList which will contain the values on success.
     *
     * @return True if the property could be retrieved, else false.
     */
    bool getLongProperty (const QString& property, QList<long>& values, long nelements = 1) const;

    /**
     * Returns the name of this XInput device. Beware that this name can not be used
     * to reliably detect a certain device as the name can be configured in xorg.conf.
     *
     * @return The name of this device.
     */
    const QString& getName() const;

    /**
     * Gets a string property.
     *
     * @param property The property to get.
     * @param nelements The maximum number of elements to get.
     * @param values    A reference to a QList which will contain the values on success.
     *
     * @return True if the property could be retrieved, else false.
     */
    bool getStringProperty (const QString& property, QList<QString>& values, long nelements = 1024) const;

    /**
     * Checks if this device has the given property. The device has to be open for
     * this method to succeed.
     *
     * @return True if this device has the property, else false.
     */
    bool hasProperty (const QString& property) const;

    /**
     * Checks if the device is open.
     *
     * @return True if the device is open, else false.
     */
    bool isOpen() const;

    /**
     * Checks if this device is a tablet device.
     *
     * @return True if this device is a tablet device, else false.
     */
    bool isTabletDevice();

    /**
     * Opens a X11 device.
     *
     * @param display    The X11 display to use.
     * @param deviceInfo The X11 device info structure.
     *
     * @return True if the device was successfully opened, else false.
     */
    bool open (Display* display, const XDeviceInfo& deviceInfo);

    /**
     * Opens the given device id.
     *
     * @param display The X11 display.
     * @param id      The X11 device id of the device to open.
     * @param name    The name of the device.
     *
     * @return True on success, false on error.
     */
    bool open (Display* display, XID id, const QString& name);

    /**
     * Sets a button mapping on the device. The parameter \a buttonMap has
     * to contain as many values as returned by \a getDeviceButtonMapping().
     *
     * @param buttonMap The new button map for the device.
     *
     * @return True on success, false on error.
     */
    bool setDeviceButtonMapping(const QList<int>& buttonMap) const;

    /**
     * Sets a float property. The values have to be separated by a single whitespace.
     *
     * @param property The property to set.
     * @param values   A string containing all values separated by a whitespace.
     *
     * @return True if the property could be set, else false.
     */
    bool setFloatProperty (const QString& property, const QString& values) const;

    /**
     * Sets a float property.
     *
     * @param propert The property to set.
     * @param values  A list of values to set on this property.
     *
     * @return True if the property could be set, else false.
     */
    bool setFloatProperty (const QString& property, const QList<float>& values) const;

    /**
     * Sets a long property. The values have to be separated by a single whitespace.
     *
     * @param property The property to set.
     * @param values   A string containing all values separated by a whitespace.
     *
     * @return True if the property could be set, else false.
     */
    bool setLongProperty (const QString& property, const QString& values) const;

    /**
     * Sets a long property.
     *
     * @param property The property to set.
     * @param values   A list of values to set on this property.
     *
     * @return True if the property could be set, else false.
     */
    bool setLongProperty (const QString& property, const QList<long>& values) const;


private:

    /**
     * A template method which fetches property values from this device.
     *
     * @param property     The property to get.
     * @param expectedType The expected Xinput type of the property.
     * @param nelements    The maximum number of elements to fetch.
     * @param values       A reference to a list which will contain the values on success.
     *
     * @return True if the property could be fetched, else false.
     */
    template<typename T>
    bool getProperty (const QString& property, Atom expectedType, long nelements, QList< T >& values) const;

    /**
     * Retrieves X11 property values.
     * On success the data parameter has to be freed using Xfree().
     *
     * @param property       The property go get.
     * @param expectedType   The expected Xinput property type.
     * @param expectedFormat The expected Xinput property format.
     * @param data           A pointer to a data pointer which will be set to the retrieved data.
     * @param nitems         A pointer to a counter which will contain the number of items which were retrieved.
     *
     * @return True on success, false on error.
     */
    bool getPropertyData (const QString& property, Atom expectedType, int expectedFormat, long nelements, unsigned char** data, unsigned long* nitems) const;

    /**
     * Looks up a X11 property atom.
     *
     * @param property The property to lookup.
     * @param atom     Pointer to an Atom which will contain the result.
     *
     * @return True if the property could be resolved, else false.
     */
    bool lookupProperty (const QString& property, Atom* atom) const;

    /**
     * A template method which sets a property on this device. The property has to exist already!
     *
     * @param property     The property to set.
     * @param expectedType The expected type of the property.
     * @param values       The values to set on the property.
     *
     * @return True if the property could be set, else false.
     */
    template<typename T>
    bool setProperty (const QString& property, Atom expectedType, const QList<T>& values) const;


    Q_DECLARE_PRIVATE(X11InputDevice)
    X11InputDevicePrivate *const d_ptr;

}; // CLASS
}  // NAMESPACE
#endif // HEADER PROTECTION
