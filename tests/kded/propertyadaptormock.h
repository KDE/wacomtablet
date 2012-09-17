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

#ifndef PROPERTYADAPTORMOCK_H
#define PROPERTYADAPTORMOCK_H

#include "propertyadaptor.h"

#include <QtCore/QMap>

namespace Wacom
{
/**
 * A property adaptor mock template class which can be used for
 * any kind of property that uses QString as its key type.
 */
template<class T>
class PropertyAdaptorMock : PropertyAdaptor
{
public:

    PropertyAdaptorMock() : PropertyAdaptor() {}

    virtual const QList<Property> getProperties() const
    {
        return T::ids();
    }

    virtual const QString getProperty(const Property& property) const
    {
        if (!supportsProperty(property)) {
            return QString();
        }

        return m_properties.value(property.key());
    }

    virtual bool setProperty(const Wacom::Property& property, const QString& value)
    {
        if (!supportsProperty(property)) {
            return false;
        }

        m_properties.insert(property.key(), value);
        return true;
    }

    virtual bool supportsProperty(const Property& property) const
    {
        return (T::map(property) != NULL);
    }


    QMap<QString,QString> m_properties;
};
}
#endif
