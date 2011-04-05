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

#include "screenarea.h"

#include <QtGui/QMouseEvent>
#include <QtGui/QCursor>
#include <QtGui/QPainter>
#include <QtGui/QPen>
#include <QtGui/QBrush>
#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>

using namespace Wacom;

const qreal handleSize = 6;
const int tabletGap = 20;

ScreenArea::ScreenArea( QWidget *parent ) :
    QWidget( parent )
{
    setMouseTracking( true );

    setMinimumWidth( 600 + 2 * tabletGap );
    setMaximumWidth( 600 + 2 * tabletGap );

    setupWidget();
}

QRect ScreenArea::getSelectedArea()
{
    QRectF selectedArea;

    selectedArea.setX(( m_selectedArea.x() - tabletGap ) / m_scaling );
    selectedArea.setY(( m_selectedArea.y() - tabletGap ) / m_scaling );
    selectedArea.setWidth( m_selectedArea.width() / m_scaling );
    selectedArea.setHeight( m_selectedArea.height() / m_scaling );

    return selectedArea.toRect();
}

QString ScreenArea::getSelectedAreaString()
{
    QRect selectedArea;

    selectedArea.setX(( m_selectedArea.x() - tabletGap ) / m_scaling );
    selectedArea.setY(( m_selectedArea.y() - tabletGap ) / m_scaling );
    selectedArea.setWidth( m_selectedArea.width() / m_scaling );
    selectedArea.setHeight( m_selectedArea.height() / m_scaling );

    QString area = QString::fromLatin1( "%1 %2 %3 %4" )
                   .arg( selectedArea.x() )
                   .arg( selectedArea.y() )
                   .arg( selectedArea.width() )
                   .arg( selectedArea.height() );

    return area;
}

void ScreenArea::resetSelection()
{
    m_selectedArea = QRect( rect().x() + tabletGap,
                            rect().y() + tabletGap,
                            rect().width() - 2 * tabletGap,
                            rect().height() - 2 * tabletGap );

    updateDragHandle();
    update();
}

void ScreenArea::setSelection( QString area )
{
    if( area.isEmpty() ) {
        return;
    }

    QStringList list = area.split( QLatin1String( " " ) );

    m_selectedArea.setX( list.at( 0 ).toInt()*m_scaling + tabletGap );
    m_selectedArea.setY( list.at( 1 ).toInt()*m_scaling + tabletGap );
    m_selectedArea.setWidth( list.at( 2 ).toInt()*m_scaling );
    m_selectedArea.setHeight( list.at( 3 ).toInt()*m_scaling );

    updateDragHandle();
    update();
}

void ScreenArea::setScreenNumber( int screen )
{
    QRect selectedScreen = QApplication::desktop()->screenGeometry( screen );

    m_selectedArea.setX( selectedScreen.x()*m_scaling + tabletGap );
    m_selectedArea.setY( selectedScreen.y()*m_scaling + tabletGap );
    m_selectedArea.setWidth( selectedScreen.width()*m_scaling );
    m_selectedArea.setHeight( selectedScreen.height()*m_scaling );

    updateDragHandle();
    update();
}

void ScreenArea::paintEvent( QPaintEvent *event )
{
    Q_UNUSED( event );

    QPainter painter( this );
    painter.setRenderHint( QPainter::Antialiasing );

    QPen pen;
    pen.setWidth( 1 );
    pen.setColor( Qt::red );
    painter.setPen( pen );
    painter.setBrush( Qt::gray );
    painter.drawRect( m_selectedArea );

    painter.setPen( Qt::black );
    painter.setBrush( Qt::transparent );
    painter.drawRect( m_virtualScreen );

    for( int i = 0; i < m_screens.size(); i++ ) {
        QRectF screen = m_screens.at( i );
        painter.drawRect( screen.x()*m_scaling + tabletGap,
                          screen.y()*m_scaling + tabletGap,
                          screen.width() * m_scaling,
                          screen.height() * m_scaling );

        painter.drawText( screen.x()*m_scaling + tabletGap + screen.width() * m_scaling / 2,
                          screen.y()*m_scaling + tabletGap + screen.height() * m_scaling / 2,
                          QString::fromLatin1( "%1" ).arg( i + 1 ) );
    }

    if( isEnabled() ) {
        painter.setPen( pen );

        // draw drag handles
        painter.setBrush( Qt::red );
        painter.drawRect( m_dragHandleLeft );
        painter.drawRect( m_dragHandleRight );
        painter.drawRect( m_dragHandleTop );
        painter.drawRect( m_dragHandleBottom );
    }

    // paint selected screen size below the screen box
    painter.setPen( Qt::darkRed );
    painter.setBrush( Qt::darkRed );

    QRect screnArea = getSelectedArea();
    QString selectedArea = QString::fromLatin1( "%1 %2, %3x%4" )
                           .arg( screnArea.x() )
                           .arg( screnArea.y() )
                           .arg( screnArea.width() )
                           .arg( screnArea.height() );

    painter.drawText( rect().width() / 2 - 100, rect().height(), selectedArea );
}

void ScreenArea::mouseMoveEvent( QMouseEvent *event )
{
    if( !m_dragMode ) {
        if( m_dragHandleLeft.contains( event->pos() )
            || m_dragHandleRight.contains( event->pos() ) ) {

            setCursor( Qt::SizeHorCursor );

        }
        else if( m_dragHandleTop.contains( event->pos() )
                 || m_dragHandleBottom.contains( event->pos() ) ) {

            setCursor( Qt::SizeVerCursor );

        }
        else {
            setCursor( Qt::ArrowCursor );
        }
    }

    if( m_dragMode ) {
        switch( m_mode ) {
        case 1:
            if( event->x() >= tabletGap && event->x() <= width() - tabletGap ) {
                m_selectedArea.setX( event->x() );
            }
            else if( event->x() < tabletGap ) {
                m_selectedArea.setX( tabletGap );
            }
            else if( event->x() > width() - tabletGap ) {
                m_selectedArea.setX( width() - tabletGap );
            }
            break;
        case 2:
            if( event->x() >= tabletGap && event->x() <= width() - tabletGap ) {
                m_selectedArea.setWidth( event->x() - m_selectedArea.x() );
            }
            else if( event->x() < tabletGap ) {
                m_selectedArea.setWidth( tabletGap - m_selectedArea.x() );
            }
            else if( event->x() > width() - tabletGap ) {
                m_selectedArea.setWidth( width() - tabletGap - m_selectedArea.x() );
            }
            break;
        case 3:
            if( event->y() >= tabletGap && event->y() <= height() - tabletGap ) {
                m_selectedArea.setY( event->y() );
            }
            else if( event->y() < tabletGap ) {
                m_selectedArea.setY( tabletGap );
            }
            else if( event->y() > width() - tabletGap ) {
                m_selectedArea.setY( height() - tabletGap );
            }
            break;
        case 4:
            if( event->y() >= tabletGap && event->y() <= height() - tabletGap ) {
                m_selectedArea.setHeight( event->y() - m_selectedArea.y() );
            }
            else if( event->y() < tabletGap ) {
                m_selectedArea.setHeight( tabletGap - m_selectedArea.y() );
            }
            else if( event->y() > height() - tabletGap ) {
                m_selectedArea.setHeight( height() - tabletGap - m_selectedArea.y() );
            }
            break;
        case 5:
            if( event->x() >= tabletGap && event->x() <= width() - tabletGap
                && event->y() >= tabletGap && event->y() <= height() - tabletGap ) {

                qreal newX = m_selectedArea.x() + event->pos().x() - m_dragPoint.x();

                if( newX >= tabletGap && newX + m_selectedArea.width() <= rect().width() - tabletGap ) {
                    m_selectedArea.setX( newX );
                    m_selectedArea.setWidth( m_selectedArea.width() + event->pos().x() - m_dragPoint.x() );
                }

                qreal newXY = m_selectedArea.y() + event->pos().y() - m_dragPoint.y();

                if( newXY >= tabletGap && newXY + m_selectedArea.height() <= rect().height() - tabletGap ) {
                    m_selectedArea.setY( newXY );
                    m_selectedArea.setHeight( m_selectedArea.height() + event->pos().y() - m_dragPoint.y() );
                }
                m_dragPoint =  event->pos();
            }
            break;
        }

        // cap tablet width / height
        if( m_selectedArea.height() > m_virtualScreen.height() ) {
            m_selectedArea.setHeight( m_virtualScreen.height() );
        }

        if( m_selectedArea.width() > m_virtualScreen.width() ) {
            m_selectedArea.setWidth( m_virtualScreen.width() );
        }

        updateDragHandle();
        update();

        emit selectedArea( getSelectedAreaString() );
    }
}

void ScreenArea::mousePressEvent( QMouseEvent *event )
{
    if( !m_dragMode ) {
        m_mode = 0;
        if( m_dragHandleLeft.contains( event->pos() ) ) {
            m_dragMode = true;
            m_mode = 1;
        }
        else if( m_dragHandleRight.contains( event->pos() ) ) {
            m_dragMode = true;
            m_mode = 2;
        }
        else if( m_dragHandleTop.contains( event->pos() ) ) {
            m_dragMode = true;
            m_mode = 3;
        }
        else if( m_dragHandleBottom.contains( event->pos() ) ) {
            m_dragMode = true;
            m_mode = 4;
        }
        else if( m_selectedArea.contains( event->pos() ) ) {
            m_dragMode = true;
            m_mode = 5;
            setCursor( Qt::SizeAllCursor );

            m_dragPoint = event->pos();
        }
    }
}

void ScreenArea::mouseReleaseEvent( QMouseEvent *event )
{
    Q_UNUSED( event );

    m_dragMode = false;
    m_mode = 0;
}

void ScreenArea::setupWidget()
{
    QRectF virtualScreen;

    if( QApplication::desktop()->isVirtualDesktop() ) {
        int num = QApplication::desktop()->numScreens();

        for( int i = 0; i < num; i++ ) {
            QRect screen = QApplication::desktop()->screenGeometry( i );

            m_screens.append( screen );

            virtualScreen = virtualScreen.united( screen );
        }
    }
    else {
        virtualScreen = QApplication::desktop()->screenGeometry( 0 );
        m_screens.append( virtualScreen );
    }

    m_scaling = ( width() - 2 * tabletGap ) / virtualScreen.width();

    m_virtualScreen.setX( tabletGap );
    m_virtualScreen.setY( tabletGap );
    m_virtualScreen.setWidth( virtualScreen.width() * m_scaling );
    m_virtualScreen.setHeight( virtualScreen.height() * m_scaling );

    setMaximumHeight( m_virtualScreen.height() + 2 * tabletGap );
    setMinimumHeight( m_virtualScreen.height() + 2 * tabletGap );

    m_selectedArea = m_virtualScreen;

    updateDragHandle();
}

void ScreenArea::updateDragHandle()
{
    m_dragHandleLeft.setX( m_selectedArea.x() - handleSize / 2 );
    m_dragHandleLeft.setY( m_selectedArea.y() + m_selectedArea.height() / 2 - handleSize / 2 );
    m_dragHandleLeft.setWidth( handleSize );
    m_dragHandleLeft.setHeight( handleSize );

    m_dragHandleRight.setX( m_selectedArea.x() + m_selectedArea.width() - handleSize / 2 );
    m_dragHandleRight.setY( m_selectedArea.y() + m_selectedArea.height() / 2 - handleSize / 2 );
    m_dragHandleRight.setWidth( handleSize );
    m_dragHandleRight.setHeight( handleSize );

    m_dragHandleTop.setX( m_selectedArea.x() + m_selectedArea.width() / 2 - handleSize / 2 );
    m_dragHandleTop.setY( m_selectedArea.y() - handleSize / 2 );
    m_dragHandleTop.setWidth( handleSize );
    m_dragHandleTop.setHeight( handleSize );

    m_dragHandleBottom.setX( m_selectedArea.x() + m_selectedArea.width() / 2 - handleSize / 2 );
    m_dragHandleBottom.setY( m_selectedArea.y() + m_selectedArea.height() - handleSize / 2 );
    m_dragHandleBottom.setWidth( handleSize );
    m_dragHandleBottom.setHeight( handleSize );
}

