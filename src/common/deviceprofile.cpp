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
    /**
     * The name of this device profile.
     */
    QString                 name;

    /**
     * Stores most of the configuration properties. In the future
     * more and more properties might get switched to real member 
     * variables but for now this is the most convenient way.
     */
    QHash<QString, QString> config;
};
}

DeviceProfile::DeviceProfile() : PropertyAdaptor(NULL), d_ptr(new DeviceProfilePrivate) { }

DeviceProfile::DeviceProfile(const QString& name)
    : PropertyAdaptor(NULL), d_ptr(new DeviceProfilePrivate)
{
    setName(name);
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
    
    d->name   = that.d_ptr->name;
    d->config = that.d_ptr->config;
    
    return *this;
}

const QString DeviceProfile::getAbsWheel2Down() const
{
    return getProperty(Property::AbsWheel2Down);
}


const QString DeviceProfile::getAbsWheel2Up() const
{
    return getProperty(Property::AbsWheel2Up);
}


const QString DeviceProfile::getAbsWheelDown() const
{
    return getProperty(Property::AbsWheelDown);
}


const QString DeviceProfile::getAbsWheelUp() const
{
    return getProperty(Property::AbsWheelUp);
}


const QString DeviceProfile::getArea() const
{
    return getProperty(Property::Area);
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


const QString DeviceProfile::getChangeArea() const
{
    return getProperty(Property::ChangeArea);
}


const QString DeviceProfile::getCursorProximity() const
{
    return getProperty(Property::CursorProximity);
}


const QString DeviceProfile::getForceProportions() const
{
    return getProperty(Property::ForceProportions);
}


const QString DeviceProfile::getGesture() const
{
    return getProperty(Property::Gesture);
}


const QString DeviceProfile::getInvertScroll() const
{
    return getProperty(Property::InvertScroll);
}


const QString DeviceProfile::getMapToOutput() const
{
    return getProperty(Property::MapToOutput);
}


const QString DeviceProfile::getMode() const
{
    return getProperty(Property::Mode);
}


const QString& DeviceProfile::getName() const
{
    Q_D( const DeviceProfile );
    return d->name;
}


const QString DeviceProfile::getPressureCurve() const
{
    return getProperty(Property::PressureCurve);
}


const QString DeviceProfile::getProperty(const Property& property) const
{
    Q_D( const DeviceProfile );
    return d->config.value(property.key());
}


const QList<Property> DeviceProfile::getProperties() const
{
    QList<Property> properties;

    foreach(const DeviceProperty& property, DeviceProperty::list()) {
        properties.push_back(property.id());
    }

    return properties;
}

const QString DeviceProfile::getRawSample() const
{
    return getProperty(Property::RawSample);
}


const QString DeviceProfile::getRelWheelDown() const
{
    return getProperty(Property::RelWheelDown);
}


const QString DeviceProfile::getRelWheelUp() const
{
    return getProperty(Property::RelWheelUp);
}


const QString DeviceProfile::getRotate() const
{
    return getProperty(Property::Rotate);
}


const QString DeviceProfile::getRotateWithScreen() const
{
    return getProperty(Property::RotateWithScreen);
}


const QString DeviceProfile::getScreenMapping() const
{
    return getProperty(Property::ScreenMapping);
}


const QString DeviceProfile::getScreenSpace() const
{
    return getProperty(Property::ScreenSpace);
}


const QString DeviceProfile::getScrollDistance() const
{
    return getProperty(Property::ScrollDistance);
}


const QString DeviceProfile::getStripLeftDown() const
{
    return getProperty(Property::StripLeftDown);
}


const QString DeviceProfile::getStripLeftUp() const
{
    return getProperty(Property::StripLeftUp);
}


const QString DeviceProfile::getStripRightDown() const
{
    return getProperty(Property::StripRightDown);
}


const QString DeviceProfile::getStripRightUp() const
{
    return getProperty(Property::StripRightUp);
}


const QString DeviceProfile::getSuppress() const
{
    return getProperty(Property::Suppress);
}


const QString DeviceProfile::getTabletArea() const
{
    return getProperty(Property::TabletArea);
}


const QString DeviceProfile::getTabletPcButton() const
{
    return getProperty(Property::TabletPcButton);
}


const QString DeviceProfile::getTapTime() const
{
    return getProperty(Property::TapTime);
}


const QString DeviceProfile::getThreshold() const
{
    return getProperty(Property::Threshold);
}


const QString DeviceProfile::getTouch() const
{
    return getProperty(Property::Touch);
}


const QString DeviceProfile::getZoomDistance() const
{
    return getProperty(Property::ZoomDistance);
}


void DeviceProfile::setAbsWheel2Down(const QString& value)
{
    setProperty(Property::AbsWheel2Down, value);
}


void DeviceProfile::setAbsWheel2Up(const QString& value)
{
    setProperty(Property::AbsWheel2Up, value);
}


void DeviceProfile::setAbsWheelDown(const QString& value)
{
    setProperty(Property::AbsWheelDown, value);
}


void DeviceProfile::setAbsWheelUp(const QString& value)
{
    setProperty(Property::AbsWheelUp, value);
}


void DeviceProfile::setArea(const QString& value)
{
    setProperty(Property::Area, value);
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


void DeviceProfile::setChangeArea(const QString& value)
{
    setProperty(Property::ChangeArea, value);
}


void DeviceProfile::setCursorProximity(const QString& value)
{
    setProperty(Property::CursorProximity, value);
}


void DeviceProfile::setForceProportions(const QString& value)
{
    setProperty(Property::ForceProportions, value);
}


void DeviceProfile::setGesture(const QString& value)
{
    setProperty(Property::Gesture, value);
}


void DeviceProfile::setInvertScroll(const QString& value)
{
    setProperty(Property::InvertScroll, value);
}


void DeviceProfile::setName ( const QString& name )
{
    Q_D( DeviceProfile );
    d->name = name;
}


void DeviceProfile::setMapToOutput(const QString& value)
{
    setProperty(Property::MapToOutput, value);
}


void DeviceProfile::setMode(const QString& value)
{
    setProperty(Property::Mode, value);
}


void DeviceProfile::setPressureCurve(const QString& value)
{
    setProperty(Property::PressureCurve, value);
}


bool DeviceProfile::setProperty(const Property& property, const QString& value)
{
    Q_D( DeviceProfile );

    if (value.isEmpty()) {
        d->config.remove(property.key());
    } else {
        d->config.insert(property.key(), value);
    }

    return true;
}


void DeviceProfile::setRawSample(const QString& value)
{
    setProperty(Property::RawSample, value);
}


void DeviceProfile::setRelWheelDown(const QString& value)
{
    setProperty(Property::RelWheelDown, value);
}


void DeviceProfile::setRelWheelUp(const QString& value)
{
    setProperty(Property::RelWheelUp, value);
}


void DeviceProfile::setRotate(const QString& value)
{
    setProperty(Property::Rotate, value);
}


void DeviceProfile::setRotateWithScreen(const QString& value)
{
    setProperty(Property::RotateWithScreen, value);
}


void DeviceProfile::setScreenMapping(const QString& value)
{
    setProperty(Property::ScreenMapping, value);
}


void DeviceProfile::setScreenSpace(const QString& value)
{
    setProperty(Property::ScreenSpace, value);
}


void DeviceProfile::setScrollDistance(const QString& value)
{
    setProperty(Property::ScrollDistance, value);
}


void DeviceProfile::setStripLeftDown(const QString& value)
{
    setProperty(Property::StripLeftDown, value);
}


void DeviceProfile::setStripLeftUp(const QString& value)
{
    setProperty(Property::StripLeftUp, value);
}


void DeviceProfile::setStripRightDown(const QString& value)
{
    setProperty(Property::StripRightDown, value);
}


void DeviceProfile::setStripRightUp(const QString& value)
{
    setProperty(Property::StripRightUp, value);
}


void DeviceProfile::setSuppress(const QString& value)
{
    setProperty(Property::Suppress, value);
}


void DeviceProfile::setTabletArea(const QString& value)
{
    setProperty(Property::TabletArea, value);
}


void DeviceProfile::setTabletPcButton(const QString& value)
{
    setProperty(Property::TabletPcButton, value);
}


void DeviceProfile::setTapTime(const QString& value)
{
    setProperty(Property::TapTime, value);
}


void DeviceProfile::setThreshold(const QString& value)
{
    setProperty(Property::Threshold, value);
}


void DeviceProfile::setTouch(const QString& value)
{
    setProperty(Property::Touch, value);
}


void DeviceProfile::setZoomDistance(const QString& value)
{
    setProperty(Property::ZoomDistance, value);
}

bool DeviceProfile::supportsProperty(const Property& property) const
{
    return (DeviceProperty::map(property) != NULL);
}

