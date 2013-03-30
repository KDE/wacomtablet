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

#ifndef TABLETHANDLER_H
#define TABLETHANDLER_H

#include "property.h"
#include "tablethandlerinterface.h"
#include "tabletinformation.h"
#include "screenrotation.h"

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>

namespace Wacom
{
class ScreenSpace;
class TabletProfile;
class TabletHandlerPrivate;

class TabletHandler : public TabletHandlerInterface
{
    Q_OBJECT

public:

    TabletHandler();

    /**
     * A constructor used for unit testing.
     *
     * @param profileFile The full path to the profiles configuration file.
     * @param configFile  The full path to the main configuration file.
     */
    TabletHandler(const QString& profileFile, const QString configFile);

    virtual ~TabletHandler();

    /**
      * returns the current value for a specific tablet setting
      * This is forwarded to the right backend specified by m_curDevice
      *
      * @param device name of the tablet device we set. Internal name of the pad/stylus/eraser/cursor
      * @param property the property we are looking for
      *
      * @return the value as string
      */
    QString getProperty(const DeviceType& deviceType, const Property& property) const;



    /**
      * @brief Reads a list of all available profiles from the profile manager.
      *
      * @return the list of all available profiles
      */
    QStringList listProfiles();


    /**
      * @brief Applies a profile to the tablet device
      *
      * The profile must be known to the profile manager, otherwise a
      * notification error is displayed.
      *
      * @param profile The name of the profile to apply.
      */
    void setProfile(const QString& profile);


    /**
      * Sets the configuration of @p param from @p device with @p value
      * This is forwarded to the right backend specified by m_curDevice
      *
      * @param device   The name of the device to set the property on.
      * @param property The property to set.
      * @param value    New value of the parameter
      */
    void setProperty(const DeviceType& deviceType, const Property & property, const QString& value);




public Q_SLOTS:
    /**
      * @brief Handles the connection of a new tablet device.
      *
      * This slot has to be connected to the X device event notifier and
      * executed when a new tablet device is plugged in.
      *
      * @param deviceid The device id as reported by X11.
      */
    void onTabletAdded(const TabletInformation& info);

    /**
      * @brief Handles the removal of a tablet device.
      *
      * This slot has to be connected to the X device event notifier and
      * executed when a tablet is disconnected from the system.
      *
      * @param deviceid The device id as reported by X11.
      */
    void onTabletRemoved(const TabletInformation& info);

    /**
     * @brief Handles rotating the tablet.
     *
     * This slot has to be connected to the X event notifier and executed
     * when the screen is rotated.
     *
     * @param screenRotation The screen rotation.
     */
    void onScreenRotated(const ScreenRotation& screenRotation);

    /**
      * Toggles the stylus/eraser to absolute/relative mode
      */
    void onTogglePenMode();

    /**
      * Toggles the touch tool on/off
      */
    void onToggleTouch();

    /**
     * @brief Toggles to which screen the stylus/eraser/touch will be mapped
     *
     * the order is
     * @li full
     * @li screen1
     * @li screen2
     */
    void onToggleScreenMapping();


    /**
     * @brief Maps stylus/eraser/touch to the full available screen space
     *
     * in xsetwacom file
     *  @li copies AreaMapFull to Area
     *  @li sets ScreenSpace=full
     */
    void onMapToFullScreen();


    /**
     * @brief Maps stylus/eraser/touch to the second screen
     *
     * in xsetwacom file
     *  @li copies AreaMap0 to Area in
     *  @li sets MapToOutput=0
     *  @li sets ScreenSpace=map0
     */
    void onMapToScreen1();

    /**
     * @brief Maps stylus/eraser/touch to the second screen
     *
     * in xsetwacom file
     *  @li copies AreaMap1 to Area in
     *  @li sets MapToOutput=1
     *  @li sets ScreenSpace=map1
     */
    void onMapToScreen2();


Q_SIGNALS:
    /**
     * Emitted if the user should be notified.
     */
    void notify (const QString& eventId, const QString& title, const QString& message);


    /**
      * Emitted when the profile of the current tablet is changed.
      *
      * @param profile The name of the new active profile.
      */
    void profileChanged(const QString& profile);


    /**
      * Emitted when a new tablet is connected or if the currently active tablet changes.
      */
    void tabletAdded(const TabletInformation& info);


    /**
      * Emitted when the currently active tablet is removed.
      */
    void tabletRemoved();


private:

    /**
     * Auto rotates the tablet if auto-rotation is enabled. If auto-rotation
     * is disabled, the tablet's rotation settings will be left untouched.
     *
     * @param screenRotation The current screen rotation.
     * @param tabletProfile  The tablet profile to read the rotation settings from.
     */
    void autoRotateTablet( const ScreenRotation& screenRotation, const TabletProfile& tabletProfile );

    /**
     * Checks if the current tablet supports the given device type.
     *
     * @param type The device type to check for.
     *
     * @return True if the tablet supports the given device, else false.
     */
    bool hasDevice( const DeviceType& type) const;


    /**
     * Checks if there currently is a tablet available.
     *
     * @return True if a tablet is available, else false.
     */
    bool hasTablet() const;


    /**
     * Maps a tablet device to a screen and updated its profile.
     * However the profile will not be saved, only updated with
     * the new settings.
     *
     * @param device The device to map.
     * @param screepSpace The screen space to map the device to.
     * @param trackingMode The tracking mode to apply.
     * @param tabletProfile The profile to update.
     */
    void mapDeviceToOutput(const Wacom::DeviceType& device, const ScreenSpace& screenSpace, const QString& trackingMode, TabletProfile& tabletProfile);

    /**
     * Maps the stylus, eraser and touch device to the given output and
     * updates the profiles accordingly. If no mapping is configured the
     * device is mapped to the full desktop.
     *
     * @param output A string returned by ScreenSpace::toString()
     */
    void mapPenToScreenSpace(const ScreenSpace& screenSpace, const QString& trackingMode = QLatin1String("absolute"));


    /**
     * Maps stylus/eraser/touch to their current screen space.
     * The tablet profile will be saved after the operation is complete.
     *
     * @param tabletProfile The tablet profile to use.
     */
    void mapTabletToCurrentScreenSpace(TabletProfile& tabletProfile);


    /**
      * resets all device information
      */
    void clearTabletInformation();


    Q_DECLARE_PRIVATE(TabletHandler)
    TabletHandlerPrivate *const d_ptr; /**< d-pointer for this class */

}; // CLASS
}  // NAMESPACE
#endif // HEADER PROTECTION
