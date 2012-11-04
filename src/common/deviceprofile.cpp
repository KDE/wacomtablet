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

#include <KDE/KDebug>

#include <QtCore/QHash>

#include "property.h"
#include "deviceproperty.h"
#include "deviceprofile.h"

using namespace Wacom;

namespace Wacom {
/**
  * Private class of the DeviceProfile for the d-pointer
  *
  */
class DeviceProfilePrivate {
public:
    QString                 deviceType;

    /**
     * Stores most of the configuration properties. In the future
     * more and more properties might get switched to real member 
     * variables but for now this is the most convenient way.
     */
    QHash<QString, QString> config;
};
}

DeviceProfile::DeviceProfile() : PropertyAdaptor(NULL), d_ptr(new DeviceProfilePrivate) { }

DeviceProfile::DeviceProfile(const DeviceType& type)
    : PropertyAdaptor(NULL), d_ptr(new DeviceProfilePrivate)
{
    setDeviceType(type);
}

DeviceProfile::DeviceProfile(const DeviceProfile& profile)
    : PropertyAdaptor(NULL), d_ptr(new DeviceProfilePrivate)
{
    operator=(profile);
}

DeviceProfile::~DeviceProfile()
{
    delete this->d_ptr;
}



DeviceProfile& DeviceProfile::operator= ( const DeviceProfile& that )
{
    Q_D( DeviceProfile );

    d->deviceType = that.d_ptr->deviceType;
    d->config = that.d_ptr->config;

    return *this;
}



const QString DeviceProfile::getButton(int number) const
{
    switch(number) {
        case 1:
            return getProperty(Property::Button1);
        case 2:
            return getProperty(Property::Button2);
        case 3:
            return getProperty(Property::Button3);
        case 4:
            return getProperty(Property::Button4);
        case 5:
            return getProperty(Property::Button5);
        case 6:
            return getProperty(Property::Button6);
        case 7:
            return getProperty(Property::Button7);
        case 8:
            return getProperty(Property::Button8);
        case 9:
            return getProperty(Property::Button9);
        case 10:
            return getProperty(Property::Button10);
        default:
            kError() << QString::fromLatin1("Unsupported button number '%1'!").arg(number);
    }

    return QString();
}



const QString DeviceProfile::getDeviceType() const
{
    return getName();
}



const QString& DeviceProfile::getName() const
{
    Q_D( const DeviceProfile );
    return d->deviceType;
}



const QString DeviceProfile::getProperty(const Property& property) const
{
    Q_D( const DeviceProfile );
    return d->config.value(property.key());
}



const QList<Property> DeviceProfile::getProperties() const
{
    return DeviceProperty::ids();
}


bool DeviceProfile::setButton(int number, const QString& shortcut)
{
    switch(number) {
        case 1:
            setProperty(Property::Button1, shortcut);
            break;
        case 2:
            setProperty(Property::Button2, shortcut);
            break;
        case 3:
            setProperty(Property::Button3, shortcut);
            break;
        case 4:
            setProperty(Property::Button4, shortcut);
            break;
        case 5:
            setProperty(Property::Button5, shortcut);
            break;
        case 6:
            setProperty(Property::Button6, shortcut);
            break;
        case 7:
            setProperty(Property::Button7, shortcut);
            break;
        case 8:
            setProperty(Property::Button8, shortcut);
            break;
        case 9:
            setProperty(Property::Button9, shortcut);
            break;
        case 10:
            setProperty(Property::Button10, shortcut);
            break;
        default:
            kError() << QString::fromLatin1("Unsupported button number '%1'!").arg(number);
            return false;
    }

    return true;
}



void DeviceProfile::setDeviceType(const DeviceType& type)
{
    Q_D( DeviceProfile );
    d->deviceType = type.key();
}



bool DeviceProfile::setProperty(const Property& property, const QString& value)
{
    Q_D( DeviceProfile );

    if (!supportsProperty(property)) {
        return false;
    }

    if (value.isEmpty()) {
        d->config.remove(property.key());
    } else {
        d->config.insert(property.key(), value);
    }

    return true;
}



bool DeviceProfile::supportsProperty(const Property& property) const
{
    return (DeviceProperty::map(property) != NULL);
}

