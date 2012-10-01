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

#ifndef X11INPUT_H
#define X11INPUT_H

#include "tabletinformation.h"
#include "x11inputdevice.h"
#include "x11inputvisitor.h"

namespace Wacom
{
class X11Input
{
public:

    /**
     * Find tablet if available. If a tablet was found the tablet information
     * structure will have the available flag set to true.
     *
     * @return A tablet information structure.
     */
    static TabletInformation findTablet();

    /**
     * Iterates over all X11 input devices and passes each device to the
     * visitor object. The visitor can then decide either to continue
     * iteration or abort it.
     */
    static void scanDevices(X11InputVisitor& visitor);

}; // CLASS
}  // NAMESPACE
#endif // HEADER PROTECTION
