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

#ifndef X11INFO_H
#define X11INFO_H

#include "screenrotation.h"

#include <QtCore/QList>
#include <QtCore/QRectF>

namespace Wacom
{

/**
 * A static class with some helper methods to get information about the running X server.
 * This should be used by all our classes instead of QX11Info. This way we have all X
 * specific stuff in one place.
 */
class X11Info
{
public:

    /**
     * @return All screens united as one rectangle.
     */
    static const QRect getDisplayGeometry();


    /**
     * @return The number of screens the current display has.
     */
    static int getNumberOfScreens();

    /**
     * Returns a list of all X11 screen geometries.
     *
     * @return List of X11 screen geometries.
     */
    static const QList< QRect > getScreenGeometries();

    /**
     * Gets the current rotation of the screen. This is based on the monitor
     * rotation and not the rotation of the canvas. If the monitor is e.g.
     * rotated to the right, then the canvas is rotated to the left.
     *
     * @return The current rotation as seen by the monitor.
     */
    static const ScreenRotation getScreenRotation();


private:

    X11Info() {
        // no instance required - it's all static.
    }

}; // CLASS
}  // NAMESPACE
#endif // HEADER PROTECTION
