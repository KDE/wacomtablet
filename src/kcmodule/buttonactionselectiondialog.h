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

#ifndef BUTTONACTIONSELECTIONDIALOG_H
#define BUTTONACTIONSELECTIONDIALOG_H

#include <KDE/KDialog>

namespace Wacom {

class ButtonShortcut;
class ButtonActionSelectionDialogPrivate;

class ButtonActionSelectionDialog : public KDialog
{
    Q_OBJECT

public:
    explicit ButtonActionSelectionDialog(QWidget* parent = 0);

    virtual ~ButtonActionSelectionDialog();

    /**
     * @return The selected button shortcut.
     */
    const ButtonShortcut& getShortcut() const;

    /**
     * Sets a button shortcut.
     *
     * @param shortcut The shortcut to set.
     */
    void setShortcut(const ButtonShortcut& shortcut);


private:

    Q_DECLARE_PRIVATE(ButtonActionSelectionDialog)
    ButtonActionSelectionDialogPrivate *const d_ptr; //!< D-Pointer for this class.

}; // CLASS
}  // NAMESPACE
#endif // HEADER PROTECTION
