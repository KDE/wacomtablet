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

#ifndef TABLETINFO_H
#define TABLETINFO_H

#include <QString>

#include "enum.h"
#include "property.h"

namespace Wacom {

// forward declaration
class TabletInfo;
class TabletInfoTemplateSpecializationLessFunctor;

/**
 * Do not use this typedef!
 *
 * It is a helper typedef to ease maintenance of template parameters.
 */
typedef Enum<TabletInfo, QString, TabletInfoTemplateSpecializationLessFunctor, PropertyKeyEqualsFunctor> TabletInfoTemplateSpecialization;


/**
 * Do not use this functor!
 *
 * It is required by the TabletInfo class and can only be used to sort instances
 * of type TabletInfoTemplateSpecialization.
 */
struct TabletInfoTemplateSpecializationLessFunctor
{
    bool operator()(const TabletInfoTemplateSpecialization* p1, const TabletInfoTemplateSpecialization* p2)
    {
        return (p1->key() < p2->key());
    }
};


/**
 * An enum of all supported device information properties.
 */
class TabletInfo : public TabletInfoTemplateSpecialization {

public:

    static const TabletInfo CompanyId;
    static const TabletInfo CompanyName;
    static const TabletInfo CursorName;
    static const TabletInfo EraserName;
    static const TabletInfo PadName;
    static const TabletInfo StylusName;
    static const TabletInfo TabletId;
    static const TabletInfo TabletModel;
    static const TabletInfo TabletName;
    static const TabletInfo TouchName;


private:

    TabletInfo(const QString& key) : TabletInfoTemplateSpecialization(this, key) {}

}; // CLASS

/*
 * Declare static instances-container of the TabletInfo template specialization.
 */
template<>
TabletInfoTemplateSpecialization::Container TabletInfoTemplateSpecialization::instances;

}      // NAMESPACE
#endif // HEADER PROTECTION
