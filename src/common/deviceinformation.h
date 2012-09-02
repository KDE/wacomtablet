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

#include <QtCore/QString>
#include <QtCore/QStringList>

namespace Wacom
{

struct DeviceInformation
{
    QString companyID;
    QString deviceID;
    QString companyName;
    QString deviceName;
    QString deviceModel;
    QStringList deviceList;
    QString padName;
    QString stylusName;
    QString eraserName;
    QString cursorName;
    QString touchName;
    bool isDeviceAvailable;
    bool hasPadButtons;
};

} // NAMESPACE
#endif // HEADER PROTECTION