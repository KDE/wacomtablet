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

#include "debug.h"
#include "mainconfig.h"

#include <KDE/KSharedConfig>
#include <KDE/KConfigGroup>

using namespace Wacom;

namespace Wacom
{
class MainConfigPrivate
{
    public:
        KSharedConfig::Ptr config;
        KConfigGroup       general;
}; // CLASS 
}  // NAMESPACE


MainConfig::MainConfig() : d_ptr( new MainConfigPrivate )
{
    open(QLatin1String("wacomtablet-kderc"));
}


MainConfig::MainConfig(const QString& fileName) : d_ptr( new MainConfigPrivate )
{
    open(fileName);
}



MainConfig::~MainConfig()
{
    Q_D( MainConfig );
    if (!d->config.isNull()) {
        d->config->sync();
    }

    delete this->d_ptr;
}


void MainConfig::open(const QString& fileName)
{
    Q_D( MainConfig );
    d->config  = KSharedConfig::openConfig( fileName );
    d->general = KConfigGroup(d->config, "General");
}


QString MainConfig::getLastProfile()
{
    Q_D( MainConfig );
    QString profile;

    if (!d->config.isNull()) {
        d->config->reparseConfiguration();
        profile = d->general.readEntry("lastprofile");
    }

    return profile.isEmpty() ? QString::fromLatin1("default") : profile;
}


void MainConfig::setLastProfile(const QString& profile)
{
    Q_D( MainConfig );
    if (!d->config.isNull()) {
        d->config->reparseConfiguration();
        d->general.writeEntry("lastprofile", profile);
        d->general.sync();
    }
}
