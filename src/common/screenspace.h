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

#ifndef SCREENSPACE_H
#define SCREENSPACE_H

#include <QtCore/QString>

namespace Wacom
{

class ScreenSpacePrivate;

class ScreenSpace
{

public:
    enum ScreenSpaceType
    {
        DESKTOP,
        MONITOR
    };

    ScreenSpace();
    ScreenSpace(const QString& screenSpace);
    ScreenSpace(const ScreenSpace& screenSpace);
    virtual ~ScreenSpace();

    ScreenSpace& operator= (const ScreenSpace& screenSpace);

    bool operator== (const ScreenSpace& screenSpace) const;

    bool operator!= (const ScreenSpace& screenSpace) const;

    static const ScreenSpace desktop();

    int getScreenNumber() const;

    bool isDesktop() const;

    bool isMonitor() const;

    bool isMonitor(int screenNumber) const;

    static const ScreenSpace monitor(int screenNumber);

    const QString toString() const;


private:
    ScreenSpace(ScreenSpaceType type, int monitorNumber = -1);

    void setScreenSpace(const QString& screenSpace);

    Q_DECLARE_PRIVATE(ScreenSpace)
    ScreenSpacePrivate *const d_ptr; // D-Pointer for private members.

}; // CLASS
}  // NAMESPACE
#endif // HEADER PROTECTION
