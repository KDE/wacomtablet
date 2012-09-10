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
    m_profile     = QLatin1String("default");
    m_flagPenMode = false;
    m_flagTouch   = false;

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


QString TabletHandlerMock::getProperty(const QString& device, const Property& property) const
{
    return getProperty(device, property.key());
}



QString TabletHandlerMock::getProperty(const QString& device, const QString& property) const
{
    //m_lastDeviceGet   = device;
    //m_lastPropertyGet = property;

    return m_property;
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



void TabletHandlerMock::setProperty(const QString& device, const QString& property, const QString& value)
{
    m_lastDeviceSet   = device;
    m_lastPropertySet = property;
    m_property        = value;
}



void TabletHandlerMock::setProperty(const QString& device, const Property& property, const QString& value)
{
    setProperty(device, property.key(), value);
}



void TabletHandlerMock::togglePenMode()
{
    m_flagPenMode = !m_flagPenMode;
}



void TabletHandlerMock::toggleTouch()
{
    m_flagTouch = !m_flagTouch;
}
