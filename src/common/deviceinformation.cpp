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

#include "deviceinformation.h"

using namespace Wacom;

const QString& DeviceInformation::get(const DeviceInfo& info) const
{
    if (info == DeviceInfo::CompanyId) {
        return companyId;

    } else if (info == DeviceInfo::CompanyName) {
        return companyName;

    } else if (info == DeviceInfo::CursorName) {
        return getDeviceName(DeviceType::Cursor);

    } else if (info == DeviceInfo::TabletId) {
        return tabletId;

    } else if (info == DeviceInfo::TabletModel) {
        return tabletModel;

    } else if (info == DeviceInfo::TabletName) {
        return tabletName;

    } else if (info == DeviceInfo::EraserName) {
        return getDeviceName(DeviceType::Eraser);

    } else if (info == DeviceInfo::PadName) {
        return getDeviceName(DeviceType::Pad);

    } else if (info == DeviceInfo::StylusName) {
        return getDeviceName(DeviceType::Stylus);

    } else if (info == DeviceInfo::TouchName) {
        return getDeviceName(DeviceType::Touch);

    } else {
        kDebug() << QString::fromLatin1("FIXME: Unknown device information property '%1'!").arg(info.key());
    }

    return unknown;
}



const QStringList& DeviceInformation::getDeviceList() const
{
    return deviceList;
}



const QString& DeviceInformation::getDeviceName(const DeviceType& device) const
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



bool DeviceInformation::hasButtons() const
{
    return hasPadButtons;
}


bool DeviceInformation::isAvailable() const
{
    return isDeviceAvailable;
}



void DeviceInformation::set(const DeviceInfo& info, const QString& value)
{
    if (info == DeviceInfo::CompanyId) {
        companyId = value;

    } else if (info == DeviceInfo::CompanyName) {
        companyName = value;

    } else if (info == DeviceInfo::CursorName) {
        setDeviceName(DeviceType::Cursor, value);

    } else if (info == DeviceInfo::TabletId) {
        tabletId = value;

    } else if (info == DeviceInfo::TabletModel) {
        tabletModel = value;

    } else if (info == DeviceInfo::TabletName) {
        tabletName = value;

    } else if (info == DeviceInfo::EraserName) {
        setDeviceName(DeviceType::Eraser, value);

    } else if (info == DeviceInfo::PadName) {
        setDeviceName(DeviceType::Pad, value);

    } else if (info == DeviceInfo::StylusName) {
        setDeviceName(DeviceType::Stylus, value);

    } else if (info == DeviceInfo::TouchName) {
        setDeviceName(DeviceType::Touch, value);

    } else {
        kDebug() << QString::fromLatin1("FIXME: Can not set unknown device information property '%1' to '%2'!").arg(info.key()).arg(value);
    }
}



void DeviceInformation::setAvailable(bool value)
{
    isDeviceAvailable = value;
}



void DeviceInformation::setDeviceList(const QStringList& list)
{
    deviceList = list;
}



void DeviceInformation::setDeviceName(const DeviceType& device, const QString& name)
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



void DeviceInformation::setButtons(bool value)
{
    hasPadButtons = value;
}
