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

#ifndef X11TABLETFINDER_H
#define X11TABLETFINDER_H

#include "x11inputvisitor.h"
#include "tabletinformation.h"

#include <QtCore/QString>

namespace Wacom
{

// Forward Declarations
class X11TabletFinderPrivate;

/**
 * An X11 input visitor which is used by X11Input to find tablet devices.
 */
class X11TabletFinder : public X11InputVisitor
{

public:

    X11TabletFinder();
    virtual ~X11TabletFinder();

    /**
     * Returns the tablet information which was found or an empty
     * structure if none was found.
     */
    const TabletInformation& getInformation() const;

    /**
     * Checks if a device was found.
     *
     * @return True if a device was found, else false.
     */
    bool isAvailable() const;


    /**
     * @see X11InputVisitor::visit(X11InputDevice&)
     */
    bool visit (X11InputDevice& device);


protected:

    /**
     * Gets the tablet id of this device.
     *
     * @return The tablet id or 0 on error.
     */
    const QString getTabletId (X11InputDevice& device);

    /**
     * Gets the tool type name from the wacom tool type property.
     *
     * @return The tool type name on success, an empty string on error.
     */
    const QString getToolType (X11InputDevice& device);

    /**
     * Parses the tool type name and updates the tablet information.
     *
     * @return True if the tool type is supported, else false.
     */
    bool parseToolType(const QString& toolType, const QString& deviceName);


private:

    Q_DECLARE_PRIVATE(X11TabletFinder)
    X11TabletFinderPrivate *const d_ptr;

}; // CLASS
}  // NAMESPACE
#endif // HEADER PROTECTION
