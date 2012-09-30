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

#ifndef X11INPUT_H
#define X11INPUT_H

#include "x11inputdevice.h"
#include "x11inputvisitor.h"

// X11 forward declarations
struct _XDeviceInfo;

namespace Wacom
{
class X11Input
{
public:

    /**
     * Iterates over all X11 input devices and passes each device to the
     * visitor object. The visitor can then decide either to continue
     * iteration or abort it.
     */
    static void scanDevices(X11InputVisitor& visitor);

}; // CLASS
}  // NAMESPACE
#endif // HEADER PROTECTION
