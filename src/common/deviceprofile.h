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

#ifndef DEVICEPROFILE_H
#define DEVICEPROFILE_H

#include <QtCore/QString>
#include <QList>

#include "property.h"
#include "propertyadaptor.h"

namespace Wacom {

class DeviceProfilePrivate;

/**
  * This class implements the profile of a single device (stylus/eraser/cursor/pad/touch)
  */
class DeviceProfile : public PropertyAdaptor {
public:
    /**
      * Default constructor
      */
    DeviceProfile();

    /**
     * @param name The name of the device profile.
     */
    explicit DeviceProfile( const QString& name );

    /**
     * Copy constructor.
     */
    DeviceProfile( const DeviceProfile& profile );

    /**
      * Default destructor
      */
    ~DeviceProfile();

    /**
     * Copy operator.
     * 
     * @param that The instance to copy.
     * 
     * @return A reference to this instance.
     */
    DeviceProfile& operator=(const DeviceProfile& that);

    /**
     * @return X11 event to which absolute wheel down should be mapped.
     */
    const QString getAbsWheel2Down() const;

    /**
     * @return X11 event to which absolute wheel up should be mapped.
     */
    const QString getAbsWheel2Up() const;

    /**
     * @return X11 event to which absolute wheel down should be mapped.
     */
    const QString getAbsWheelDown() const;

    /**
     * @return X11 event to which absolute wheel up should be mapped.
     */
    const QString getAbsWheelUp() const;

    /**
     * @return Valid tablet area in device coordinates.
     */
    const QString getArea() const;

    /**
     * @return X11 event to which the given button should be mapped.
     */
    const QString getButton(int number) const;

    /**
     * @return 
     */
    const QString getChangeArea() const;

    /**
     * @return The cursor distance for proximity-out in distance from the tablet.
     */
    const QString getCursorProximity() const;

    /**
     * @return 
     */
    const QString getForceProportions() const;

    /**
     * @return The current state of multi-touch gesture events.
     */
    const QString getGesture() const;

    /**
     * @return The current state of the invert scroll flag.
     */
    const QString getInvertScroll() const;

    /**
     * @return The output to map the device to.
     */
    const QString getMapToOutput() const;

    /**
     * @return The current cursor movement mode.
     */
    const QString getMode() const;
    
    /**
     * Gets the name of this device profile. 
     * This is not the full profile name but only the name of this device's profile.
     * 
     * @return The name of this profile.
     */
    const QString& getName() const;

    /**
     * @return Bezier curve for pressure.
     */
    const QString getPressureCurve() const;

    /**
     * @return The value of the given property or an empty string.
     */
    const QString getProperty(const Property& key) const;


    /**
     * @return A list of properties supported by this class.
     */
    const QList<Property> getProperties() const;
    
    /**
     * @return Number of raw data used to filter the points.
     */
    const QString getRawSample() const;

    /**
     * @return X11 event to which relative wheel down should be mapped.
     */
    const QString getRelWheelDown() const;

    /**
     * @return X11 event to which relative wheel up should be mapped. 
     */
    const QString getRelWheelUp() const;

    /**
     * @return The rotation of the tablet
     */
    const QString getRotate() const;

    /**
     * @return The current state of the rotate with screen flag.
     */
    const QString getRotateWithScreen() const;

    /**
     * @return The screen mapping.
     */
    const QString getScreenMapping() const;

    /**
     * @return The dimensions of the screen space.
     */
    const QString getScreenSpace() const;

    /**
     * @return The scroll distance.
     */
    const QString getScrollDistance() const;

    /**
     * @return X11 event to which left strip down should be mapped.
     */
    const QString getStripLeftDown() const;

    /**
     * @return X11 event to which left strip up should be mapped.
     */
    const QString getStripLeftUp() const;

    /**
     * @return X11 event to which right strip down should be mapped.
     */
    const QString getStripRightDown() const;

    /**
     * @return X11 event to which right strip up should be mapped.
     */
    const QString getStripRightUp() const;

    /**
     * @return Number of points trimmed from input.
     */
    const QString getSuppress() const;

    /**
     * @return The tablet area.
     */
    const QString getTabletArea() const;

    /**
     * @return State of the tablet pc button.
     */
    const QString getTabletPcButton() const;

    /**
     * @return Minimum time between taps for a right click.
     */
    const QString getTapTime() const;

    /**
     * @return The tip/eraser pressure threshold.
     */
    const QString getThreshold() const;

    /**
     * @return The touch mode state.
     */
    const QString getTouch() const;

    /**
     * @return
     */
    const QString getZoomDistance() const;
    
    void setAbsWheel2Down(const QString& value);
    
    void setAbsWheel2Up(const QString& value);
    
    void setAbsWheelDown(const QString& value);
    
    void setAbsWheelUp(const QString& value);
    
    void setArea(const QString& value);
    
    bool setButton(int number, const QString& shortcut);

    void setChangeArea(const QString& value);
    
    void setCursorProximity(const QString& value);
    
    void setForceProportions(const QString& value);
    
    void setGesture(const QString& value);
    
    void setInvertScroll(const QString& value);
    
    void setMapToOutput(const QString& value);
    
    void setMode(const QString& value);
    
    /**
     * Sets the name of this profile.
     * 
     * @param name The new name of this profile.
     */
    void setName(const QString& name);

    void setPressureCurve(const QString& value);
    
    /**
     * Sets a property.
     *
     * @param key   The property to set.
     * @param value The property's value.
     */
    bool setProperty(const Property& key, const QString& value);

    void setRawSample(const QString& value);
    
    void setRelWheelDown(const QString& value);
    
    void setRelWheelUp(const QString& value);
    
    void setRotate(const QString& value);
    
    void setRotateWithScreen(const QString& value);
    
    void setScreenMapping(const QString& value);
    
    void setScreenSpace(const QString& value);
    
    void setScrollDistance(const QString& value);
    
    void setStripLeftDown(const QString& value);
    
    void setStripLeftUp(const QString& value);
    
    void setStripRightDown(const QString& value);
    
    void setStripRightUp(const QString& value);
    
    void setSuppress(const QString& value);
    
    void setTabletArea(const QString& value);
    
    void setTabletPcButton(const QString& value);
    
    void setTapTime(const QString& value);
    
    void setThreshold(const QString& value);
    
    void setTouch(const QString& value);
    
    void setZoomDistance(const QString& value);

    bool supportsProperty(const Property& property) const;
    

private:
    Q_DECLARE_PRIVATE( DeviceProfile )

    DeviceProfilePrivate *const d_ptr; /**< d-pointer for this class */

};

}      // NAMESPACE
#endif // HEADER PROTECTION
