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

class ScreenAreaSelectionWidget : public QWidget
{
    Q_OBJECT

public:

    explicit ScreenAreaSelectionWidget(QWidget* parent = 0);

    virtual ~ScreenAreaSelectionWidget();

    QString getSelection() const;

    void setSelection( const QString& selection);

    void setupWidget( const QList<QRect>& screenAreas, const QRect& tabletArea, const QRect& selectedTabletArea, const QString& tabletAreaCaption = QString());


public slots:

    void onFullScreenSelected(bool checked);

    void onMonitorScreenSelected();

    void onMonitorSelected(bool checked);

    void onPartialScreenSelected(bool checked);


signals:

    void changed();


protected:

    enum ScreenAreaType {
        FullScreenArea,
        MonitorArea,
        PartialScreenArea
    };

    void setMonitorSelection( const int screenNum );

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
