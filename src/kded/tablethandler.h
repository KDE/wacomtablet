/*
 * Copyright 2009, 2010 Jörg Ehrichs <joerg.ehichs@gmx.de>
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
#include "tabletrotation.h"

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>

namespace Wacom
{
class TabletProfile;
class TabletHandlerPrivate;

class TabletHandler : public TabletHandlerInterface
{
    Q_OBJECT

public:

    TabletHandler();
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
    QString getProperty(const QString& device, const Property& property) const;



    /**
      * @brief Reads a list of all available profiles from the profile manager.
      *
      * @return the list of all available profiles
      */
    QStringList listProfiles() const;


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
    void setProperty(const QString& device, const Property & property, const QString& value);




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
    void onScreenRotated(const TabletRotation& screenRotation);

    /**
      * Toggles the stylus/eraser to absolute/relative mode
      */
    void onTogglePenMode();

    /**
      * Toggles the touch tool on/off
      */
    void onToggleTouch();


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
      * resets all device information
      */
    void clearTabletInformation();


    /**
     * @deprecated Do not use, this is a temporary method to ease transission.
     */
    DeviceType getDeviceTypeByName(const QString& deviceName) const;


    void toggleMode(const DeviceType& type);

    
    Q_DECLARE_PRIVATE(TabletHandler)
    TabletHandlerPrivate *const d_ptr; /**< d-pointer for this class */

}; // CLASS
}  // NAMESPACE
#endif // HEADER PROTECTION
