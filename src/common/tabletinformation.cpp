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

using namespace Wacom;

TabletInformation::TabletInformation()
{
    isTabletAvailable = false;
    hasPadButtons     = false;
}


bool TabletInformation::operator!= (const TabletInformation& other) const
{
    return !operator==(other);
}



bool TabletInformation::operator== (const TabletInformation& other) const
{
    // compare booleans
    if (isTabletAvailable != other.isTabletAvailable || hasPadButtons != other.hasPadButtons) {
        return false;
    }

    // only keys which do have a value should be set
    if (infoMap.size() != other.infoMap.size()) {
        return false;
    }

    // QMap is always ordered by key so we can just iterate over it and compare keys directly
    QMap<QString,QString>::const_iterator thisIter = infoMap.constBegin();
    QMap<QString,QString>::const_iterator thatIter = other.infoMap.constBegin();

    while (thisIter != infoMap.constEnd() && thatIter != other.infoMap.constEnd()) {

        if (thisIter.key().compare(thatIter.key(), Qt::CaseInsensitive) != 0) {
            return false;
        }

        if (thisIter.value().compare(thatIter.value(), Qt::CaseInsensitive) != 0) {
            return false;
        }

        ++thisIter;
        ++thatIter;
    }

    return true;
}



const QString& TabletInformation::get(const QString& info) const
{
    const TabletInfo* devinfo = TabletInfo::find(info);

    if (devinfo == NULL) {
        kError() << QString::fromLatin1("Can not get unsupported tablet information identifier '%1'!").arg(info);
        return unknown;
    }

    return get(*devinfo);
}


const QString& TabletInformation::get(const TabletInfo& info) const
{
    QMap<QString,QString>::const_iterator iter = infoMap.constFind(info.key());

    if (iter == infoMap.constEnd()) {
        return unknown;
    }

    return iter.value();
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



const QString& TabletInformation::getDeviceName(const QString& device) const
{
    const DeviceType *type = DeviceType::find(device);

    if (type == NULL) {
        kError() << QString::fromLatin1("Unsupported device type '%1'!").arg(device);
        return unknown;
    }

    return getDeviceName(*type);
}



const QString& TabletInformation::getDeviceName(const DeviceType& device) const
{
    if (device == DeviceType::Cursor) {
        return get (TabletInfo::CursorName);

    } else if (device == DeviceType::Eraser) {
        return get (TabletInfo::EraserName);

    } else if (device == DeviceType::Pad) {

        // if no pad is present, use stylus name as alternative way
        // fixes some problems with serial TabletPC that do not have a pad as such but still
        // can handle pad rotations and such when applied to the stylus settings
        if (get(TabletInfo::PadName).isEmpty()) {
            return get (TabletInfo::CursorName);
        }

        return get(TabletInfo::PadName);

    } else if (device == DeviceType::Stylus) {
        return get (TabletInfo::StylusName);

    } else if (device == DeviceType::Touch) {
        return get (TabletInfo::TouchName);

    } else {
        kDebug() << QString::fromLatin1("FIXME: Unknown device type '%1'!").arg(device.key());
    }

    return unknown;
}



int TabletInformation::getXDeviceId() const
{
    QString id = get(TabletInfo::DeviceId);

    if (id.isEmpty()) {
        return 0;
    }

    bool ok;
    int dev_id = id.toInt(&ok, 10);

    return (ok ? dev_id : 0);
}



bool TabletInformation::hasButtons() const
{
    return hasPadButtons;
}


bool TabletInformation::hasDevice(const DeviceType& device) const
{
    return !getDeviceName(device).isEmpty();
}


bool TabletInformation::isAvailable() const
{
    return isTabletAvailable;
}



void TabletInformation::set(const TabletInfo& info, const QString& value)
{
    QMap<QString,QString>::iterator iter = infoMap.find(info.key());

    // make sure only keys exist which actually have a value
    if (iter != infoMap.end()) {

        if (value.isEmpty()) {
            infoMap.erase(iter);

        } else {
            iter.value() = value;
        }

    } else if (!value.isEmpty()){
        infoMap.insert(info.key(), value);
    }
}



void TabletInformation::setAvailable(bool value)
{
    isTabletAvailable = value;
}



void TabletInformation::setDeviceName(const DeviceType& device, const QString& name)
{
    if (device == DeviceType::Cursor) {
        set (TabletInfo::CursorName, name);

    } else if (device == DeviceType::Eraser) {
        set (TabletInfo::EraserName, name);

    } else if (device == DeviceType::Pad) {
        set (TabletInfo::PadName, name);

    } else if (device == DeviceType::Stylus) {
        set (TabletInfo::StylusName, name);

    } else if (device == DeviceType::Touch) {
        set (TabletInfo::TouchName, name);

    } else {
        kDebug() << QString::fromLatin1("FIXME: Unknown device type '%1'!").arg(device.key());
    }
}



void TabletInformation::setButtons(bool value)
{
    hasPadButtons = value;
}
