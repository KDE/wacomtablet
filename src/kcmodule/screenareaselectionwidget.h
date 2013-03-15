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

#include <QtCore/QList>
#include <QtCore/QRect>
#include <QtCore/QSize>
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
     * @return The currently selected monitor or -1 if the whole desktop is selected.
     */
    int getSelectedMonitor() const;

    /**
     * Selects the whole desktop.
     */
    void setFullScreenSelection();


    /**
     * Updates the selected area based on a selected monitor screen.
     *
     * @param screenNum The screen number to select (0<= screenNum < number of Screens).
     */
    void setMonitorSelection( const int screenNum );


    /**
     * Sets up the screen area widget with the given screen areas.
     *
     * @param screenAreas The screen areas to display.
     */
    void setupScreens( const QList< QRect >& screenGeometries, const QSize& widgetTargetSize );


    /**
     * Sets up the tablet area widget with the given area and selection.
     *
     * @param tabletArea The maximum area of the tablet.
     * @param selectedTabletArea The selected tablet area.
     */
    void setupTablet(const QRect& geometry, const QRect& selection, const QString& caption, const QSize& widgetTargetSize);



public slots:

    /**
     * Called by the UI when the user selects full screen mapping.
     */
    void onFullScreenSelected(bool checked);

    /**
     * Called by the UI when the user selects a monitor screen.
     */
    void onMonitorScreenSelected();

    /**
     * Called by the UI when the user selects monitor mapping.
     */
    void onMonitorSelected(bool checked);


signals:

    /**
     * Signals the controller that the user selected the full screen area.
     * Should only be used by the controller.
     */
    void signalFullScreenSelected();

    /**
     * Signals the controller that the user selected a monitor.
     * Should only be used by the controller.
     */
    void signalMonitorSelected(int screenNum);


protected:

    /**
     * The different mapping types which can be selected.
     */
    enum ScreenAreaType {
        FullScreenArea,    //!< Selects the whole desktop.
        MonitorArea,       //!< Selects monitor mapping.
    };

    /**
     * Updates the widgets and the selection according to the given area mapping.
     *
     * @param areaType The new area mapping.
     */
    void setScreenAreaType( ScreenAreaType areaType );


private:

    /**
     * Sets up the monitor screen selection combo box according to the
     * given screen list.
     *
     * @param screenAreas The currently available screens.
     */
    void setupMonitorComboBox ( const QList<QRect>& screenAreas );

    /**
     * Sets up the user interface. Must only be called once by a constructor!
     */
    void setupUi();


    Q_DECLARE_PRIVATE(ScreenAreaSelectionWidget)
    ScreenAreaSelectionWidgetPrivate *const d_ptr; //!< D-Pointer for this class.

}; // CLASS
}  // NAMESPACE
#endif // HEADER PROTECTION
