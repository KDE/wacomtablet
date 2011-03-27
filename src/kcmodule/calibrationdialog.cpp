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

#include "calibrationdialog.h"

//KDE includes
#include <KDE/KLocalizedString>

//Qt includes
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtGui/QX11Info>

// X11 includes
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/XInput.h>
#include <X11/extensions/XInput2.h>

const int frameGap = 10;
const int boxwidth = 100;

CalibrationDialog::CalibrationDialog( const QString &padname ) :
    QDialog( )
{
    setWindowState( Qt::WindowFullScreen );

    m_padName = padname;
    m_drawCross = 0;
    m_shiftLeft = frameGap;
    m_shiftTop = frameGap;

    getMaxTabletArea();

    QLabel *showInfo = new QLabel();
    showInfo->setText( i18n( "Please tab into all four corners to calibrate the tablet.\nPress escape to cancel the process." ) );
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
    painter.setPen( Qt::black );

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
            m_topLeft = event->globalPos();
            m_shiftLeft = frameGap;
            m_shiftTop = size().height() - frameGap - boxwidth;
            break;
        case 2:
            m_bottomLeft = event->globalPos();
            m_shiftLeft = size().width() - frameGap - boxwidth;
            m_shiftTop = size().height() - frameGap - boxwidth;
            break;
        case 3:
            m_bottomRight = event->globalPos();
            m_shiftLeft = size().width() - frameGap - boxwidth;
            m_shiftTop = frameGap;
            break;
        case 4:
            m_topRight = event->globalPos();
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
}

void CalibrationDialog::getMaxTabletArea()
{
    int ndevices;
    XDevice *dev = NULL;
    Display *dpy = QX11Info::display();

    XDeviceInfo *info = XListInputDevices( dpy, &ndevices );
    for( int i = 0; i < ndevices; i++ ) {
        if( info[i].name == m_padName.toLatin1() ) {
            dev = XOpenDevice( dpy, info[i].id );
            break;
        }
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
    m_originaltabletArea.setX( ldata[0] );
    m_originaltabletArea.setX( ldata[1] );
    m_originaltabletArea.setWidth( ldata[2] );
    m_originaltabletArea.setHeight( ldata[3] );

    // and apply the old values again
    XChangeDeviceProperty( dpy, dev, prop, type, format,
                           PropModeReplace, dataOld, nitems );

    XFlush( dpy );
    free( data );

    XFreeDeviceList( info );


    XCloseDevice( QX11Info::display(), dev );
}
