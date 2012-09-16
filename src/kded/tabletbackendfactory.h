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

#ifndef TABLETBACKENDFACTORY_H
#define TABLETBACKENDFACTORY_H

#include "tabletbackend.h"

namespace Wacom
{
class TabletBackendFactory
{
public:

    /**
     * Creates a new instance of the tablet backend. The tablet information
     * parameter already needs to contain some basic tablet information which
     * can be obtained by querying the underlying window system. This should
     * have been done by the event notifier.
     *
     * The returned tablet backend instance need to be deleted once you are
     * done with it.
     *
     * @param info Basic tablet information.
     *
     * @return A new tablet backend instance.
     */
    static TabletBackend* createBackend (TabletInformation& info);


private:

    //! Private default constructor as this is a static class.
    TabletBackendFactory();

    //! Copy constructor which does nothing.
    TabletBackendFactory(const TabletBackendFactory& factory);

    //! Copy operator which does nothing.
    TabletBackendFactory& operator= (const TabletBackendFactory& factory);

}; // CLASS 
}  // NAMESPACE
#endif // HEADER PROTECTION
