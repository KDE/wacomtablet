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

#ifndef GLOBALACTIONS_H
#define GLOBALACTIONS_H

#include <KActionCollection>

namespace Wacom
{

class GlobalActions : public KActionCollection
{
    Q_OBJECT
public:
    explicit GlobalActions(bool isConfiguration, QObject *parent);

signals:
    void toggleTouchTriggered();
    void toggleStylusTriggered();
    void toggleScreenMapTriggered();
    void mapToFullScreenTriggered();
    void mapToScreen1Triggered();
    void mapToScreen2Triggered();
    void nextProfileTriggered();
    void previousProfileTriggered();
}; // CLASS
} // NAMESPACE
#endif // HEADER PROTECTION
