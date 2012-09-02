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
#include "deviceprofilexsetwacomadaptor.h"

#include <assert.h>
#include <QtCore/QRegExp>

using namespace Wacom;

namespace Wacom {
/**
  * Private class for the d-pointer.
  */
class DeviceProfileXsetwacomAdaptorPrivate {
public:
    DeviceProfile* profile;
};
}

DeviceProfileXsetwacomAdaptor::DeviceProfileXsetwacomAdaptor(DeviceProfile& profile)
    : PropertyAdaptor(&profile), d_ptr(new DeviceProfileXsetwacomAdaptorPrivate)
{
    Q_D( DeviceProfileXsetwacomAdaptor );
    d->profile = &profile;
}

DeviceProfileXsetwacomAdaptor::~DeviceProfileXsetwacomAdaptor()
{
    delete this->d_ptr;
}


const QList< Property > DeviceProfileXsetwacomAdaptor::getProperties() const
{
    Q_D( const DeviceProfileXsetwacomAdaptor );
    assert(d->profile != NULL);

    QList<Property>          result;
    QList<XsetwacomProperty> properties = getXsetwacomProperties();

    foreach(const XsetwacomProperty& property, properties) {
        result.append(property.id());
    }

    return result;
}


const QList< XsetwacomProperty > DeviceProfileXsetwacomAdaptor::getXsetwacomProperties() const
{
    Q_D( const DeviceProfileXsetwacomAdaptor );
    assert(d->profile != NULL);

    QList<XsetwacomProperty> result;
    QList<XsetwacomProperty> properties = XsetwacomProperty::list();

    foreach(const XsetwacomProperty& property, properties) {
        if (!d->profile->getProperty(property.id()).isEmpty()) {
            result.append(property);
        }
    }

    return result;
}


const QString DeviceProfileXsetwacomAdaptor::getProperty(const Property& property) const
{
    Q_D( const DeviceProfileXsetwacomAdaptor );
    assert(d->profile != NULL);
    return d->profile->getProperty(property);
}


const QString DeviceProfileXsetwacomAdaptor::getXsetwacomProperty(const XsetwacomProperty& property) const
{
    Q_D( const DeviceProfileXsetwacomAdaptor );
    assert(d->profile != NULL);
    return d->profile->getProperty(property.id());
}


bool DeviceProfileXsetwacomAdaptor::setProperty(const Property& property, const QString& value)
{
    Q_D( DeviceProfileXsetwacomAdaptor );
    assert(d->profile != NULL);
    return d->profile->setProperty(property, value);
}


bool DeviceProfileXsetwacomAdaptor::setXsetwacomProperty(const XsetwacomProperty& property, const QString& value)
{
    Q_D( DeviceProfileXsetwacomAdaptor );
    assert(d->profile != NULL);
    return d->profile->setProperty(property.id(), value);
}
