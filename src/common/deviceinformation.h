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

#ifndef DEVICEINFORMATION_H
#define DEVICEINFORMATION_H

#include "deviceinfo.h"
#include "devicetype.h"

#include <QtCore/QString>
#include <QtCore/QStringList>

namespace Wacom
{

// TODO rename this to TabletInformation
class DeviceInformation
{
public:
    /*
     * DEPRECATED Do not use these members directly any more! Use the getters/setters!
     */
    QString unknown;         //!< A dummy member so we can safely return a reference.
    QString companyID;
    QString deviceID;        // TODO rename this to tabletID
    QString companyName;
    QString deviceName;      // TODO rename this to tabletName 
    QString deviceModel;     // TODO rename this to tabletModel
    QStringList deviceList;
    QString padName;
    QString stylusName;
    QString eraserName;
    QString cursorName;
    QString touchName;
    bool isDeviceAvailable;
    bool hasPadButtons;

    const QString& get(const DeviceInfo& info) const;

    const QStringList& getDeviceList() const;

    const QString& getDeviceName(const DeviceType& device) const;

    bool hasButtons() const;

    bool isAvailable() const;

    void set (const DeviceInfo& info, const QString& value);

    void setAvailable(bool value);
    
    void setDeviceList(const QStringList& list);

    void setDeviceName(const DeviceType& device, const QString& name);
    
    void setButtons(bool value);
};

} // NAMESPACE
#endif // HEADER PROTECTION