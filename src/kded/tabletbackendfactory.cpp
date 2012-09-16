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
#include "tabletbackendfactory.h"
#include "tabletdatabase.h"
#include "xinputadaptor.h"
#include "xsetwacomadaptor.h"

#include <QtCore/QMap>

using namespace Wacom;

TabletBackendFactory::TabletBackendFactory() {}

TabletBackendFactory::TabletBackendFactory(const TabletBackendFactory& factory)
{
    // prevent compiler warnings, at least for debug builds
    assert (&factory != NULL);
}

TabletBackendFactory& TabletBackendFactory::operator=(const TabletBackendFactory& factory)
{
    // prevent compiler warnings, at least for debug builds
    assert (&factory != NULL);
    return *this;
}


TabletBackend* TabletBackendFactory::createBackend(TabletInformation& info)
{
    TabletDatabase tabletDatabase;

    // lookup tablet information
    if (!tabletDatabase.lookupDevice(info, info.tabletId)) {
        kDebug() << "Could not find device in database: " << info.tabletId;
        return NULL;
    }

    // lookup tablet button mapping
    QMap<QString,QString> buttonMapping;
    tabletDatabase.lookupButtonMapping(buttonMapping, info.companyId, info.tabletId);


    // create tablet backend
    QString        deviceName;
    TabletBackend* backend = new TabletBackend(info);

    foreach (const DeviceType& type, DeviceType::list()) {

        if (!info.hasDevice(type)) {
            continue;
        }

        deviceName = info.getDeviceName(type);

        if (type == DeviceType::Pad) {
            backend->addAdaptor(type, new XsetwacomAdaptor(deviceName, buttonMapping));

        } else if (type == DeviceType::Stylus || type == DeviceType::Eraser || type == DeviceType::Touch) {
            backend->addAdaptor(type, new XsetwacomAdaptor(deviceName));
            backend->addAdaptor(type, new XinputAdaptor(deviceName));

        } else {
            backend->addAdaptor(type, new XsetwacomAdaptor(deviceName));
        }
    }

    return backend;
}


