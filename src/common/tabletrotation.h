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

#ifndef TABLETROTATION_H
#define TABLETROTATION_H

#include <QString>

#include "enum.h"
#include "property.h"

namespace Wacom
{

class TabletRotation;
class TabletRotationTemplateSpecializationLessFunctor;

/**
 * @brief Helper Typedef! Do not use!
 *
 * This typedef is required by the TabletRotation class. It eases maintenane of template parameters.
 */
typedef Enum<TabletRotation, QString, TabletRotationTemplateSpecializationLessFunctor, PropertyKeyEqualsFunctor> TabletRotationTemplateSpecialization;



/**
 * @brief Helper Class! Do not use!
 *
 * This functor is required by the TabletRotation class to sort its instances.
 */
struct TabletRotationTemplateSpecializationLessFunctor
{
    bool operator()(const TabletRotationTemplateSpecialization* p1, const TabletRotationTemplateSpecialization* p2)
    {
        return (p1->key() < p2->key());
    }
};


class TabletRotation : public TabletRotationTemplateSpecialization
{
public:
    static const TabletRotation NONE;
    static const TabletRotation CCW;
    static const TabletRotation HALF;
    static const TabletRotation CW;

private:

    TabletRotation (const QString& key) : TabletRotationTemplateSpecialization(this, key) {}
    
}; // CLASS

/*
 * Declare static instances-container of the template specialization.
 */
template<>
TabletRotationTemplateSpecialization::Container TabletRotationTemplateSpecialization::instances;

}  // NAMESPACE
#endif // HEADER PROTECTION
