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

#include "deviceinformation.h"

namespace Wacom
{
    class DeviceInformationPrivate
    {
        public:
            DeviceInformationPrivate (const DeviceType& type) : deviceType (type) {}

            QString    deviceName;
            QString    deviceNode;
            DeviceType deviceType;
            long       deviceId;
            long       productId;
            long       tabletId;
            long       vendorId;
    };
}

using namespace Wacom;

DeviceInformation::DeviceInformation (const DeviceType& deviceType, const QString& deviceName)
    : d_ptr (new DeviceInformationPrivate (deviceType))
{
    Q_D (DeviceInformation);

    d->deviceId   = 0;
    d->deviceName = deviceName;
    d->productId  = 0;
    d->tabletId   = 0;
    d->vendorId   = 0;
}


DeviceInformation::DeviceInformation (const DeviceInformation& that)
    : d_ptr (new DeviceInformationPrivate (that.d_ptr->deviceType))
{
    operator= (that);
}


DeviceInformation::~DeviceInformation()
{
    delete d_ptr;
}



DeviceInformation& DeviceInformation::operator= (const DeviceInformation& that)
{
    Q_D (DeviceInformation);

    d->deviceId   = that.d_ptr->deviceId;
    d->deviceName = that.d_ptr->deviceName;
    d->deviceNode = that.d_ptr->deviceNode;
    d->deviceType = that.d_ptr->deviceType;
    d->productId  = that.d_ptr->productId;
    d->tabletId   = that.d_ptr->tabletId;
    d->vendorId   = that.d_ptr->vendorId;

    return *this;
}



bool DeviceInformation::operator!= (const DeviceInformation& that) const
{
    return !operator==(that);
}



bool DeviceInformation::operator== (const DeviceInformation& that) const
{
    Q_D (const DeviceInformation);

    if (d->deviceName.compare(that.d_ptr->deviceName, Qt::CaseInsensitive) != 0 ||
        d->deviceNode.compare(that.d_ptr->deviceNode, Qt::CaseInsensitive) != 0 ||
        d->deviceId   != that.d_ptr->deviceId   ||
        d->deviceType != that.d_ptr->deviceType ||
        d->productId  != that.d_ptr->productId  ||
        d->tabletId   != that.d_ptr->tabletId   ||
        d->vendorId   != that.d_ptr->vendorId)
    {
        return false;
    }

    return true;
}


long int DeviceInformation::getDeviceId() const
{
    Q_D (const DeviceInformation);
    return d->deviceId;
}


const QString& DeviceInformation::getDeviceNode() const
{
    Q_D (const DeviceInformation);
    return d->deviceNode;
}



const QString& DeviceInformation::getName() const
{
    Q_D (const DeviceInformation);
    return d->deviceName;
}



long int DeviceInformation::getProductId() const
{
    Q_D (const DeviceInformation);
    return d->productId;
}



long int DeviceInformation::getTabletSerial() const
{
    Q_D (const DeviceInformation);
    return d->tabletId;
}



const DeviceType& DeviceInformation::getType() const
{
    Q_D (const DeviceInformation);
    return d->deviceType;
}



long int DeviceInformation::getVendorId() const
{
    Q_D (const DeviceInformation);
    return d->vendorId;
}



void DeviceInformation::setDeviceId(long int deviceId)
{
    Q_D (DeviceInformation);
    d->deviceId = deviceId;
}


void DeviceInformation::setDeviceNode (const QString& deviceNode)
{
    Q_D (DeviceInformation);
    d->deviceNode = deviceNode;
}



void DeviceInformation::setProductId (long productId)
{
    Q_D (DeviceInformation);
    d->productId = productId;
}



void DeviceInformation::setTabletSerial (long tabletId)
{
    Q_D (DeviceInformation);
    d->tabletId = tabletId;
}



void DeviceInformation::setVendorId (long vendorId)
{
    Q_D (DeviceInformation);
    d->vendorId = vendorId;
}
