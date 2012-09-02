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

#ifndef CONFIGADAPTER_H
#define CONFIGADAPTER_H

#include "propertyadaptor.h"

#include <KDE/KConfigGroup>

namespace Wacom {

/**
 * The base class for configuration adpaters.
 * 
 * These adaptors support direct reading and writing of KDE config groups as
 * well as property mapping between configuration file keys and config or
 * system properties.
 */
class ConfigAdaptor : public PropertyAdaptor {

public:

    /**
      * Default constructor.
      */
    explicit ConfigAdaptor(PropertyAdaptor* adaptee);

    /**
      * Default destructor
      */
    virtual ~ConfigAdaptor();

    /**
     * Loads a config from the given config group. The default implementation
     * does nothing and always returns false.
     * 
     * @param config The configuration group to load the configuration from.
     *
     * @return True on success, false on error.
     */
    virtual bool loadConfig( const KConfigGroup& config );


    /**
     * Save the configuration to a config group. The default implementation
     * does nothing and always returns false.
     *
     * @param config The configuration group to save to configuration in.
     *
     * @return True on success, false on error.
     */
    virtual bool saveConfig( KConfigGroup& config ) const;


};     // CLASS
}      // NAMESPACE
#endif // HEADER PROTECTION
