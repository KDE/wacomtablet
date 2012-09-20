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

#include "tabletbackendmock.h"

using namespace Wacom;

TabletBackendMock::TabletBackendMock()
{
    m_propertyAdaptor     = NULL;
    m_propertyAdaptorType = NULL;
    m_deviceProfileType   = NULL;
}


TabletBackendMock::~TabletBackendMock()
{
    if (m_propertyAdaptor) {
        delete m_propertyAdaptor;
    }
}



void TabletBackendMock::addAdaptor(const DeviceType& deviceType, PropertyAdaptor* adaptor)
{
    if (m_propertyAdaptor) {
        delete m_propertyAdaptor;
    }

    m_propertyAdaptor     = adaptor;
    m_propertyAdaptorType = &deviceType;
}



const TabletInformation& TabletBackendMock::getInformation() const
{
    return m_tabletInformation;
}



const QString TabletBackendMock::getProperty(const DeviceType& type, const Property& property) const
{
    QMap< QString,PropertyAdaptorMock<DeviceProperty> >::const_iterator it = m_properties.find(type.key());

    if (it == m_properties.end()) {
        return QString();
    }

    return it->getProperty(property);
}



void TabletBackendMock::setProfile(const TabletProfile& profile)
{
    m_tabletProfile = profile;
}



void TabletBackendMock::setProfile(const DeviceType& deviceType, const DeviceProfile& profile)
{
    m_deviceProfile = profile;
    m_deviceProfileType = &deviceType;
}



bool TabletBackendMock::setProperty(const DeviceType& type, const Property& property, const QString& value)
{
    QMap< QString,PropertyAdaptorMock<DeviceProperty> >::iterator it = m_properties.find(type.key());

    if (it == m_properties.end()) {
        return false;
    }

    return it->setProperty(property, value);
}


