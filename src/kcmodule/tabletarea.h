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

#ifndef TABLETAREA_H
#define TABLETAREA_H

//Qt includes
#include <QtGui/QWidget>
#include <QtCore/QRect>
#include <QtCore/QRectF>

namespace Wacom {

class TabletArea : public QWidget {
    Q_OBJECT
public:
    explicit TabletArea( QWidget *parent = 0 );

    void setTool( QString toolName );

    QRect getOriginalArea();
    QString getOriginalAreaString();
    QRect getSelectedArea();
    QString getSelectedAreaString();

    void setSelection( QString area );
    void setSelection( qreal width, qreal height );

public slots:
    void resetSelection();

protected:
    void paintEvent( QPaintEvent *event );
    void mouseMoveEvent( QMouseEvent *event );
    void mousePressEvent( QMouseEvent *event );
    void mouseReleaseEvent( QMouseEvent *event );

signals:
    void selectedArea( QString area );
    void sizeChanged(bool changed);

private:
    /**
     * Get tablet width/height from xinput
    */
    void getMaxTabletArea();
    void setupWidget();
    void updateDragHandle();

    QString m_toolName;

    QRectF m_origialArea;   // [tablet units]
    qreal m_scaling;        // scaling factor between tablet/widget units
    QRectF m_selectedArea;  // [widget units]

    QRectF m_dragHandleLeft;
    QRectF m_dragHandleRight;
    QRectF m_dragHandleTop;
    QRectF m_dragHandleBottom;

    bool m_dragMode;
    int m_mode;
    QPoint m_dragPoint;
};

}
#endif // TABLETAREA_H
