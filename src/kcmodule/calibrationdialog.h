/*
 * Copyright 2011 Jörg Ehrichs <joerg.ehichs@gmx.de>
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

#ifndef CALIBRATIONDIALOG_H
#define CALIBRATIONDIALOG_H

#include <QDialog>

class CalibrationDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CalibrationDialog(const QString &padname);

    QRect calibratedArea();

 protected:
     void paintEvent(QPaintEvent *event);
     void mousePressEvent( QMouseEvent * event );

private:
    void calculateNewArea();
    void getMaxTabletArea();
    int m_drawCross;
    int m_shiftLeft;
    int m_shiftTop;

    QString m_padName;
    QRectF m_originaltabletArea;
    QRectF m_newtabletArea;
    QPointF m_topLeft;
    QPointF m_bottomLeft;
    QPointF m_topRight;
    QPointF m_bottomRight;
};

#endif // CALIBRATIONDIALOG_H
