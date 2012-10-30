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

#ifndef PROPERTY_H
#define PROPERTY_H

#include <QString>

#include "enum.h"

namespace Wacom {

// forward declaration
class  Property;
struct PropertyTemplateSpecializationLessFunctor;
struct PropertyKeyEqualsFunctor;

/**
 * Do not use this typedef!
 *
 * It is a helper typedef to ease maintenance of template parameters.
 */
typedef Enum<Property, QString, PropertyTemplateSpecializationLessFunctor, PropertyKeyEqualsFunctor> PropertyTemplateSpecialization;


/**
 * Do not use this functor!
 *
 * It is required by the Property class and can only be used to sort instances
 * of type PropertyTemplateSpecialization.
 */
struct PropertyTemplateSpecializationLessFunctor
{
    bool operator()(const PropertyTemplateSpecialization* p1, const PropertyTemplateSpecialization* p2)
    {
        return (p1->key() < p2->key());
    }
};


/**
 * A functor which is used to compare two Property keys from type QString.
 * The keys are compared case insensitive. This functor should be used by
 * all property enumerations.
 *
 * @return True if both keys are equal, else false;
 */
struct PropertyKeyEqualsFunctor
{
    bool operator()(const QString& k1, const QString& k2)
    {
        return (k1.compare(k2, Qt::CaseInsensitive) == 0);
    }
};


/**
 * An enum of all supported properties.
 *
 * Different subsystems support different sets of properties, which in
 * turn can be mapped to different key values. This class serves as a
 * global list of supported properties to allow property mapping
 * between subsystems.
 */
class Property : public PropertyTemplateSpecialization {

public:

    static const Property AbsWheel2Down;
    static const Property AbsWheel2Up;
    static const Property AbsWheelDown;
    static const Property AbsWheelUp;
    static const Property Area;
    static const Property Button1;
    static const Property Button2;
    static const Property Button3;
    static const Property Button4;
    static const Property Button5;
    static const Property Button6;
    static const Property Button7;
    static const Property Button8;
    static const Property Button9;
    static const Property Button10;

    //! @deprecated No idea what this is for. It will be removed soon.
    static const Property ChangeArea;

    static const Property CursorAccelProfile;
    static const Property CursorAccelConstantDeceleration;
    static const Property CursorAccelAdaptiveDeceleration;
    static const Property CursorAccelVelocityScaling;
    static const Property CursorProximity;
    static const Property ForceProportions;
    static const Property Gesture;
    static const Property InvertScroll;
    static const Property MapToOutput;
    static const Property MMonitor;
    static const Property Mode;
    static const Property PressureCurve;
    static const Property RawSample;
    static const Property RelWheelDown;
    static const Property RelWheelUp;
    static const Property Rotate;
    static const Property RotateWithScreen;
    static const Property ScreenMapping;
    static const Property ScreenNo;
    static const Property ScreenSpace;
    static const Property ScrollDistance;
    static const Property StripLeftDown;
    static const Property StripLeftUp;
    static const Property StripRightDown;
    static const Property StripRightUp;
    static const Property Suppress;
    static const Property TabletArea;
    static const Property TabletPcButton;
    static const Property TapTime;
    static const Property Threshold;
    static const Property Touch;
    static const Property TVResolution0;
    static const Property TVResolution1;
    static const Property TwinView;
    static const Property ZoomDistance;


private:

    Property(const QString& key) : PropertyTemplateSpecialization(this, key) {}

}; // CLASS

/*
 * Declare static instances-container of the Property template specialization.
 */
template<>
PropertyTemplateSpecialization::Container PropertyTemplateSpecialization::instances;

}      // NAMESPACE
#endif // HEADER PROTECTION
