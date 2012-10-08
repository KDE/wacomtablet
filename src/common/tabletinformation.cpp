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

#include "tabletinformation.h"

namespace Wacom
{
    class TabletInformationPrivate
    {
        public:
            /*
             * Declarations
             */
            typedef QMap<QString,DeviceInformation> DeviceInformationMap;
            typedef QMap<QString,QString>           TabletInfoMap;

            /*
             * Members
             */
            DeviceInformationMap deviceMap;
            TabletInfoMap        infoMap;

            QString unknown;   //!< An empty string which allows us to return a const reference.

            bool isAvailable;
            bool hasButtons;

            /*
             * Helper methods
             */
            TabletInformationPrivate()
            {
                isAvailable = false;
                hasButtons  = false;
            }


            TabletInformationPrivate& operator= (const TabletInformationPrivate& that)
            {
                deviceMap    = that.deviceMap;
                infoMap      = that.infoMap;
                isAvailable  = that.isAvailable;
                hasButtons   = that.hasButtons;

                return *this;
            }

            bool operator== (const TabletInformationPrivate& that) const
            {
                // we don't care if the device is available or not
                if (hasButtons       != that.hasButtons     ||
                    infoMap.size()   != that.infoMap.size() ||
                    deviceMap.size() != that.deviceMap.size()
                   )
                {
                    return false;
                }


                // QMap is always ordered by key so we can just iterate over it and compare keys directly
                TabletInfoMap::const_iterator thisInfoIter = infoMap.constBegin();
                TabletInfoMap::const_iterator thatInfoIter = that.infoMap.constBegin();

                while (thatInfoIter != infoMap.constEnd() && thatInfoIter != that.infoMap.constEnd()) {

                    if (thisInfoIter.key().compare(thatInfoIter.key(), Qt::CaseInsensitive) != 0) {
                        return false;
                    }

                    if (thisInfoIter.value().compare(thatInfoIter.value(), Qt::CaseInsensitive) != 0) {
                        return false;
                    }

                    ++thisInfoIter;
                    ++thatInfoIter;
                }

                DeviceInformationMap::const_iterator thisDevIter = deviceMap.constBegin();
                DeviceInformationMap::const_iterator thatDevIter = that.deviceMap.constBegin();

                while (thisDevIter != deviceMap.constEnd() && thatDevIter != that.deviceMap.constEnd()) {

                    if (thisDevIter.key().compare(thatDevIter.key(), Qt::CaseInsensitive) != 0) {
                        return false;
                    }

                    if (thisDevIter.value() != thatDevIter.value()) {
                        return false;
                    }

                    ++thisDevIter;
                    ++thatDevIter;
                }

                return true;
            }
    }; // CLASS TabletInformationPrivate
} // NAMESPACE Wacom

using namespace Wacom;

TabletInformation::TabletInformation() : d_ptr(new TabletInformationPrivate)
{
}


TabletInformation::TabletInformation(long int tabletSerial) : d_ptr(new TabletInformationPrivate)
{
    set(TabletInfo::TabletSerial, QString::number(tabletSerial));
}


TabletInformation::TabletInformation(const TabletInformation& that) : d_ptr(new TabletInformationPrivate)
{
    operator=(that);
}


TabletInformation::~TabletInformation()
{
    delete d_ptr;
}



TabletInformation& TabletInformation::operator=(const TabletInformation& that)
{
    Q_D(TabletInformation);

    d->operator= (*(that.d_ptr));

    return *this;
}



bool TabletInformation::operator!= (const TabletInformation& other) const
{
    return !operator== (other);
}



bool TabletInformation::operator== (const TabletInformation& other) const
{
    Q_D (const TabletInformation);
    assert(other.d_ptr != NULL);
    return d->operator== (*(other.d_ptr));
}



const QString& TabletInformation::get (const QString& info) const
{
    Q_D (const TabletInformation);

    const TabletInfo* devinfo = TabletInfo::find(info);

    if (devinfo == NULL) {
        kError() << QString::fromLatin1("Can not get unsupported tablet information identifier '%1'!").arg(info);
        return d->unknown;
    }

    return get(*devinfo);
}



const QString& TabletInformation::get (const TabletInfo& info) const
{
    Q_D (const TabletInformation);

    TabletInformationPrivate::TabletInfoMap::const_iterator iter = d->infoMap.constFind(info.key());

    if (iter == d->infoMap.constEnd()) {
        return d->unknown;
    }

    return iter.value();
}



const DeviceInformation* TabletInformation::getDevice (const DeviceType& deviceType) const
{
    Q_D (const TabletInformation);

    TabletInformationPrivate::DeviceInformationMap::ConstIterator iter = d->deviceMap.constFind(deviceType.key());

    if (iter == d->deviceMap.constEnd()) {
        return NULL;
    }

    return &(iter.value());
}



const QStringList TabletInformation::getDeviceList() const
{
    QString     device;
    QStringList deviceList;

    foreach (const DeviceType& type, DeviceType::list()) {
        device = getDeviceName(type);

        if (!device.isEmpty()) {
            deviceList.append(device);
        }
    }

    return deviceList;
}



const QString& TabletInformation::getDeviceName (const QString& device) const
{
    Q_D (const TabletInformation);

    const DeviceType *type = DeviceType::find(device);

    if (type == NULL) {
        kError() << QString::fromLatin1("Unsupported device type '%1'!").arg(device);
        return d->unknown;
    }

    return getDeviceName(*type);
}



const QString& TabletInformation::getDeviceName (const DeviceType& device) const
{
    Q_D(const TabletInformation);

    TabletInformationPrivate::DeviceInformationMap::ConstIterator iter = d->deviceMap.find(device.key());

    if (iter == d->deviceMap.constEnd()) {
        return d->unknown;
    }

    return iter->getName();
}



long int TabletInformation::getTabletSerial() const
{
    return get (TabletInfo::TabletSerial).toLong();
}



bool TabletInformation::hasButtons() const
{
    Q_D(const TabletInformation);
    return d->hasButtons;
}



bool TabletInformation::hasDevice(int deviceId) const
{
    foreach (const DeviceType& type, DeviceType::list()) {
        const DeviceInformation* device = getDevice(type);

        if (device != NULL && device->getDeviceId() == deviceId) {
            return true;
        }
    }

    return false;
}



bool TabletInformation::hasDevice (const DeviceType& device) const
{
    Q_D(const TabletInformation);
    return d->deviceMap.contains(device.key());
}



bool TabletInformation::isAvailable() const
{
    Q_D(const TabletInformation);
    return d->isAvailable;
}



void TabletInformation::set (const TabletInfo& info, const QString& value)
{
    Q_D (TabletInformation);

    // setting the tablet serial requires updating the id for now
    if (info == TabletInfo::TabletSerial) {
        long serial = value.toLong();

        if (serial > 0) {
            set (TabletInfo::TabletId, QString::fromLatin1("%1").arg(serial, 4, 16, QLatin1Char('0')).toUpper());
        }
    }

    TabletInformationPrivate::TabletInfoMap::iterator iter = d->infoMap.find(info.key());

    // make sure only keys exist which actually have a value
    if (iter != d->infoMap.end()) {

        if (value.isEmpty()) {
            d->infoMap.erase(iter);
        } else {
            iter.value() = value;
        }

    } else if (!value.isEmpty()){
        d->infoMap.insert(info.key(), value);
    }
}



void TabletInformation::setAvailable(bool value)
{
    Q_D(TabletInformation);
    d->isAvailable = value;
}



void TabletInformation::setDevice (const DeviceInformation& device)
{
    Q_D(TabletInformation);
    d->deviceMap.insert (device.getType().key(), device);
}



void TabletInformation::setButtons(bool value)
{
    Q_D(TabletInformation);
    d->hasButtons = value;
}

