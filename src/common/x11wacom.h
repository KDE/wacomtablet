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

#ifndef X11WACOM_H
#define X11WACOM_H

#include "devicetype.h"

#include <QtCore/QRectF>
#include <QtCore/QString>

namespace Wacom
{

/**
 * A static class which offers some helper methods to access
 * Wacom devices using xinput.
 */
class X11Wacom
{
public:

    /**
     * Returns the maximum size of the given tablet device.
     *
     * @param deviceName The name of the device to get the area from.
     *
     * @return The maximum size of the tablet area.
     */
    static const QRect getMaximumTabletArea(const QString& deviceName);


    /**
     * Sets the coordinate transformation property on the given device.
     *
     * @return True on success, false on error.
     */
    static bool setCoordinateTransformationMatrix(const QString& deviceName, qreal offsetX, qreal offsetY, qreal width, qreal height);


}; // CLASS
}  // NAMESPACE
#endif // HEADER PROTECTION
