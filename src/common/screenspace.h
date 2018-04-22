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

#include <QString>

namespace Wacom
{

class ScreenSpacePrivate;

/**
 * @brief Specifies either a specific display output (e.g. HDMI-0) or whole screen
 */

class ScreenSpace
{

public:
    ScreenSpace();
    ScreenSpace(const QString& screenSpace);
    ScreenSpace(const ScreenSpace& screenSpace);
    virtual ~ScreenSpace();

    ScreenSpace& operator= (const ScreenSpace& screenSpace);

    bool operator== (const ScreenSpace& screenSpace) const;

    bool operator!= (const ScreenSpace& screenSpace) const;

    static const ScreenSpace desktop();

    bool isDesktop() const;

    bool isMonitor() const;

    static const ScreenSpace monitor(QString output);

    const QString toString() const;

    /**
     * This function allows to cycle through all existing screen spaces in a loop,
     * which means every individual connected screen and "desktop" (whole screen)
     *
     * @return Next ScreenSpace after this
     */
    ScreenSpace next() const;

private:
    void setScreenSpace(const QString& screenSpace);

    Q_DECLARE_PRIVATE(ScreenSpace)
    ScreenSpacePrivate *const d_ptr; // D-Pointer for private members.

}; // CLASS
}  // NAMESPACE
#endif // HEADER PROTECTION
