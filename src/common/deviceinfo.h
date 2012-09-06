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

#ifndef DEVICEINFO_H
#define DEVICEINFO_H

#include <QString>

#include "enum.h"
#include "property.h"

namespace Wacom {

// forward declaration
class DeviceInfo;
class DeviceInfoTemplateSpecializationLessFunctor;

/**
 * Do not use this typedef!
 *
 * It is a helper typedef to ease maintenance of template parameters.
 */
typedef Enum<DeviceInfo, QString, DeviceInfoTemplateSpecializationLessFunctor, PropertyKeyEqualsFunctor> DeviceInfoTemplateSpecialization;


/**
 * Do not use this functor!
 *
 * It is required by the DeviceInfo class and can only be used to sort instances
 * of type DeviceInfoTemplateSpecialization.
 */
struct DeviceInfoTemplateSpecializationLessFunctor
{
    bool operator()(const DeviceInfoTemplateSpecialization* p1, const DeviceInfoTemplateSpecialization* p2)
    {
        return (p1->key() < p2->key());
    }
};


/**
 * An enum of all supported device information properties.
 */
class DeviceInfo : public DeviceInfoTemplateSpecialization {

public:

    static const DeviceInfo CompanyId;
    static const DeviceInfo CompanyName;
    static const DeviceInfo CursorName;
    static const DeviceInfo EraserName;
    static const DeviceInfo PadName;
    static const DeviceInfo StylusName;
    static const DeviceInfo TabletId;
    static const DeviceInfo TabletModel;
    static const DeviceInfo TabletName;
    static const DeviceInfo TouchName;


private:

    DeviceInfo(const QString& key) : DeviceInfoTemplateSpecialization(this, key) {}

}; // CLASS

/*
 * Declare static instances-container of the DeviceInfo template specialization.
 */
template<>
DeviceInfoTemplateSpecialization::Container DeviceInfoTemplateSpecialization::instances;

}      // NAMESPACE
#endif // HEADER PROTECTION
