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

#ifndef SELECTQUOTETEXT_H
#define SELECTQUOTETEXT_H

#include <KDE/KDialog>

namespace Ui
{
class SelectQuoteText;
}

namespace Wacom {

/**
  * Implements the selectquotetext.ui designer file
  */
class SelectQuoteText : public KDialog
{
    Q_OBJECT
public:
    /**
      * Creates the KDialog window and set up all elements.
      *
      * @param parent the parent widget
      */
    SelectQuoteText(QWidget *parent = 0);

    /**
      * Destroys the element again.
      */
    ~SelectQuoteText();

    /**
      * The quotet text that the user entered for further usage
      *
      * @return the text from the KLineEdit field
      */
    QString quoteText() const;

private slots:
    /**
      * Called when the used clicks ok in the dialog.
      *
      * Uses the text from the KLineEdit field without any changes
      */
    void slotOkClicked();

private:
    Ui::SelectQuoteText *ui; /**< Ui designer file for this object */
    QString m_quoteText;     /**< Cache for the text from the KLineEdit field */
};

}

#endif // SELECTQUOTETEXT_H
