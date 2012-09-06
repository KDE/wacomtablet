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

#ifndef DEVICETYPE_H
#define DEVICETYPE_H

#include <QString>

#include "enum.h"
#include "property.h"

namespace Wacom {

// forward declaration
class DeviceType;
class DeviceTypeTemplateSpecializationLessFunctor;

/**
 * Do not use this typedef!
 *
 * It is a helper typedef to ease maintenance of template parameters.
 */
typedef Enum<DeviceType, QString, DeviceTypeTemplateSpecializationLessFunctor, PropertyKeyEqualsFunctor> DeviceTypeTemplateSpecialization;


/**
 * Do not use this functor!
 *
 * It is required by the DeviceType class and can only be used to sort instances
 * of type DeviceTypeTemplateSpecialization.
 */
struct DeviceTypeTemplateSpecializationLessFunctor
{
    bool operator()(const DeviceTypeTemplateSpecialization* p1, const DeviceTypeTemplateSpecialization* p2)
    {
        return (p1->key() < p2->key());
    }
};


/**
 * An enum of all device types.
 */
class DeviceType : public DeviceTypeTemplateSpecialization {

public:

    static const DeviceType Cursor;
    static const DeviceType Eraser;
    static const DeviceType Pad;
    static const DeviceType Stylus;
    static const DeviceType Touch;


private:

    DeviceType(const QString& key) : DeviceTypeTemplateSpecialization(this, key) {}

}; // CLASS

/*
 * Declare static instances-container of the DeviceType template specialization.
 */
template<>
DeviceTypeTemplateSpecialization::Container DeviceTypeTemplateSpecialization::instances;

}      // NAMESPACE
#endif // HEADER PROTECTION
