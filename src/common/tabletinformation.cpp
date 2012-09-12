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

#include "tabletinformation.h"

using namespace Wacom;

TabletInformation::TabletInformation()
{
    isTabletAvailable = false;
    hasPadButtons     = false;
}


bool TabletInformation::operator!=(const TabletInformation& other) const
{
    return !operator==(other);
}



bool TabletInformation::operator==(const TabletInformation& other) const
{
    if (xdeviceId.compare   (other.xdeviceId,   Qt::CaseInsensitive) != 0 ||
        companyId.compare   (other.companyId,   Qt::CaseInsensitive) != 0 ||
        companyName.compare (other.companyName, Qt::CaseInsensitive) != 0 ||
        tabletId.compare    (other.tabletId,    Qt::CaseInsensitive) != 0 ||
        tabletModel.compare (other.tabletModel, Qt::CaseInsensitive) != 0 ||
        tabletName.compare  (other.tabletName,  Qt::CaseInsensitive) != 0 ||
        padName.compare     (other.padName,     Qt::CaseInsensitive) != 0 ||
        stylusName.compare  (other.stylusName,  Qt::CaseInsensitive) != 0 ||
        eraserName.compare  (other.eraserName,  Qt::CaseInsensitive) != 0 ||
        cursorName.compare  (other.cursorName,  Qt::CaseInsensitive) != 0 ||
        touchName.compare   (other.touchName,   Qt::CaseInsensitive) != 0 ||
        isTabletAvailable != other.isTabletAvailable ||
        hasPadButtons     != other.hasPadButtons
    )
    {
        return false;
    }

    return true;
}



const QString& TabletInformation::get(const QString& info) const
{
    const TabletInfo* devinfo = TabletInfo::find(info);

    if (devinfo == NULL) {
        kError() << QString::fromLatin1("Unsupported tablet info identifier '%1'!").arg(info);
        return unknown;
    }

    return get(*devinfo);
}


const QString& TabletInformation::get(const TabletInfo& info) const
{
    if (info == TabletInfo::CompanyId) {
        return companyId;

    } else if (info == TabletInfo::CompanyName) {
        return companyName;

    } else if (info == TabletInfo::CursorName) {
        return getDeviceName(DeviceType::Cursor);

    } else if (info == TabletInfo::TabletId) {
        return tabletId;

    } else if (info == TabletInfo::TabletModel) {
        return tabletModel;

    } else if (info == TabletInfo::TabletName) {
        return tabletName;

    } else if (info == TabletInfo::EraserName) {
        return getDeviceName(DeviceType::Eraser);

    } else if (info == TabletInfo::PadName) {
        return getDeviceName(DeviceType::Pad);

    } else if (info == TabletInfo::StylusName) {
        return getDeviceName(DeviceType::Stylus);

    } else if (info == TabletInfo::TouchName) {
        return getDeviceName(DeviceType::Touch);

    } else {
        kDebug() << QString::fromLatin1("FIXME: Unknown device information property '%1'!").arg(info.key());
    }

    return unknown;
}



const QStringList& TabletInformation::getDeviceList() const
{
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
        return cursorName;

    } else if (device == DeviceType::Eraser) {
        return eraserName;

    } else if (device == DeviceType::Pad) {
        // if no pad is present, use stylus name as alternative way
        // fixes some problems with serial TabletPC that do not have a pad as such but still
        // can handle pad rotations and such when applied to the stylus settings
        if (padName.isEmpty()) {
            return cursorName;
        }
        
        return padName;

    } else if (device == DeviceType::Stylus) {
        return stylusName;

    } else if (device == DeviceType::Touch) {
        return touchName;

    } else {
        kDebug() << QString::fromLatin1("FIXME: Unknown device type '%1'!").arg(device.key());
    }

    return unknown;
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
    if (info == TabletInfo::CompanyId) {
        companyId = value;

    } else if (info == TabletInfo::CompanyName) {
        companyName = value;

    } else if (info == TabletInfo::CursorName) {
        setDeviceName(DeviceType::Cursor, value);

    } else if (info == TabletInfo::TabletId) {
        tabletId = value;

    } else if (info == TabletInfo::TabletModel) {
        tabletModel = value;

    } else if (info == TabletInfo::TabletName) {
        tabletName = value;

    } else if (info == TabletInfo::EraserName) {
        setDeviceName(DeviceType::Eraser, value);

    } else if (info == TabletInfo::PadName) {
        setDeviceName(DeviceType::Pad, value);

    } else if (info == TabletInfo::StylusName) {
        setDeviceName(DeviceType::Stylus, value);

    } else if (info == TabletInfo::TouchName) {
        setDeviceName(DeviceType::Touch, value);

    } else {
        kDebug() << QString::fromLatin1("FIXME: Can not set unknown device information property '%1' to '%2'!").arg(info.key()).arg(value);
    }
}



void TabletInformation::setAvailable(bool value)
{
    isTabletAvailable = value;
}



void TabletInformation::setDeviceList(const QStringList& list)
{
    deviceList = list;
}



void TabletInformation::setDeviceName(const DeviceType& device, const QString& name)
{
    if (device == DeviceType::Cursor) {
        cursorName = name;

    } else if (device == DeviceType::Eraser) {
        eraserName = name;

    } else if (device == DeviceType::Pad) {
        padName = name;

    } else if (device == DeviceType::Stylus) {
        stylusName = name;

    } else if (device == DeviceType::Touch) {
        touchName = name;

    } else {
        kDebug() << QString::fromLatin1("FIXME: Unknown device type '%1'!").arg(device.key());
    }
}



void TabletInformation::setButtons(bool value)
{
    hasPadButtons = value;
}
