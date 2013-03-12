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

#ifndef TABLETAREASELECTIONWIDGET_H
#define TABLETAREASELECTIONWIDGET_H

#include <QtCore/QRectF>
#include <QtCore/QString>
#include <QtGui/QWidget>

class QRectF;

namespace Wacom
{

class TabletAreaSelectionWidgetPrivate;

class TabletAreaSelectionWidget : public QWidget
{
    Q_OBJECT

public:

    explicit TabletAreaSelectionWidget(QWidget* parent = 0);

    virtual ~TabletAreaSelectionWidget();


    const QString getSelection() const;

    void setSelection(const QString& selection);

    void setupWidget( const QRect& tabletArea, const QList< QRect >& screenAreas, const QRect& screenAreaSelection, const QString& deviceName );


public slots:

    void onCalibrateClicked();

    void onForceProportionsClicked();

    void onFullTabletSelected(bool checked);

    void onTabletAreaChanged();

    void onTabletAreaSelected(bool checked);


signals:

    void changed();


protected:

    enum TabletAreaType {
        FullTabletArea,
        PartialTabletArea
    };

    void setSelection(const QRect& selection);

    void setTabletAreaType(TabletAreaType type);


private:

    bool isFullAreaSelection(const QRect& selection) const;

    void setupUi();

    void setupScreenArea(const QList< QRect >& screenAreas = QList<QRect>(), const QRect& screenAreaSelection = QRect());

    void setupTabletArea(const QRect& tabletArea = QRect());

    Q_DECLARE_PRIVATE(TabletAreaSelectionWidget)
    TabletAreaSelectionWidgetPrivate *const d_ptr; //!< D-Pointer for this class.

}; // CLASS
}  // NAMESPACE
#endif // HEADER PROTECTION
