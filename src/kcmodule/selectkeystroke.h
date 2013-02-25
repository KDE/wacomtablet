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

#ifndef SELECTKEYSTROKE_H
#define SELECTKEYSTROKE_H

#include <KDE/KDialog>

namespace Ui
{
class SelectKeyStroke;
}

class KKeySequenceWidget;
class QCheckBox;

namespace Wacom
{

class ButtonShortcut;
class SelectKeyStrokePrivate;

/**
  * Implements the selectkeystroke.ui designer file
  */
class SelectKeyStroke : public KDialog
{
    Q_OBJECT

public:
    /**
      * Creates the KDialog window and set up all elements.
      *
      * @param parent the parent widget
      */
    explicit SelectKeyStroke(QWidget *parent = 0);

    /**
      * Destroys the element again.
      */
    ~SelectKeyStroke();

    /**
     * @return The selected shortcut.
     */
    const ButtonShortcut& getShortcut() const;

    /**
     * Sets a shortcut.
     *
     * @param shortcut The shortcut to set.
     */
    void setShortcut(const ButtonShortcut& shortcut);


private slots:

    /**
     * Called when the keyboard shortcut sequence is modified.
     */
    void onShortcutChanged (QKeySequence sequence);

    /**
     * Called when the state of a modifier checkbox changes.
     */
    void onModifierChanged(int state);

    /**
     * Called when the button is clicked to clear the modifiers.
     */
    void onModifiersCleared(bool checked);
    

private:

    /**
     * Sets up the user interface. This should only be called once by the constructor.
     */
    void setupUi();

    void updateActionName (const ButtonShortcut& shortcut);

    void updateModifierWidgets (const ButtonShortcut& shortcut);

    void updateQCheckBox (QCheckBox& checkbox, bool isChecked, bool isEnabled) const;

    void updateShortcutWidgets (const ButtonShortcut& shortcut);


    Q_DECLARE_PRIVATE( SelectKeyStroke )
    SelectKeyStrokePrivate *const d_ptr; //!< D-Pointer for this class.

};     // CLASS
}      // NAMESPACE
#endif // HEADER PROTECTION
