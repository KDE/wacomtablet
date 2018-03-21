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

//KDE includes
#include <KLocalizedString>

//Qt includes
#include <QLabel>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QX11Info>

#include <xcb/xcb.h>
#include <xcb/xinput.h>
// X11 includes
#include <X11/extensions/XInput.h>

#include <xorg/wacom-properties.h>

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

    getMaxTabletArea();

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
}

void CalibrationDialog::getMaxTabletArea()
{
    int ndevices;
    XID deviceId = 0;
    Display *dpy = QX11Info::display();

    XDeviceInfo *info = XListInputDevices( dpy, &ndevices );
    for( int i = 0; i < ndevices; i++ ) {
        if( info[i].name == m_toolName.toLatin1() ) {
            xcb_input_open_device_cookie_t open_device_cookie = xcb_input_open_device(QX11Info::connection(), info[i].id);
            xcb_input_open_device_reply_t* open_device_reply = xcb_input_open_device_reply(QX11Info::connection(), open_device_cookie, NULL);
            deviceId = info[i].id;
            bool success = open_device_reply != NULL;
            free(open_device_reply);
            if (success) {
                break;
            } else {
                return;
            }
        }
    }

    Atom prop;

    xcb_intern_atom_cookie_t atom_cookie = xcb_intern_atom(QX11Info::connection(), true, strlen(WACOM_PROP_TABLET_AREA), WACOM_PROP_TABLET_AREA);
    xcb_intern_atom_reply_t* atom_reply = xcb_intern_atom_reply(QX11Info::connection(), atom_cookie, NULL);
    if (!atom_reply) {
        return;
    }

    prop = atom_reply->atom;
    free(atom_reply);

    xcb_input_get_device_property_cookie_t cookie = xcb_input_get_device_property(QX11Info::connection(), prop, XCB_ATOM_ANY, 0, 1000, deviceId, false);
    xcb_input_get_device_property_reply_t* reply = xcb_input_get_device_property_reply(QX11Info::connection(), cookie, NULL);
    if (!reply || reply->type != XCB_ATOM_INTEGER || reply->num_items < 4) {
        return;
    }

    uint32_t dataOld[4];
    uint32_t* items = static_cast<uint32_t*>(xcb_input_get_device_property_items(reply));
    for (int i = 0; i < 4; i ++) {
        dataOld[i] = items[i];
    }

    free(reply);

    uint32_t ldata[4] = {(uint32_t) -1, (uint32_t) -1, (uint32_t) -1, (uint32_t) -1};
    xcb_input_change_device_property(QX11Info::connection(), prop, XCB_ATOM_INTEGER, deviceId, 32, XCB_PROP_MODE_REPLACE, 4, ldata);

    cookie = xcb_input_get_device_property(QX11Info::connection(), prop, XCB_ATOM_ANY, 0, 1000, deviceId, false);
    reply = xcb_input_get_device_property_reply(QX11Info::connection(), cookie, NULL);

    if (!reply || reply->type != XCB_ATOM_INTEGER || reply->num_items < 4) {
        return;
    }

    items = static_cast<uint32_t*>(xcb_input_get_device_property_items(reply));
    m_originaltabletArea.setX( items[0] );
    m_originaltabletArea.setX( items[1] );
    m_originaltabletArea.setWidth( items[2] );
    m_originaltabletArea.setHeight( items[3] );

    free(reply);

    xcb_input_change_device_property(QX11Info::connection(), prop, XCB_ATOM_INTEGER, deviceId, 32, XCB_PROP_MODE_REPLACE, 4, dataOld);
    XFreeDeviceList( info );
    xcb_input_close_device(QX11Info::connection(), deviceId);
}
