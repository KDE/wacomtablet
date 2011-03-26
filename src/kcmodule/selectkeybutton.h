/*
 * Copyright 2010 Jörg Ehrichs <joerg.ehichs@gmx.de>
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

namespace Ui
{
class SelectKeyButton;
}

namespace Wacom
{

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
    SelectKeyButton(QWidget *parent = 0);

    /**
      * Destroys the element again.
      */
    ~SelectKeyButton();

    /**
      * The selected button number
      *
      * @return button selection as sting "Button i"
      */
    QString keyButton();

private slots:
    /**
      * Called when the used clicks ok in the dialog.
      *
      * Regardless of the translation of the .ui file this parse the selection back to
      * "Button i" because xsetwacom only accept this values.
      */
    void slotOkClicked();

private:
    Ui::SelectKeyButton *ui; /**< Ui designer file for this object */
    QString m_keyButton;     /**< The selected button as parsed text ready for xsetwacom usage */
};

}
#endif // SELECTKEYBUTTON_H
