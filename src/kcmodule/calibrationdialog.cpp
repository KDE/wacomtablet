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

#include "calibrationdialog.h"

#include "debug.h"

//KDE includes
#include <KLocalizedString>

//Qt includes
#include <QLabel>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QX11Info>

#include <x11wacom.h>

using namespace Wacom;

const int frameGap = 10;
const int boxwidth = 100;

CalibrationDialog::CalibrationDialog( const QString &toolname ) :
    QDialog( )
{
    setWindowState( Qt::WindowFullScreen );

    m_toolName = toolname;
    m_drawCross = 0;
    m_shiftLeft = frameGap;
    m_shiftTop = frameGap;

    m_originaltabletArea = X11Wacom::getMaximumTabletArea(m_toolName);

    QLabel *showInfo = new QLabel();
    showInfo->setText( i18n( "Please tap into all four corners to calibrate the tablet.\nPress escape to cancel the process." ) );
    showInfo->setAlignment( Qt::AlignCenter );
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget( showInfo );

    setLayout(mainLayout);
}

QRect CalibrationDialog::calibratedArea()
{
    return m_newtabletArea.toRect();
}

void CalibrationDialog::paintEvent( QPaintEvent *event )
{
    Q_UNUSED( event );

    QPainter painter( this );
    painter.setPen(palette().color(QPalette::WindowText));

    // vertical line
    painter.drawLine( m_shiftLeft + boxwidth / 2,
                      m_shiftTop,
                      m_shiftLeft + boxwidth / 2,
                      m_shiftTop + boxwidth );

    // horizontal line
    painter.drawLine( m_shiftLeft,
                      m_shiftTop + boxwidth / 2,
                      m_shiftLeft + boxwidth,
                      m_shiftTop + boxwidth / 2 );

    // draw circle around center
    painter.drawEllipse( QPoint( m_shiftLeft + boxwidth / 2,
                                 m_shiftTop + boxwidth / 2 ),
                         10, 10 );
}

void CalibrationDialog::mousePressEvent( QMouseEvent *event )
{
    if( event->pos().x() > m_shiftLeft
        && event->pos().x() < m_shiftLeft + boxwidth
        && event->pos().y() > m_shiftTop
        && event->pos().y() < m_shiftTop + boxwidth ) {

        m_drawCross++;

        switch( m_drawCross ) {
        case 1:
            m_topLeft = event->windowPos();
            m_shiftLeft = frameGap;
            m_shiftTop = size().height() - frameGap - boxwidth;
            break;
        case 2:
            m_bottomLeft = event->windowPos();
            m_shiftLeft = size().width() - frameGap - boxwidth;
            m_shiftTop = size().height() - frameGap - boxwidth;
            break;
        case 3:
            m_bottomRight = event->windowPos();
            m_shiftLeft = size().width() - frameGap - boxwidth;
            m_shiftTop = frameGap;
            break;
        case 4:
            m_topRight = event->windowPos();
            calculateNewArea();
            close();
            break;
        }

        update();
    }
}

void CalibrationDialog::calculateNewArea()
{
    qreal frameoffset = frameGeometry().height() - size().height();
    qreal tabletScreenRatioWidth = m_originaltabletArea.width() / size().width();
    qreal tabletScreenRatioHeight = m_originaltabletArea.height() / size().height();

    qreal clickedX = ( m_topLeft.x() + m_bottomLeft.x() ) / 2;
    qreal newX = ( clickedX - frameGap - boxwidth / 2 ) * tabletScreenRatioWidth;

    qreal clickedY = ( m_topLeft.y() + m_topRight.y() ) / 2;
    qreal newY = ( clickedY - frameGap - boxwidth / 2 - frameoffset ) * tabletScreenRatioHeight;

    qreal clickedWidth = ( m_topRight.x() + m_bottomRight.x() ) / 2;
    qreal newWidth = ( clickedWidth + frameGap + boxwidth / 2 ) * tabletScreenRatioWidth;

    qreal clickedHeight = ( m_bottomRight.y() + m_bottomLeft.y() ) / 2;
    qreal newHeight = ( clickedHeight + frameGap + boxwidth / 2 + frameoffset ) * tabletScreenRatioHeight;

    m_newtabletArea.setX( newX );
    m_newtabletArea.setY( newY );
    m_newtabletArea.setWidth( newWidth );
    m_newtabletArea.setHeight( newHeight );

    dbgWacom << "Calibration debug:" << frameGeometry() << size() << m_originaltabletArea << m_topLeft << m_bottomLeft << m_topRight << m_bottomRight;
    dbgWacom << "Calibration debug:" << frameoffset << tabletScreenRatioWidth << tabletScreenRatioHeight << clickedX << clickedY << clickedWidth << clickedHeight;
    dbgWacom << "Calibration debug:" << m_newtabletArea;
}
