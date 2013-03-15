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

#ifndef SCREENAREASELECTIONDIALOG_H
#define SCREENAREASELECTIONDIALOG_H

#include <KDE/KDialog>

#include <QtCore/QRect>
#include <QtCore/QString>

namespace Wacom
{

class ScreenAreaSelectionDialogPrivate;

/**
 * A dialog which displays the current tablet area selection and
 * let's the user choose a screen mapping for that selection.
 */
class ScreenAreaSelectionDialog : public KDialog
{
    Q_OBJECT

public:

    explicit ScreenAreaSelectionDialog(QWidget* parent = 0);

    virtual ~ScreenAreaSelectionDialog();

    /**
     * Gets the current selection in profile format.
     * See \a ScreenAreaSelectionWidget::getSelection() for a list of valid return values.
     *
     * @return The current screen selection in profile format.
     *
     * @sa ScreenAreaSelectionWidget::getSelection()
     */
    QString getSelection() const;


    /**
     * Sets the current selection based on the given string in profile format.
     * See \a ScreenAreaSelectionWidget::setSelection(const QString&) for a list of valid values.
     *
     * @sa ScreenAreaSelectionWidget::setSelection(const QString&)
     */
    void setSelection( const QString& selection);


    /**
     * Sets up the dialog. This method has to be called before any
     * selection is set. This method can be called at any time to
     * reinitialize the widget. Be aware that the parameter
     * \a selectedTabletArea is in coordinate format. Valid values are:
     *
     * empty string   : Selects the full tablet area.
     * "-1 -1 -1 -1"  : Selecte the full tablet area.
     * "x1 y1 x2 y2"  : Selects a rectangle with the top left corner at (x1/y1)
     *                  and the bottom right corner at (x2/y2).
     *
     * @param fullTabletArea     The geometry of the full tablet area.
     * @param selectedTabletArea The geometry of the selected tablet area (x1 y1 x2 y2).
     * @param tabletAreaCaption  An optional tablet area caption.
     *
     */
    void setupWidget( const QString& screenSelection, const QString& tabletSelection, const QString& tabletCaption, const QString& deviceName );


private:

    /**
     * Sets up the UI, must only be called once by the constructor.
     */
    void setupUi();


    Q_DECLARE_PRIVATE(ScreenAreaSelectionDialog)
    ScreenAreaSelectionDialogPrivate *const d_ptr; //!< D-Pointer for this class.

}; // CLASS
}  // NAMESPACE
#endif // HEADER PROTECTION
