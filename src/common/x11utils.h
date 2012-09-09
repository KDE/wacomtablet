/*
 * Copyright 2009, 2010 Jörg Ehrichs <joerg.ehichs@gmx.de>
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

#ifndef X11UTILS_H
#define X11UTILS_H

#include "tabletinformation.h"

#include <QtCore/QString>
#include <QtCore/QList>

// X11 forward declarations
struct _XDeviceInfo;

namespace Wacom
{

class X11Utils
{
    public:
        /* Forward Declarations
         * 
         * We can not forward declare X11's' anonymous XDevice struct.
         * To keep the header clean from includes we only need for private
         * methods, we just define our own version of this struct.
         */
        struct XDevice;
        typedef struct _XDeviceInfo XDeviceInfo;
        typedef long unsigned int Atom;

        static bool findTabletDevice (TabletInformation& devinfo);

        static XDevice* findXDevice (const QString& device);

        static bool hasXDeviceProperty (XDevice& xdev, unsigned int property);

        static bool isTabletDevice (int deviceId);

        static bool getXinputFloatProperty (const QString& device, const QString& property, long nelements, QList<float>& values);
        
        static bool getXinputLongProperty (const QString& device, const QString& property, long nelements, QList<long>& values);
        
        static bool setXinputFloatProperty (const QString& device, const QString& property, const QString& values);

        static bool setXinputFloatProperty (const QString& device, const QString& property, const QList<float>& values);

        static bool setXinputLongProperty (const QString& device, const QString& property, const QString& values);
        
        static bool setXinputLongProperty (const QString& device, const QString& property, const QList<long>& values);

        static bool mapTabletToScreen (const QString& device, qreal offsetX, qreal offsetY, qreal width, qreal height);

    private:
        /**
         * Get Xinput1 property values.
         * On success, the memory @a data is pointing to has to be freed using Xfree().
         *
         * @param device   The xinput device to get the value from.
         * @param property The xinput property to get.
         * @param offset   The value offset.
         * @param length   The number of values.
         * @param data     Pointer to a data pointer which will contain all values.
         * @param nitems   Pointer to an unsigned long which will contain the number of values fetched.
         * @param type     Pointer to an Atom which will contain the type of the value.
         * @param format   Pointer to an int which will contain the number of bits of each value.
         * 
         * @return True on success, false on error.
         */
        static bool getXinputProperty (const QString& device, const QString& property, long offset, long length, unsigned char** data, unsigned long* nitems, Atom* type, int* format);

        /**
         * Set Xinput1 property value.
         *
         * @param device    The xinput device to set the value on.
         * @param property  The xinput property to set.
         * @param type      The xinput value type.
         * @param data      Pointer to the data store.
         * @param nelements The number of elements the data store contains.
         *
         * @return True on success, false on error.
         */
        static bool setXinputProperty (const QString& device, const QString& property, Atom type, unsigned char* data, int nelements);

        static bool parseXDevicePropertyToolType (TabletInformation& devinfo, XDevice& xdev, XDeviceInfo& xdevinfo);

        static bool parseXDevicePropertySerialId (TabletInformation& devinfo, XDevice& xdev);

        static bool parseXDeviceToolType (TabletInformation& devinfo, const QString& xdevtype, XDeviceInfo& xdevinfo);

}; // CLASS
}  // NAMESPACE
#endif // HEADER PROTECTION
