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

#include "tabletrotation.h"

#include <QtCore/QObject>

namespace Wacom
{
class DeviceHandler;
class TabletHandlerPrivate;

class TabletHandler : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.Wacom")

public:
    TabletHandler(DeviceHandler& deviceHandler);

public Q_SLOTS:
    /**
      * Toggles the stylus/eraser to absolute/relative mode
      */
    void actionTogglePenMode();

    /**
      * Toggles the touch tool on/off.
      */
    void actionToggleTouch();

    /**
      * Handles the connection of a new tablet device.
      *
      * @param deviceid The device id as reported by X11.
      */
    void onDeviceAdded(int deviceid);

    /**
      * Handles the removal of a tablet device.
      *
      * @param deviceid The device id as reported by X11.
      */
    void onDeviceRemoved(int deviceid);

    /**
     * Handles rotating the tablet.
     *
     * @param screenRotation Integer values for the screen rotations
     */
    void onScreenRotated(TabletRotation screenRotation);

    /**
      * Checks if a tablet is detected and available for further usage
      *
      * @return @c true if tablet is available, @c false otherwise
      */
    Q_SCRIPTABLE bool tabletAvailable() const;

    /**
      * Applies a profile to the tablet device
      *
      * The profile must exist in the tabletprofilerc file and thus created by the kcmodule.
      * Otherwise a notification error is send and shown.
      *
      * @param profile name of the profile as specified in the tabletprofilesrc file.
      */
    Q_SCRIPTABLE void setProfile(const QString& profile);

    /**
      * Returns the current active profile for this tablet.
      *
      * This is not necessary the real configuration in case some other program changed the tablet
      * behaviour. But this is the name of the profile that was used last.
      * Can be used to show in the applet as information or as beginning selection in the kcmodule.
      *
      * @return name of the last used profile
      */
    Q_SCRIPTABLE QString profile() const;

    /**
      * Returns a list of all available profiles
      *
      * This way around the plasma applet does not check the local KConfig file itself
      * and can be used as a remote applet.
      *
      * @return the list of all available profiles
      */
    Q_SCRIPTABLE QStringList profileList() const;


Q_SIGNALS:

    /**
     * Emitted if the user should be notified.
     */
    void notify (const QString& eventId, const QString& title, const QString& message);

    /**
      * Emitted if a new tablet is connected and detected
      *
      * This signal is send via DBus to inform other about the recently added device
      *
      * @see deviceAdded(const QString& udi)
      */
    Q_SCRIPTABLE void tabletAdded();

    /**
      * Emitted if a known tablet is removed
      *
      * This signal is send via DBus to inform other about the recently removed device
      *
      * @see deviceRemoved(const QString& udi)
      */
    Q_SCRIPTABLE void tabletRemoved();

    /**
      * Emitted when the profile of the device is changed
      *
      * This signal is send via DBus to inform other about the change
      *
      * @param profile name of the current profile
      */
    Q_SCRIPTABLE void profileChanged(const QString& profile);

private:
    Q_DECLARE_PRIVATE(TabletHandler)
    TabletHandlerPrivate *const d_ptr; /**< d-pointer for this class */
    
}; // CLASS
}  // NAMESPACE
#endif // HEADER PROTECTION
