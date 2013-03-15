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

#ifndef TABLETAREASELECTIONVIEW_H
#define TABLETAREASELECTIONVIEW_H

#include <QtCore/QRect>
#include <QtCore/QString>
#include <QtGui/QWidget>

namespace Wacom
{

class TabletAreaSelectionViewPrivate;

/**
 * A widget which displays the current screen area selection and lets
 * the user map this selection to a tablet area.
 */
class TabletAreaSelectionView : public QWidget
{
    Q_OBJECT

public:

    explicit TabletAreaSelectionView( QWidget* parent = 0 );

    virtual ~TabletAreaSelectionView();


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
    void setSelection( const QString& selection );


    /**
     * Sets up the widget. This method has to be called before a
     * selection can be set. The method can be called at any time
     * to reinitilize the widget.
     *
     * @param tabletArea The geometry of the full tablet area.
     * @param screenAreas The geometries of all X11 screens currently available.
     * @param screenAreaSelection The geometry of the current screen area selection.
     * @param deviceName The Xinput name of the tablet device which is configured.
     */
    void setupWidget( const QRect& tabletArea, const QList< QRect >& screenAreas, const QRect& screenAreaSelection, const QString& deviceName );


public slots:

    /**
     * Called when the user wants to calibrate the current device.
     */
    void onCalibrateClicked();

    /**
     * Called when the user wants to adjust the selection to the screen proportions.
     */
    void onForceProportionsClicked();

    /**
     * Called when the user wants to select the full tablet area.
     */
    void onFullTabletSelected(bool checked);

    /**
     * Called when the user changes the tablet area selection.
     */
    void onTabletAreaChanged();

    /**
     * Called when the user wants to select an area of the tablet.
     */
    void onTabletAreaSelected(bool checked);


signals:

    /**
     * Emitted when the configuration changes.
     */
    void changed();


protected:

    /**
     * The available mapping types for the tablet.
     */
    enum TabletAreaType {
        FullTabletArea,    //!< Enables full desktop selection.
        PartialTabletArea  //!< Enables area selection.
    };


    /**
     * Sets a selection based on the given geometry.
     *
     * @param selection The geometry of the new selection.
     */
    void setSelection(const QRect& selection);


    /**
     * Sets the given mapping type and updates the widgets
     * and the selection.
     *
     * @param type The new mapping type.
     */
    void setTabletAreaType(TabletAreaType type);


private:

    /**
     * Checks if the given selection selects the full tablet area.
     *
     * @param selection The selection to check.
     *
     * @return True if the given selection selects the full tablet area, else false.
     */
    bool isFullAreaSelection(const QRect& selection) const;

    /**
     * Sets up this widget. Must only be called once by the constructor.
     */
    void setupUi();

    /**
     * Sets up the screen area widget.
     *
     * @param screenAreas A list of X11 screen geometries.
     * @param screenAreaSelection The selection to set.
     */
    void setupScreenArea(const QList< QRect >& screenAreas = QList<QRect>(), const QRect& screenAreaSelection = QRect());

    /**
     * Sets up the tablet area widget.
     *
     * @param tabletArea The geometry of the tablet area.
     */
    void setupTabletArea(const QRect& tabletArea = QRect());


    Q_DECLARE_PRIVATE(TabletAreaSelectionView)
    TabletAreaSelectionViewPrivate *const d_ptr; //!< D-Pointer for this class.

}; // CLASS
}  // NAMESPACE
#endif // HEADER PROTECTION
