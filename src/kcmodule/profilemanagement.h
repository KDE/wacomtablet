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

#ifndef PROFILEMANAGEMENT_H
#define PROFILEMANAGEMENT_H

#include "profilemanager.h"
#include "deviceprofile.h"

//Qt includes
#include <QtCore/QString>
#include <QtCore/QStringList>

class KConfigGroup;
class QDBusInterface;

namespace Wacom
{
/**
  * The profilemanagement is used as general tablet independent backend to store all parameters per profile
  * The KConfig approach is used to store the user data. To apply the profiles the deviceinterface has to
  * interpret the profile information into whatever the device backends expect.
  * Profiles are saved by tablet name and thus it is possible to have the same profile names for different tablets
  */
class ProfileManagement
{
    public:

        /**
         * Returns the instance of this singleton class.
         */
        static ProfileManagement& instance();

        /**
          * Creates a new profile for the currently detected device and saves it back to the KConfig file
          *
          * @param profilename name of the profile that will be created
          */
        void createNewProfile(const QString & profilename = QLatin1String( "default" ));

        /**
          * Returns the KConfigGroup with all available profiles for the current tablet device
          *
          * This is used to fill the combobox with the profile names, create/delete profiles
          * and to update the configuration if something changed.
          *
          * @return the profile group object that holds all profile information.
          */
        const QStringList availableProfiles();

        /**
          * Deletes the profile from the current connected device.
          *
          * Should no profile be left a default profile will be created and selected.
          */
        void deleteProfile();

        /**
          * Returns a device profile of the currently active tablet profile.
          *
          * Always returns a device profile of the currently connected tablet. The
          * parameter @c device can be one of stylus/pad/eraser/touch or cursor.
          *
          * @param device The device profile to get.
          * 
          * @return The device profile of the requested device.
          */
        DeviceProfile loadDeviceProfile(const DeviceType& device);

        /**
         * Saves the given profile as a device profile of the currently
         * active tablet profile.
         *
         * @param profile The profile to save.
         * @return True on success, false on error.
         */
        bool saveDeviceProfile (const DeviceProfile& profile);

        /**
          * Sets the used profile.
          *
          * If the kcmodule changes the profile the name here should change as well to obtain the
          * right config values
          *
          * @param name Name of the new profile
          */
        void setProfileName(const QString & name);

        /**
          * Returns the current used profile name.
          *
          * @return name of the profile in use
          */
        QString profileName() const;

        /** Enumeration for all available tablet pad button actions */
        enum PadButton {
            Pad_Disable,        /**< disables the button */
            Pad_Button,         /**< button action */
            Pad_Keystroke,      /**< keystroke function action */
        };

        /** Enumeration for all available tablet pen button actions (stylus/eraser) */
        enum PenButton {
            Pen_Disable,        /**< disables the button */
            Pen_LeftClick,      /**< left click action */
            Pen_MiddleClick,    /**< middle click action */
            Pen_RightClick,     /**< right click action */
            Pen_Button,         /**< button action */
            Pen_Keystroke,      /**< keystroke function action */
            Pen_ModeToggle,     /**< toggle between absolute/relative cursor */
            Pen_DisplayToggle   /**< toggle display (Twinview/single screen) */
        };

        /**
          * Transforms the retrieved pad button function from xsetwacom into an Enumeration
          *
          * @param buttonParam name of the Button to ask for
          * @return Button function as Enumeration
          */
        PadButton getPadButtonFunction(const QString & buttonParam);

        /**
          * Transforms the retrieved pen button function from xsetwacom into an Enumeration
          *
          * @param buttonParam name of the Button to ask for
          * @return Button function as Enumeration
          */
        PenButton getPenButtonFunction(const QString & buttonParam);

        /**
          * Transforms the pad button function as read from the GUI into a way the xsetwacom params understand
          *
          * @param mode Enumeration function mode
          * @param buttonParam Value of the button mode
          * @return xsetwacom compatible representation of the button config
          */
        QString transformButtonToConfig(PadButton mode, const QString & buttonParam);

        /**
          * Transforms the pad button function value as read from the KConfig file into a human readable version
          *
          * @param mode Enumeration function mode
          * @param buttonParam Value of the button mode
          * @return Human readable button function
          */
        QString transformButtonFromConfig(PadButton mode, QString & buttonParam);

        /**
          * Transforms the pen button function as read from the GUI into a way the xsetwacom params understand
          *
          * @param mode Enumeration function mode
          * @param buttonParam Value of the button mode
          * @return xsetwacom compatible representation of the button config
          */
        QString transformButtonToConfig(PenButton mode, const QString & buttonParam);

        /**
          * Transforms the pen button function value as read from the KConfig file into a human readable version
          *
          * @param mode Enumeration function mode
          * @param buttonParam Value of the button mode
          * @return Human readable button function
          */
        QString transformButtonFromConfig(PenButton mode, QString & buttonParam);

        /**
          * Reloads the profiles
          *
          * If the tablet is removed/connected/changed during runtime, this function updates its internal status
          */
        void reload();

    private:
        /**
          * Default constructor.
          * This is a singleton class, do not use this constructor, use \a instance() instead.
          */
        ProfileManagement();

        /**
         * Copy constructor which does nothing.
         */
        ProfileManagement(const ProfileManagement&);

        /**
         * Copy operator which does nothing.
         */
        ProfileManagement& operator=(const ProfileManagement&);

        QString         m_deviceName;      /**< Cached name of the device so. So we don't have to ask via Dbus every time */
        QString         m_profileName;     /**< Current selected profile. */
        ProfileManager  m_profileManager;  /**< Manages the profile configuration file */


}; // CLASS
}  // NAMESPACE
#endif // PROFILEMANAGEMENT_H
