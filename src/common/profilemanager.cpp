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

#include "debug.h"
#include "profilemanager.h"
#include "tabletprofile.h"
#include "tabletprofileconfigadaptor.h"

#include <KDE/KSharedConfig>


using namespace Wacom;

namespace Wacom {
/**
  * Private class of the ProfileManager for the d-pointer
  *
  */
class ProfileManagerPrivate {
public:
    QString             fileName;
    QString             tabletId;
    KConfigGroup        tabletGroup;
    KSharedConfig::Ptr  config;
};
}

ProfileManager::ProfileManager() : d_ptr(new ProfileManagerPrivate) {}


ProfileManager::ProfileManager( const QString& filename )
    : d_ptr(new ProfileManagerPrivate)
{
    open(filename);
}

ProfileManager::~ProfileManager()
{
    delete this->d_ptr;
}


void ProfileManager::close()
{
    Q_D( ProfileManager );

    d->tabletId.clear();
    d->tabletGroup = KConfigGroup();
    d->fileName.clear();
    d->config.clear();
}

bool ProfileManager::deleteProfile(const QString& profile)
{
    Q_D( ProfileManager );

    if (!isLoaded()) {
        return false;
    }

    KConfigGroup configGroup = KConfigGroup(&(d->tabletGroup), profile);

    if (configGroup.exists()) {
        configGroup.deleteGroup();
    }

    d->tabletGroup.sync();
    return true;
}



bool ProfileManager::hasIdentifier(const QString& identifier) const
{
    Q_D( const ProfileManager );

    if (!isOpen()) {
        return false;
    }

    return KConfigGroup(d->config, identifier).exists();
}


bool ProfileManager::hasProfile(const QString& profileName) const
{
    Q_D( const ProfileManager );

    if (!isLoaded() || profileName.isEmpty()) {
        return false;
    }

    return KConfigGroup(&(d->tabletGroup), profileName).exists();
}


bool ProfileManager::isLoaded() const
{
    Q_D( const ProfileManager );
    return (isOpen() && !d->tabletId.isEmpty());
}


bool ProfileManager::isOpen() const
{
    Q_D( const ProfileManager );
    return (!d->fileName.isEmpty() && !d->config.isNull());
}



const QStringList ProfileManager::listIdentifiers() const
{
    Q_D( const ProfileManager );

    if (!isOpen()) {
        return QStringList();
    }

    return d->config->groupList();
}


const QStringList ProfileManager::listProfiles() const
{
    Q_D( const ProfileManager );
    
    if (!isLoaded()) {
        return QStringList();
    }
    
    return d->tabletGroup.groupList();
}



bool ProfileManager::readProfiles(const QString& tabletIdentifier)
{
    Q_D( ProfileManager );

    if (!isOpen() || tabletIdentifier.isEmpty()) {
        d->tabletId = QString();
        return false;
    }

    d->config->reparseConfiguration();
    d->tabletId    = tabletIdentifier;
    d->tabletGroup = KConfigGroup( d->config, d->tabletId );

    return true;
}


const TabletProfile ProfileManager::loadProfile(const QString& profile) const
{
    Q_D( const ProfileManager );
    TabletProfile tabletProfile(profile);

    if (!isLoaded()) {
        return tabletProfile;
    }

    KConfigGroup configGroup(&(d->tabletGroup), profile);

    if (!configGroup.exists()) {
        return tabletProfile;
    }

    TabletProfileConfigAdaptor configAdaptor(tabletProfile);
    configAdaptor.loadConfig(configGroup);

    return tabletProfile;
}


void ProfileManager::open(const QString& filename)
{
    Q_D( ProfileManager );

    close();
    
    if (!filename.isEmpty()) {
        d->fileName = filename;
        d->config   = KSharedConfig::openConfig( filename, KConfig::SimpleConfig );
    }
}


void ProfileManager::reload()
{
    Q_D( ProfileManager );

    if (isOpen()) {
        d->config->reparseConfiguration();
    }
}



bool ProfileManager::saveProfile(TabletProfile& tabletProfile)
{
    Q_D( ProfileManager );
    
    QString profileName = tabletProfile.getName();

    if (!isLoaded() || profileName.isEmpty()) {
        kError() << QString::fromLatin1("Can not save profile '%1' as it either does not have a name or no configuration file was opened!").arg(profileName);
        return false;
    }

    KConfigGroup configGroup = KConfigGroup(&(d->tabletGroup), profileName);

    if (configGroup.exists()) {
        configGroup.deleteGroup();
    }

    TabletProfileConfigAdaptor configAdaptor(tabletProfile);

    if (!configAdaptor.saveConfig(configGroup)) {
        return false;
    }

    configGroup.sync();

    return true;
}
