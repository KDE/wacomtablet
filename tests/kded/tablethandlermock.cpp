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
#include "tablethandlermock.h"

using namespace Wacom;

TabletHandlerMock::TabletHandlerMock() : TabletHandlerInterface(NULL)
{
    m_profile = QLatin1String("default");

    m_profiles.append(QLatin1String("default"));
    m_profiles.append(QLatin1String("myprofile"));
}

TabletHandlerMock::~TabletHandlerMock() {}


void TabletHandlerMock::emitProfileChanged(const QString& profile)
{
    emit profileChanged(profile);
}



void TabletHandlerMock::emitTabletAdded(const TabletInformation& info)
{
    emit tabletAdded(info);
}



void TabletHandlerMock::emitTabletRemoved()
{
    emit tabletRemoved();
}



QString TabletHandlerMock::getProperty(const DeviceType& deviceType, const Property& property) const
{
    QString prop = property.key();

    if (m_deviceType != deviceType.key()) {
        return QLatin1String("ERROR: DEVICE DOES NOT MATCH!");
    }

    if (m_property.compare(prop, Qt::CaseInsensitive) != 0) {
        return QLatin1String("ERROR: PROPERTY DOES NOT MATCH!");
    }

    return m_propertyValue;
}



QStringList TabletHandlerMock::listProfiles() const
{
    return m_profiles;
}



void TabletHandlerMock::setProfile(const QString& profile)
{
    m_profile = profile;
    emitProfileChanged(profile);
}



void TabletHandlerMock::setProperty(const DeviceType& deviceType, const Property& property, const QString& value)
{
    m_deviceType      = deviceType.key();
    m_property        = property.key();
    m_propertyValue   = value;
}

