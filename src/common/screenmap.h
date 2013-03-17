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

#ifndef SCREENMAP_H
#define SCREENMAP_H

#include "screenspace.h"
#include "screenrotation.h"

#include <QtCore/QString>
#include <QtCore/QRect>

namespace Wacom
{

class ScreenMapPrivate;

class ScreenMap
{

public:

    explicit ScreenMap(const QRect& tabletGeometry = QRect(-1, -1, -1, -1));
    explicit ScreenMap(const QString& mapping);
    explicit ScreenMap(const ScreenMap& screenMap);

    virtual ~ScreenMap();

    ScreenMap& operator= (const ScreenMap& screenMap);

    void fromString(const QString& mappings);

    const QRect getMapping(const ScreenSpace& screen, const ScreenRotation rotation = ScreenRotation::NONE) const;

    const QString getMappingAsString(const Wacom::ScreenSpace& screen) const;

    void setMapping(const ScreenSpace& screen, const QRect& mapping, const ScreenRotation& rotation = ScreenRotation::NONE);

    const QString toString() const;

private:

    bool isTabletGeometryValid() const;

    const QRect fromRotation(const QRect& tablet, const QRect& area, const ScreenRotation& rotation) const;

    const QRect toRotation(const QRect& tablet, const QRect& area, const ScreenRotation& rotation) const;

    Q_DECLARE_PRIVATE(ScreenMap)
    ScreenMapPrivate *const d_ptr; // D-Pointer for private members.

}; // CLASS
}  // NAMESPACE
#endif // HEADER PROTECTION
