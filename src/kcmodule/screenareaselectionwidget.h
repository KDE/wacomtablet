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

#include <QtCore/QRect>
#include <QtCore/QList>
#include <QtGui/QWidget>

namespace Wacom
{

class ScreenAreaSelectionWidgetPrivate;

/**
 * A widget which displays the current tablet area selection and
 * let's the user choose a screen mapping for that selection.
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
     * "x y width height" : An area was selected.
     *
     * @return The current selection as string in storage format.
     */
    QString getSelection() const;

    /**
     * Sets the current selection according to a string in storage format.
     * Before this method can be called, the widget has to be setup.
     * Valid values are:
     *
     * empty string       : Selects the whole screen.
     * "full"             : Selects the whole desktop.
     * "mapX"             : Selects monitor X (0 <= X < number of Screens).
     * "x y width height" : Selects an area.
     *
     * @param selection The new selection as string.
     */
    void setSelection( const QString& selection);

    /**
     * Sets up this widget. This is required before it can be used.
     * This method can be called at any time.
     *
     * @param screenAreas        The geometries of all X11 screens.
     * @param tabletArea         The geometry of the full tablet area.
     * @param selectedTabletArea The geometry of the selected tablet area.
     * @param tabletAreaCaption  The caption for the tablet area.
     */
    void setupWidget( const QList<QRect>& screenAreas, const QRect& tabletArea, const QRect& selectedTabletArea, const QString& tabletAreaCaption = QString());


public slots:

    /**
     * Called when the user selects full screen mapping
     */
    void onFullScreenSelected(bool checked);

    /**
     * Called when the user selects a monitor screen.
     */
    void onMonitorScreenSelected();

    /**
     * Called when the user selects monitor mapping.
     */
    void onMonitorSelected(bool checked);

    /**
     * Called when the user wants to select a screen area.
     */
    void onPartialScreenSelected(bool checked);


signals:

    /**
     * Emitted when any value changes.
     */
    void changed();


protected:

    /**
     * The different mapping types which can be selected.
     */
    enum ScreenAreaType {
        FullScreenArea,    //!< Selects the whole desktop.
        MonitorArea,       //!< Selects monitor mapping.
        PartialScreenArea  //!< Selects area mapping.
    };

    /**
     * Updates the selected area based on a selected monitor screen.
     *
     * @param screenNum The screen number to select (0<= screenNum < number of Screens).
     */
    void setMonitorSelection( const int screenNum );


    /**
     * Updates the widgets and the selection according to the given area mapping.
     *
     * @param areaType The new area mapping.
     */
    void setScreenAreaType( ScreenAreaType areaType );


private slots:

    /**
     * Called when the selection of the area widget is changed using
     * the drag handles.
     */
    void onScreenAreaChanged();

private:

    /**
     * Sets up the monitor screen selection combo box according to the
     * given screen list.
     *
     * @param screenAreas The currently available screens.
     */
    void setupMonitorComboBox ( const QList<QRect>& screenAreas );

    /**
     * Sets up the screen area widget with the given screen areas.
     *
     * @param screenAreas The screen areas to display.
     */
    void setupScreenArea( const QList<QRect>& screenAreas = QList<QRect>() );

    /**
     * Sets up the tablet area widget with the given area and selection.
     *
     * @param tabletArea The maximum area of the tablet.
     * @param selectedTabletArea The selected tablet area.
     */
    void setupTabletArea (const QRect& tabletArea = QRect(), const QRect& selectedTabletArea = QRect(), const QString& tabletAreaCaption = QString());

    /**
     * Sets up the user interface. Must only be called once by a constructor!
     */
    void setupUi();


    Q_DECLARE_PRIVATE(ScreenAreaSelectionWidget)
    ScreenAreaSelectionWidgetPrivate *const d_ptr; //!< D-Pointer for this class.

}; // CLASS
}  // NAMESPACE
#endif // HEADER PROTECTION
