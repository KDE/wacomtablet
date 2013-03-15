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

#include <QtCore/QObject>

#ifndef SCREENAREASELECTIONCONTROLLER_H
#define SCREENAREASELECTIONCONTROLLER_H

namespace Wacom
{

class ScreenAreaSelectionControllerPrivate;
class ScreenAreaSelectionView;


/**
 * The controller class which controls the ScreenAreaSelectionView.
 */
class ScreenAreaSelectionController : public QObject
{
    Q_OBJECT

public:

    ScreenAreaSelectionController();
    virtual ~ScreenAreaSelectionController();


    /**
     * Returns the current selection in profile format.
     * Possible return values are:
     *
     * "full"             : Fullscreen selection.
     * "mapX"             : Monitor X was selected ( 0 <= X < number of Screens)
     *
     * @return The current selection as string in storage format.
     */
    const QString getSelection() const;


    /**
     * Sets the current selection according to a string in storage format.
     * Before this method can be called, the widget has to be setup.
     * Valid values are:
     *
     * empty string       : Selects the whole screen.
     * "full"             : Selects the whole desktop.
     * "mapX"             : Selects monitor X (0 <= X < number of Screens).
     *
     * @param selection The new selection as string.
     */
    void setSelection(const QString& selection);


    /**
     * Sets up the controller.
     *
     * @param screenSelection The current screen selection in profile format.
     * @param tabletSelection The current tablet selection in profile format.
     * @param tabletCaption   A caption for the tablet area.
     * @param deviceName      The X11 Xinput device name of the tablet device being configured.
     */
    void setupController(const QString& screenSelection, const QString& tabletSelection, const QString& tabletCaption, const QString& deviceName);

    /**
     * Sets the widget to control.
     * After setting a widget the controller has to be set up again.
     *
     * @param widget The widget to control
     */
    void setView(ScreenAreaSelectionView* widget);


public slots:

    void onFullScreenSelected();

    void onMonitorSelected(int screenNum);


private:

    /**
     * Converts a tablet area selection from profile format to a QRect.
     *
     * @param selection The selected tablet area.
     * @param geometry  The full tablet area.
     *
     * @return The selection as rectangle.
     */
    const QRect convertTabletAreaMappingToQRect(const QString& selection, const QRect& geometry) const;


    bool hasWidget() const;

    Q_DECLARE_PRIVATE(ScreenAreaSelectionController)
    ScreenAreaSelectionControllerPrivate *const d_ptr; //!< D-Pointer for this class.

}; // CLASS
}  // NAMESPACE
#endif // HEADER PROTECTION
