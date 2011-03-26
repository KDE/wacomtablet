/*
 * Copyright 2010 Jörg Ehrichs <joerg.ehichs@gmx.de>
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

#ifndef TABLETDAEMON_H
#define TABLETDAEMON_H

//KDE includes
#include <KDE/KDEDModule>

//Qt includes
#include <QtCore/QVariantList>
#include <QtCore/QStringList>

/**
  * The wacom namespace holds all classes regarding the tablet daemon / kcmodule and applet.
  */
namespace Wacom
{
class TabletDaemonPrivate;

/**
  * This module manages the tablet device and exports a convenient DBus API
  * for tablet configuration and management.
  *
  * Its task is to monitor Hotpluging devices via solid and the detection of the tablet connected.
  * Once a tablet is recognised a default profile will be applied via the wacom::DeviceHandler.
  *
  * Profiles can be changed with the kcmodule.
  * In addition a plasma applet allows easy switching of different profiles.
  *
  * It registers the service @c "org.kde.Wacom" and exports the following objects on this service:
  *
  * @li @c /Tablet - this object. Allows to check if a tablet is available and applies the profile
  * @li @c /Device - tablet information. Basic information about the detected tablet
  */
class KDE_EXPORT TabletDaemon : public KDEDModule
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.Wacom")
public:
    /**
      * Creates a new daemon module
      *
      * @param parent the parent object
      * @param args ignored, required by KPlugin signature
      */
    explicit TabletDaemon(QObject *parent = 0, const QVariantList &args = QVariantList());

    /**
      * Destroys this module
      */
    virtual ~TabletDaemon();

public Q_SLOTS:
    /**
      * Checks if a tablet is detected and available for further usage
      *
      * Simply redirects to the wacom::DeviceHandler.
      *
      * This function is exported on DBus.
      *
      * @sa wacom::DeviceHandler::detectTablet()
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
      * This function is exported on DBus.
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
      * This function is exported on DBus.
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

private Q_SLOTS:
    /**
      * Called when solid detects a new device.
      *
      * Checks if the device is a tablet device.
      * Calls reloadDeviceInformation() from wacom::DeviceHandler.
      * If a tablet could be detected a notification is send and the default profile will be applied
      *
      */
    void deviceAdded(int deviceid);

    /**
      * Called when Solid detects a device was removed.
      *
      * Checks if the removed device uid is the same as the connected tablet udi.
      * Sends a notification about the removal and clears the wacom::DeviceHandler cache.
      *
      */
    void deviceRemoved(int deviceid);
    
    /**
     * Rotates the tablet when the screen is rotated
     * 
     * @param screenRotation Integer values for the screen rotations
     */
    void screenRotated(int screenRotation);

    /**
      * Notify about an error.
      *
      * @param message a human readable error message
      */
    void notifyError(const QString &message) const;

private:
    Q_DECLARE_PRIVATE(TabletDaemon)

    /**
      * Used on startup to detect a connected wacom tablet.
      * This will be done by the eventhandler later on
      *
      * @return int x11 device id of the wirst detected wacom tablet
      */
    int findTabletDevice();

    TabletDaemonPrivate *const d_ptr; /**< d-pointer for this class */
};

}

#endif
