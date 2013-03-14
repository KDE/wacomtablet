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

#ifndef TABLETAREASELECTIONDIALOG_H
#define TABLETAREASELECTIONDIALOG_H

#include <KDE/KDialog>
#include <QtCore/QRect>
#include <QtCore/QList>

namespace Wacom
{

class TabletAreaSelectionDialogPrivate;

/**
 * A dialog which displays the current screen area selection and lets
 * the user map this selection to a tablet area.
 */
class TabletAreaSelectionDialog : public KDialog
{
    Q_OBJECT

public:

    explicit TabletAreaSelectionDialog();

    virtual ~TabletAreaSelectionDialog();


    /**
     * Gets the current selection in profile format.
     * See TabletAreaSelectionWidget::getSelection() for a list of possible return values.
     *
     * @return The current selection in profile format.
     */
    const QString getSelection() const;


    /**
     * Sets the current selection from profile format.
     * See TabletAreaSelectionWidget::getSelection() for a list of valid values.
     *
     * @param selection The selection to set.
     */
    void setSelection(const QString& selection);


    /**
     * Sets up the widget. This method has to be called before any selection can be set.
     * This method can be called at any time to reinitialize the widget.
     * The parameter \a selectedScreenArea expects a value in profile format. Valid
     * values are:
     *
     * - "full"             : Selects the full screen area.
     * - "mapX"             : Selects monitor X.
     * - "x y width height" : Selects the given geometry.
     *
     * @param fullTabletArea The geometry of the full tablet area.
     * @param selectedScreenArea The selected screen area as string in profile format.
     * @param deviceName The Xinput name of the device which is being configured.
     */
    void setupWidget( const QRect& fullTabletArea, const QString& selectedScreenArea, const QString& deviceName );


private:

    /**
     * Converts a screen area selection in profile format to a geometry.
     *
     * @param screenAreas A list of X11 screen areas currently available.
     * @param selectedScreenArea The screen area to convert.
     *
     * @return The selected screen area as geometry.
     */
    const QRect convertScreenAreaMappingToQRect( const QList< QRect >& screenAreas, const QString& selectedScreenArea ) const;


    /**
     * Sets up the dialog. Must only be called once by a constructor.
     */
    void setupUi();

    Q_DECLARE_PRIVATE(TabletAreaSelectionDialog)
    TabletAreaSelectionDialogPrivate *const d_ptr; //!< D-Pointer for this class.

}; // CLASS
} // NAMESPACE
#endif // HEADER PROTECTION
