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

#ifndef SCREENAREASELECTIONWIDGET_H
#define SCREENAREASELECTIONWIDGET_H

#include <QtCore/QObject>
#include <QtCore/QRect>
#include <QtCore/QList>
#include <QtGui/QWidget>

namespace Wacom
{

class ScreenAreaSelectionWidgetPrivate;

/**
 * The presenter widget which intializes the ScreenAreaSelectionView and
 * the ScreenAreaSelectionController.
 */
class ScreenAreaSelectionWidget : public QWidget
{
    Q_OBJECT

public:

    explicit ScreenAreaSelectionWidget(QWidget* parent = 0);
    virtual ~ScreenAreaSelectionWidget();

    /**
     * Returns the current selection in profile format.
     * Possible return values are:
     *
     * "full"             : Fullscreen selection.
     * "mapX"             : Monitor X was selected ( 0 <= X < number of Screens)
     *
     * @return The current selection as string in storage format.
     */
    const QString getSelection();

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
     * Sets up the widget. Must be called before the widget can be used.
     * Can be called any time to reconfigure the widget.
     *
     * @param screenSelection The current screen selection in profile format.
     * @param tabletSelection The current tablet selection in profile format.
     * @param tabletCaption   A caption for the tablet area.
     * @param deviceName      The X11 Xinput device name of the tablet device being configured.
     */
    void setupWidget (const QString& screenSelection, const QString& tabletSelection, const QString& tabletCaption, const QString& deviceName);


private:

    void setupUi();

    Q_DECLARE_PRIVATE(ScreenAreaSelectionWidget)
    ScreenAreaSelectionWidgetPrivate *const d_ptr; //!< D-Pointer for this class.

}; // CLASS
}  // NAMESPACE
#endif // HEADER PROTECTION
