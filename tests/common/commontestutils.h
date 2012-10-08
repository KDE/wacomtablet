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

#ifndef COMMONTESTUTILS_H
#define COMMONTESTUTILS_H

#include "deviceinformation.h"
#include "deviceprofile.h"
#include "devicetype.h"

#include <QtCore/QString>

namespace Wacom {

class CommonTestUtils
{
public:
    /*
     * Expected Values
     */

    static const long    DEVICEINFORMATION_DEVICE_ID;
    static const QString DEVICEINFORMATION_DEVICE_NODE;
    static const long    DEVICEINFORMATION_PRODUCT_ID;
    static const long    DEVICEINFORMATION_TABLET_SERIAL;
    static const long    DEVICEINFORMATION_VENDOR_ID;


    /*
     * Helper Methods
     */

    static void assertValues (const DeviceInformation& info, const DeviceType& expectedType, const QString& expectedName);

    static void assertValues (DeviceProfile& profile, const char* name = NULL);

    static void setValues (DeviceInformation& info);

    static void setValues (DeviceProfile& profile);

}; // CLASS
}  // NAMESPACE
#endif // HEADER PROTECTION
