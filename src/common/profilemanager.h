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

#ifndef PROFILEMANAGER_H
#define PROFILEMANAGER_H

#include <KDE/KConfigGroup>

#include <QtCore/QString>

#include "tabletprofile.h"

namespace Wacom {

class ProfileManagerPrivate;

/**
  * A tablet profile manager.
  *
  * All profiles of a tablet are stored under one unique identifier for that tablet.
  * Each tablet profile can have multiple device profiles for the different components
  * of its tablet (stylus/eraser/pad/touch/...). The profile manager works by loading
  * all tablet profiles of a tablet device. Only tablet profiles of one tablet device
  * can be loaded at a time.
  *
  * Profile hierarchy in configuration file:
  *
  * + TABLET IDENTIFIER -> TABLET PROFILE -> DEVICE PROFILE
  *                                       -> DEVICE PROFILE
  *                                       -> ...
  *                     -> TABLET PROFILE -> DEVICE PROFILE
  *                                       -> ...
  *                     -> ...
  * + ...
  */
class ProfileManager {
public:
    /**
      * Default constructor
      */
    ProfileManager();

     /**
      * @param filename The filename to load the configuration from. It can either be
      *                 a filename only or a full path. When using a filename only, the
      *                 full path is determined by KSharedConfig.
      */
    explicit ProfileManager( const QString& filename );


    /**
      * Default destructor
      */
    ~ProfileManager();

    /**
     * Close the current configuration file.
     */
    void close();

    /**
     * Deletes the given profile from the currently loaded tablet.
     */
    bool deleteProfile(const QString& profile);

    /**
     * Checks if the given tablet device identifier exists in the configuration file.
     * If it exists, profiles are registered for this tablet identifier and they can
     * be loaded using ProfileManager::loadProfiles(const QString&).
     *
     * @return True if the identifier has profiles, else false.
     */
    bool hasIdentifier(const QString& identifier) const;

    /**
     * Checks if a tablet profile with the given name is registered for the current
     * tablet identifier. This method requires that the profiles for a tablet identifier
     * have been loaded first.
     *
     * @param profile The tablet profile name to check for.
     *
     * @see ProfileManager::loadProfiles(const QString&)
     */
    bool hasProfile(const QString& profileName) const;


    /**
     * Checks if a configuration file has been opened.
     */
    bool isOpen() const;

    /**
     * Checks if profiles have been loaded.
     */
    bool isLoaded() const;

    /**
     * Lists all tablet identifier which can be loaded using ProfileManager::readProfiles(const QString&)
     *
     * @return List of tablet device identifiers.
     */
    const QStringList listIdentifiers() const;

    /**
     * Lists all profiles that have been loaded.
     *
     * @see ProfileManager::loadProfiles(const QString&)
     */
    const QStringList listProfiles() const;


    /**
     * Gets the given profile for the currently loaded tablet identifier. If the tablet
     * profile does not exist yet, an empty one will be created.
     *
     * @param profile The profile name of the tablet profile to get.
     */
    const TabletProfile loadProfile(const QString& profile) const;


    /**
     * Loads all tablet profiles for a given tablet device identifier from the
     * configuration file. If the tablet device identifier does not exist in
     * the configuration file, it will be created.
     *
     * @param tablet The tablet identifier to load the profiles for.
     */
    bool readProfiles(const QString& tabletIdentifier);


    /**
     * Opens the given configuration file.
     */
    void open (const QString& filename);


    /**
     * Reloads the current configuration file.
     */
    void reload();

    /**
     * Saves the given tablet profile for the currently loaded tablet identifier.
     * If no tablet identifier is currently loaded or if the tablet profile does
     * not have a name set, nothing is done. If the  profile does already exist
     * in the configuration file, it will be overwritten.
     *
     * @param tabletProfile The tablet profile to save.
     */
    bool saveProfile(Wacom::TabletProfile& tabletProfile);


private:
    Q_DECLARE_PRIVATE( ProfileManager )

    ProfileManagerPrivate *const d_ptr; /**< d-pointer for this class */

};     // CLASS
}      // NAMESPACE
#endif // HEADER PROTECTION
