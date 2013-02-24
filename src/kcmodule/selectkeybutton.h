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

#ifndef SELECTKEYBUTTON_H
#define SELECTKEYBUTTON_H

#include <KDE/KDialog>

namespace Wacom
{

class SelectKeyButtonPrivate;

/**
  * Implements the selectkeybutton.ui designer file
  */
class SelectKeyButton : public KDialog
{
    Q_OBJECT

public:
    /**
      * Creates the KDialog window and set up all elements.
      *
      * @param parent the parent widget
      */
    explicit SelectKeyButton(QWidget *parent = 0);

    /**
      * Destroys the element again.
      */
    ~SelectKeyButton();


    /**
     * Gets the selected button number. If no button was selected, 0 is returned.
     *
     * @return The selected button number or 0.
     */
    int getButton() const;


    /**
     * Sets the button number.
     *
     * @param buttonNumber The button number to set.
     */
    void setButton(int buttonNumber);


private slots:
    /**
      * Called when the used clicks ok in the dialog.
      *
      * Regardless of the translation of the .ui file this parse the selection back to
      * "Button i" because xsetwacom only accept this values.
      */
    void onOkClicked();


private:

    /**
     * Sets up the widget. Should only be called once by the constructor.
     */
    void setupUi();

    Q_DECLARE_PRIVATE(SelectKeyButton)
    SelectKeyButtonPrivate *const d_ptr; //!< D-Pointer for this class.

}; // CLASS
}  // NAMESPACE
#endif // HEADER PROTECTION
