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
     * Get the screen space mapping as a string in profile format.
     *
     * @return The current screen space mapping.
     */
    const QString getMappings();

    /**
     * @return The current screen space which was selected.
     */
    const QString getScreenSpace() const;

    /**
     * Shows the selection for the given screen number (-1 <= screenNumber < number of screens).
     * -1 shows the selection for the full desktop.
     */
    void select(int screenNumber);

    /**
     * Shows the selection for the given screen space.
     */
    void select(const QString& screenSpace);


    /**
     * Sets the view. When a view has been set, the controller
     * needs to be set up again.
     *
     * @param view The view to control.
     */
    void setView(TabletAreaSelectionView* view);


    void setupController( const QString& mappings, const QString& deviceName );


public slots:

    /**
     * Called by the view when the user wants to calibrate the tablet.
     */
    void onCalibrateClicked();

    /**
     * Called by the view when the user wants to toggle the screen.
     */
    void onScreenToggle();

    /**
     * Called by the view when the user wants to set screen proportions on his tablet.
     */
    void onSetScreenProportions();


private:

    const QRect getScreenGeometry(int screenNumber) const;

    const QRect& getMapping(int screenNumber) const;

    void setMapping(int screenNumber, const QRect& mapping);

    void setMappings(const QString& mappings);

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
