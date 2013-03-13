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

#include "tabletbackend.h"
#include "debug.h"

namespace Wacom
{
    class TabletBackendPrivate
    {
        public:
            TabletBackend::DeviceMap deviceAdaptors;
            TabletInformation        tabletInformation;
    };
}

using namespace Wacom;

TabletBackend::TabletBackend(const Wacom::TabletInformation& tabletInformation) : d_ptr(new TabletBackendPrivate)
{
    Q_D(TabletBackend);
    d->tabletInformation = tabletInformation;
}

TabletBackend::~TabletBackend()
{
    // delete all property adaptors
    DeviceMap::iterator deviceIter;

    for (deviceIter = d_ptr->deviceAdaptors.begin() ; deviceIter != d_ptr->deviceAdaptors.end() ; ++deviceIter) {
        AdaptorList::iterator adaptorIter = deviceIter.value().begin();

        while (adaptorIter != deviceIter.value().end()) {
            delete (*adaptorIter);
            adaptorIter = deviceIter.value().erase(adaptorIter);
        }
    }

    // delete private class
    delete d_ptr;
}



void TabletBackend::addAdaptor(const DeviceType& deviceType, PropertyAdaptor* adaptor)
{
    Q_D(TabletBackend);

    DeviceMap::iterator it = d->deviceAdaptors.find(deviceType);

    if (it == d->deviceAdaptors.end()) {
        // device does not exist yet
        d->deviceAdaptors.insert(deviceType, AdaptorList());

        it = d->deviceAdaptors.find(deviceType);
        assert(it != d->deviceAdaptors.end());
    }

    it.value().append(adaptor);
}



const TabletInformation& TabletBackend::getInformation() const
{
    Q_D(const TabletBackend);
    return d->tabletInformation;
}



const QString TabletBackend::getProperty(const DeviceType& type, const Property& property) const
{
    Q_D(const TabletBackend);

    if (!d->deviceAdaptors.contains(type)) {
        kError() << QString::fromLatin1("Could not get property '%1' from unsupported device type '%2'!").arg(property.key()).arg(type.key());
        return QString();
    }

    DeviceMap::const_iterator adaptors = d->deviceAdaptors.constFind(type);
    assert(adaptors != d->deviceAdaptors.constEnd());

    foreach(const PropertyAdaptor* adaptor, adaptors.value()) {
        if (adaptor->supportsProperty(property)) {
            return adaptor->getProperty(property);
        }
    }

    return QString();
}



void TabletBackend::setProfile(const TabletProfile& profile)
{
    Q_D(TabletBackend);

    foreach(const DeviceType& deviceType, DeviceType::list()) {
        if (d->tabletInformation.hasDevice(deviceType)) {
            if (profile.hasDevice(deviceType)) {
                kDebug() << QString::fromLatin1("Setting profile '%1' on tablet '%2', device '%3'").arg(profile.getName()).arg(d->tabletInformation.get(TabletInfo::TabletName)).arg(deviceType.key());
                DeviceProfile deviceProfile = profile.getDevice(deviceType);
                setProfile(deviceType, deviceProfile);
            } else {
                kDebug() << QString::fromLatin1("Skipping '%1' settings as the current profile does not contain any settings for this device...").arg(deviceType.key());
            }
        } else {
            kDebug() << QString::fromLatin1("Skipping '%1' settings as the device does not support it...").arg(deviceType.key());
        }
    }
}



void TabletBackend::setProfile(const DeviceType& deviceType, const DeviceProfile& profile)
{
    Q_D(TabletBackend);

    if (!d->deviceAdaptors.contains(deviceType)) {
        kError() << QString::fromLatin1("Could not set profile on unsupported device type '%1'!").arg(deviceType.key());
        return;
    }

    DeviceMap::iterator adaptors = d->deviceAdaptors.find(deviceType);
    assert(adaptors != d->deviceAdaptors.end());

    QString value;

    // set properties on all adaptors
    foreach(PropertyAdaptor* adaptor, adaptors.value()) {

        // ask adaptor which properties it supports
        // this will also ensure that the properties are set in the correct order
        foreach(const Property& property, adaptor->getProperties()) {

            // set property value if it is supported by the profile and not empty
            if (profile.supportsProperty(property)) {
                value = profile.getProperty(property);

                if (!value.isEmpty()) {
                    adaptor->setProperty(property, value);
                }
            }
        }
    }
}



bool TabletBackend::setProperty(const DeviceType& type, const Property& property, const QString& value)
{
    Q_D(TabletBackend);

    if (!d->deviceAdaptors.contains(type)) {
        kError() << QString::fromLatin1("Could not set property '%1' to '%2' on unsupported device type '%3'!").arg(property.key()).arg(value).arg(type.key());
        return false;
    }

    bool                returnValue = false;
    DeviceMap::iterator adaptors    = d->deviceAdaptors.find(type);
    assert(adaptors != d->deviceAdaptors.end());

    foreach (PropertyAdaptor* adaptor, adaptors.value()) {
        if (adaptor->supportsProperty(property)) {
            if (adaptor->setProperty(property, value)) {
                returnValue = true;
            }
        }
    }

    return returnValue;
}
