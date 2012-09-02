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
#include "configadaptor.h"

#include <QString>


using namespace Wacom;

ConfigAdaptor::ConfigAdaptor(PropertyAdaptor* adaptee) : PropertyAdaptor(adaptee) {}

ConfigAdaptor::~ConfigAdaptor() {}

bool ConfigAdaptor::loadConfig ( const KConfigGroup& config )
{
    assert(&config != NULL); // pointless assertion to prevent "unused parameter" compiler warning
    kError() << QString::fromLatin1("Somone tried to load a configuration, but no one implemented ConfigAdaptor::loadConfig()!");
    return false;
}

bool ConfigAdaptor::saveConfig ( KConfigGroup& config ) const
{
    assert(&config != NULL); // pointless assertion to prevent "unused parameter" compiler warning
    kError() << QString::fromLatin1("Somone tried to save a configuration, but no one implemented ConfigAdaptor::saveConfig()!");
    return false;
}

