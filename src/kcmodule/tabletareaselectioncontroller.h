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

#ifndef TABLETAREASELECTIONCONTROLLER_H
#define TABLETAREASELECTIONCONTROLLER_H

#include <QtCore/QObject>
#include <QtCore/QString>

namespace Wacom
{

class TabletAreaSelectionControllerPrivate;
class TabletAreaSelectionView;

class TabletAreaSelectionController : public QObject
{
    Q_OBJECT

public:

    TabletAreaSelectionController();
    virtual ~TabletAreaSelectionController();


    /**
     * Gets the current tablet area selection in profile format.
     * Possible return values are:
     *
     * - "-1 -1 -1 -1" : The full tablet area was selected.
     * - "x1 y1 x2 y2" : An area was selected with the top left corner at (x1/y1)
     *                   and the bottom right corner at (x2/y2).
     *
     * @return The current selection in profile format.
     */
    const QString getSelection() const;


    /**
     * Sets the current selection from a profile format string.
     * Valid values are:
     *
     * - "-1 -1 -1 -1" : The full tablet area gets selected.
     * - "x1 y1 x2 y2" : An area gets selected with the top left corner at (x1/y1)
     *                   and the bottom right corner at (x2/y2).
     *
     * @param selection The new selection.
     */
    void setSelection(const QString& selection);


    /**
     * Sets the view. When a view has been set, the controller
     * needs to be set up again.
     *
     * @param view The view to control.
     */
    void setView(TabletAreaSelectionView* view);


    /**
     * Sets up the controller. This needs a valid view to be set first.
     *
     * @param tabletSelection The tablet selection in property format.
     * @param screenSelection The screen selection in property format.
     * @param deviceName      The name of the X11 Xinput device which is being configured.
     */
    void setupController (const QString& tabletSelection, const QString& screenSelection, const QString& deviceName);


public slots:

    /**
     * Called by the view when the user wants to calibrate the tablet.
     */
    void onCalibrateClicked();

    /**
     * Called by the view when the user wants to set screen proportions on his tablet.
     */
    void onSetScreenProportions();


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
     * @return True if this controller has a view, else false.
     */
    bool hasView() const;

    /**
     * Sets a selection on the view. This also checks for invalid selections
     * and automatically determines if the selection is a full tablet selection
     * or a part of the tablet.
     *
     * @param selection The selection to set.
     */
    void setSelection(const QRect& selection);


    Q_DECLARE_PRIVATE(TabletAreaSelectionController)
    TabletAreaSelectionControllerPrivate *const d_ptr; //!< D-Pointer for this class.

}; // CLASS
}  // NAMESPACE
#endif // HEADER PROTECTION
