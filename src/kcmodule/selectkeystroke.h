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

#ifndef SELECTKEYSTROKE_H
#define SELECTKEYSTROKE_H

#include <KDE/KDialog>

namespace Ui
{
class SelectKeyStroke;
}

namespace Wacom
{

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
    SelectKeyStroke(QWidget *parent = 0);

    /**
      * Destroys the element again.
      */
    ~SelectKeyStroke();

    /**
      * Returns the selected key or keystroke as plaintext as used by xsetwacom
      *
      * @return untranslated plaintext string of the keystroke for xsetwacom
      */
    QString keyStroke() const;

private slots:
    /**
      * Called when the used clicks ok in the dialog.
      *
      * Either the text from the combobox or from the sequencewidget are used.
      * All "+" strings are replaced with spaces and instead of the translated
      * values the original values are used as expected by xsetwacom.
      * The output can be used directly for the xsetwacom command
      */
    void slotOkClicked();

private:
    Ui::SelectKeyStroke *ui; /**< Ui designer file for this object */
    QString m_keyStroke;     /**< Cached and parsed string of the keystroke */
};

}
#endif // SELECTKEYSTROKE_H
