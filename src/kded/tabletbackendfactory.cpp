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
#include "tabletbackendfactory.h"

#include "tabletbackend.h"
#include "tabletdatabase.h"
#include "xinputadaptor.h"
#include "xsetwacomadaptor.h"

#include <QtCore/QMap>

using namespace Wacom;

TabletBackendInterface* TabletBackendFactory::m_tabletBackendMock = NULL;
bool TabletBackendFactory::m_isUnitTest = false;



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


TabletBackendInterface* TabletBackendFactory::createBackend(TabletInformation& info)
{
    static TabletBackendFactory factory;

    // return mock object if it exists or if unit testing is enabled
    if (m_tabletBackendMock || m_isUnitTest) {

        if (m_tabletBackendMock == NULL) {
            return NULL;
        }

        // reset tablet backend mock as it will get deleted by the caller
        // therefore we can not return it more than once
        TabletBackendInterface* mock = m_tabletBackendMock;
        m_tabletBackendMock = NULL;
        info = mock->getInformation();
        return mock;
    }

    // lookup tablet information
    QMap<QString,QString> buttonMapping;
    if (!factory.lookupInformation(info, buttonMapping)) {
        return NULL;
    }

    // create tablet backend
    return factory.createInstance(info, buttonMapping);
}




void TabletBackendFactory::setTabletBackendMock(TabletBackendInterface* mock)
{
    m_tabletBackendMock = mock;
}



void TabletBackendFactory::setUnitTest(bool isUnitTest)
{
    m_isUnitTest = isUnitTest;
}


TabletBackendInterface* TabletBackendFactory::createInstance(TabletInformation& info, QMap< QString, QString >& buttonMap)
{
    QString        deviceName;
    TabletBackend* backend = new TabletBackend(info);

    foreach (const DeviceType& type, DeviceType::list()) {

        if (!info.hasDevice(type)) {
            continue;
        }

        deviceName = info.getDeviceName(type);

        if (type == DeviceType::Pad) {
            backend->addAdaptor(type, new XsetwacomAdaptor(deviceName, buttonMap));

        } else if (type == DeviceType::Stylus || type == DeviceType::Eraser || type == DeviceType::Touch) {
            backend->addAdaptor(type, new XsetwacomAdaptor(deviceName));
            backend->addAdaptor(type, new XinputAdaptor(deviceName));

        } else {
            backend->addAdaptor(type, new XsetwacomAdaptor(deviceName));
        }
    }

    return backend;
}



bool TabletBackendFactory::lookupInformation(TabletInformation& info, QMap< QString, QString >& buttonMap)
{
    // lookup tablet information
    TabletDatabase tabletDatabase;

    if (!tabletDatabase.lookupDevice(info, info.tabletId)) {
        kDebug() << "Could not find device in database: " << info.tabletId;
        return false;
    }

    tabletDatabase.lookupButtonMapping(buttonMap, info.companyId, info.tabletId);

    return true;
}

