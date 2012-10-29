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

#include "tabletarea.h"

#include <QtGui/QMouseEvent>
#include <QtGui/QCursor>
#include <QtGui/QPainter>
#include <QtGui/QPen>
#include <QtGui/QBrush>
#include <QtGui/QX11Info>

// X11 includes
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/XInput.h>
#include <X11/extensions/XInput2.h>

using namespace Wacom;

const qreal handleSize = 6;
const int tabletGap = 20;

TabletArea::TabletArea( QWidget *parent ) :
    QWidget( parent )
{
    setMouseTracking( true );

    setMinimumWidth( 400 + 2 * tabletGap );
    setMaximumWidth( 400 + 2 * tabletGap );
}

void TabletArea::setTool( QString toolName )
{
    m_toolName = toolName;
    setupWidget();
}

QRect TabletArea::getOriginalArea()
{
    return m_origialArea.toRect();
}

QString TabletArea::getOriginalAreaString()
{
    int x = ( m_origialArea.x() );
    int y = ( m_origialArea.y() );
    int width = m_origialArea.width();
    int height = m_origialArea.height();

    QString area = QString::fromLatin1( "%1 %2 %3 %4" ).arg( x ).arg( y ).arg( width ).arg( height );

    return area;
}

QRect TabletArea::getSelectedArea()
{
    QRect area;
    area.setX( m_selectedArea.x() / m_scaling );
    area.setY( m_selectedArea.y() / m_scaling );
    area.setWidth( m_selectedArea.width() / m_scaling );
    area.setHeight( m_selectedArea.height() / m_scaling );

    return area;
}

QString TabletArea::getSelectedAreaString()
{
    int x = ( m_selectedArea.x() - tabletGap ) / m_scaling;
    int y = ( m_selectedArea.y() - tabletGap ) / m_scaling;
    int width = m_selectedArea.width() / m_scaling;
    int height = m_selectedArea.height() / m_scaling;

    QString area = QString::fromLatin1( "%1 %2 %3 %4" ).arg( x ).arg( y ).arg( width ).arg( height );

    return area;
}

void TabletArea::resetSelection()
{
    m_selectedArea = QRect( rect().x() + tabletGap,
                            rect().y() + tabletGap,
                            rect().width() - 2 * tabletGap,
                            rect().height() - 2 * tabletGap );

    updateDragHandle();
    update();
}

void TabletArea::setSelection( QString area )
{
    if( area.isEmpty() ) {
        return;
    }

    QStringList list = area.split( QLatin1String( " " ) );

    m_selectedArea.setX( list.at( 0 ).toInt() * m_scaling + tabletGap );
    m_selectedArea.setY( list.at( 1 ).toInt() * m_scaling + tabletGap );
    m_selectedArea.setWidth( list.at( 2 ).toInt() * m_scaling );
    m_selectedArea.setHeight( list.at( 3 ).toInt() * m_scaling );

    updateDragHandle();
    update();
}

void TabletArea::setSelection( qreal width, qreal height )
{
    m_selectedArea.setWidth( width * m_scaling );
    m_selectedArea.setHeight( height * m_scaling );

    updateDragHandle();
    update();
}

void TabletArea::paintEvent( QPaintEvent *event )
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
    painter.drawRect( rect().x() + tabletGap,
                      rect().y() + tabletGap,
                      rect().width() - 2 * tabletGap,
                      rect().height() - 2 * tabletGap );

    painter.setPen( pen );

    if( isEnabled() ) {
        // draw drag handles
        painter.setBrush( Qt::red );
        painter.drawRect( m_dragHandleLeft );
        painter.drawRect( m_dragHandleRight );
        painter.drawRect( m_dragHandleTop );
        painter.drawRect( m_dragHandleBottom );
    }

    // draw area info in the middle
    painter.setPen( Qt::black );
    painter.drawText( rect().width() / 2 - 50, rect().height() / 2 - 15, getOriginalAreaString() );
    painter.setPen( Qt::darkRed );
    painter.drawText( rect().width() / 2 - 50, rect().height() / 2 + 15, getSelectedAreaString() );
}

void TabletArea::mouseMoveEvent( QMouseEvent *event )
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
            if( event->x() > 0 && event->x() < width() ) {
                m_selectedArea.setX( event->x() );
                emit sizeChanged( false );
            }
            break;
        case 2:
            if( event->x() > 0 && event->x() < width() ) {
                m_selectedArea.setWidth( event->x() - m_selectedArea.x() );
                emit sizeChanged( false );
            }
            break;
        case 3:
            if( event->y() > 0 && event->y() < height() ) {
                m_selectedArea.setY( event->y() );
                emit sizeChanged( false );
            }
            break;
        case 4:
            if( event->y() > 0 && event->y() < height() ) {
                qreal newHeight = event->y() - m_selectedArea.y();

                if( newHeight > m_origialArea.height() * m_scaling ) {
                    newHeight = m_origialArea.height() * m_scaling;
                }
                m_selectedArea.setHeight( newHeight );
                emit sizeChanged( false );
            }
            break;
        case 5:
            if( event->x() > 0 && event->x() < width()
                && event->y() > 0 && event->y() < height() ) {

                qreal newX = m_selectedArea.x() + event->pos().x() - m_dragPoint.x();

                if( newX >= 0 && newX + m_selectedArea.width() < rect().width() ) {
                    m_selectedArea.setX( newX );
                    m_selectedArea.setWidth( m_selectedArea.width() + event->pos().x() - m_dragPoint.x() );
                }

                qreal newXY = m_selectedArea.y() + event->pos().y() - m_dragPoint.y();

                if( newXY >= 0 && newXY + m_selectedArea.height() < rect().height() ) {
                    m_selectedArea.setY( newXY );
                    m_selectedArea.setHeight( m_selectedArea.height() + event->pos().y() - m_dragPoint.y() );
                }
                m_dragPoint =  event->pos();
            }
            break;
        }

        // cap tablet width / height
        if( m_selectedArea.height() > m_origialArea.height() * m_scaling ) {
            m_selectedArea.setHeight( m_origialArea.height() * m_scaling );
        }

        if( m_selectedArea.width() > m_origialArea.width() * m_scaling ) {
            m_selectedArea.setWidth( m_origialArea.width() * m_scaling );
        }

        updateDragHandle();
        update();

        emit selectedArea( getSelectedAreaString() );
    }
}

void TabletArea::mousePressEvent( QMouseEvent *event )
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

void TabletArea::mouseReleaseEvent( QMouseEvent *event )
{
    Q_UNUSED( event );

    m_dragMode = false;
    m_mode = 0;
}

void TabletArea::setupWidget()
{
    // original area from xinput
    getMaxTabletArea();

    m_scaling = ( width() - 2 * tabletGap ) / m_origialArea.width();

    setMaximumHeight( m_origialArea.height() * m_scaling + 2 * tabletGap );
    setMinimumHeight( m_origialArea.height() * m_scaling + 2 * tabletGap );

    m_selectedArea = QRect( rect().x() + tabletGap,
                            rect().y() + tabletGap,
                            rect().width() - 2 * tabletGap,
                            rect().height() - 2 * tabletGap );

    updateDragHandle();
    update();
}

void TabletArea::updateDragHandle()
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

void TabletArea::getMaxTabletArea()
{
    // TODO remove this! This should not use X11 directly but query a property value.
    int ndevices;
    XDevice *dev = NULL;
    Display *dpy = QX11Info::display();

    XDeviceInfo *info = XListInputDevices( dpy, &ndevices );
    for( int i = 0; i < ndevices; i++ ) {
        if( info[i].name == m_toolName.toLatin1() ) {
            dev = XOpenDevice( dpy, info[i].id );
            break;
        }
    }

    if( !dev ) {
        return;
    }

    Atom prop, type;
    int format;
    unsigned char *data = NULL;
    unsigned char *dataOld = NULL;
    unsigned long nitems, bytes_after;
    long *ldata;

    prop = XInternAtom( dpy, "Wacom Tablet Area", True );

    XGetDeviceProperty( dpy, dev, prop, 0, 1000, False, AnyPropertyType,
                        &type, &format, &nitems, &bytes_after, &dataOld );

    XGetDeviceProperty( dpy, dev, prop, 0, 1000, False, AnyPropertyType,
                        &type, &format, &nitems, &bytes_after, &data );

    ldata = ( long * )data;

    // first reset to default values
    ldata[0] = -1;
    ldata[1] = -1;
    ldata[2] = -1;
    ldata[3] = -1;

    XChangeDeviceProperty( dpy, dev, prop, type, format,
                           PropModeReplace, data, nitems );

    // Now get the defaults
    XGetDeviceProperty( dpy, dev, prop, 0, 1000, False, AnyPropertyType,
                        &type, &format, &nitems, &bytes_after, &data );

    ldata = ( long * )data;
    m_origialArea.setX( ldata[0] );
    m_origialArea.setX( ldata[1] );
    m_origialArea.setWidth( ldata[2] );
    m_origialArea.setHeight( ldata[3] );

    // and apply the old values again
    XChangeDeviceProperty( dpy, dev, prop, type, format,
                           PropModeReplace, dataOld, nitems );

    XFlush( dpy );

    free( data );
    XFreeDeviceList( info );
    XCloseDevice( QX11Info::display(), dev );
}
