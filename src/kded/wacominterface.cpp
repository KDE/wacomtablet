/*
 * Copyright 2009, 2010 Jörg Ehrichs <joerg.ehichs@gmx.de>
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

#include "wacominterface.h"
#include "devicehandler.h"

//KDE includes
#include <KDE/KConfigGroup>
#include <KDE/KSharedConfig>
#include <KDE/KStandardDirs>
#include <KDE/KDebug>

// Qt includes
#include <QtCore/QRect>
#include <QtCore/QStringList>
#include <QtCore/QString>
#include <QtCore/QProcess>
#include <QtCore/QRegExp>

#include <QApplication>
#include <QDesktopWidget>
#include <QtGui/QX11Info>

// X11 includes
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/XInput.h>
#include <X11/extensions/XInput2.h>

using namespace Wacom;

WacomInterface::WacomInterface()
    : DeviceInterface()
{

}

WacomInterface::~WacomInterface()
{
}

void WacomInterface::applyProfile( const QString &device, const QString &section, KConfigGroup *gtprofile )
{
    KConfigGroup deviceGroup( gtprofile, section );

    // first get the general options of the interface
    bool m_forceProportion = false;
    bool m_fullTabletArea = false;

    if( deviceGroup.readEntry( QLatin1String( "0ForceProportions" ) ) == QLatin1String( "true" ) ) {
        m_forceProportion = true;
    }
    else {
        m_forceProportion = false;
    }

    if( deviceGroup.readEntry( QLatin1String( "0TabletArea" ) ) == QLatin1String( "true" ) ) {
        m_fullTabletArea = true;
    }
    else {
        m_fullTabletArea = false;
    }

    foreach( const QString & key, deviceGroup.keyList() ) {
        setConfiguration( device, key, deviceGroup.readEntry( key ) );
    }

    if( section == QLatin1String( "cursor" ) ) {
        setCursorSettings( device, gtprofile );
    }
    else {
        // apply the MapToOutput at the end.
        // this ensures we rotated the device beforehand
        mapTabletToScreen( device, deviceGroup.readEntry( QLatin1String( "0ScreenSpace" ) ) );
    }
}

void WacomInterface::setConfiguration( const QString &device, const QString &param, const QString &value )
{
    if( value.isEmpty() ) {
        return;
    }

    if( param.startsWith( QLatin1String( "0" ) ) ) {
        return;
    }

    QString modifiedParam = param;
    QString  cmd = QString::fromLatin1( "xsetwacom set \"%1\" %2 \"%3\"" ).arg( device ).arg( modifiedParam.replace( QRegExp( QLatin1String( "^Button([0-9])" ) ), QLatin1String( "Button \\1" ) ) ).arg( value );

    QProcess setConf;
    setConf.start( cmd );

    if( !setConf.waitForStarted() ) {
        return;
    }

    if( !setConf.waitForFinished() ) {
        return;
    }

    QByteArray errorOutput = setConf.readAll();

    if( !errorOutput.isEmpty() ) {
        kDebug() << cmd;
        kDebug() << errorOutput;
    }
}

QString WacomInterface::getConfiguration( const QString &device, const QString &param ) const
{
    QString modifiedParam = param;
    QString cmd = QString::fromLatin1( "xsetwacom get \"%1\" %2" ).arg( device ).arg( modifiedParam.replace( QRegExp( QLatin1String( "^Button([0-9])" ) ), QLatin1String( "Button \\1" ) ) );
    QProcess getConf;
    getConf.start( cmd );

    if( !getConf.waitForStarted() ) {
        return QString();
    }

    if( !getConf.waitForFinished() ) {
        return QString();
    }

    QString result = QLatin1String( getConf.readAll() );
    return result.remove( QLatin1Char( '\n' ) );
}

QString WacomInterface::getDefaultConfiguration( const QString &device, const QString &param ) const
{
    QString modifiedParam = param;
    QString cmd = QString::fromLatin1( "xsetwacom get \"%1\" %2" ).arg( device ).arg( modifiedParam.replace( QRegExp( QLatin1String( "^button([0-9])" ) ), QLatin1String( "button \\1" ) ) );
    QProcess getConf;
    getConf.start( cmd );

    if( !getConf.waitForStarted() ) {
        return QString();
    }

    if( !getConf.waitForFinished() ) {
        return QString();
    }

    QString result = QLatin1String( getConf.readAll() );
    return result.remove( QLatin1Char( '\n' ) );
}

void WacomInterface::mapTabletToScreen( const QString &device, const QString &screenArea )
{
    //what we need is the Coordinate Transformation Matrix
    // in the normal case where the whole screen is used we end up with a 3x3 identity matrix

    //in our case we want to change that
    // | w  0  offsetX |
    // | 0  h  offsetY |
    // | 0  0     1    |

    QStringList screenList = screenArea.split( QLatin1String( " " ) );

    if( screenList.isEmpty() || screenList.size() != 4 ) {
        kError() << "mapTabletToScreen :: can't parse ScreenSpace entry => device:" << device;
        return;
    }

    // read in what the suer whants to use
    int screenX = screenList.at( 0 ).toInt();
    int screenY = screenList.at( 1 ).toInt();
    int screenW = screenList.at( 2 ).toInt();
    int screenH = screenList.at( 3 ).toInt();

    //use qt to create the real screen space available (the space that corresponse to the identity matrix

    QRectF virtualScreen = QRect( 0, 0, 0, 0 );

    int num = QApplication::desktop()->numScreens();

    for( int i = 0; i < num; i++ ) {
        QRect screen = QApplication::desktop()->screenGeometry( i );

        virtualScreen = virtualScreen.united( screen );
    }

    // and now the values of the new matrix

    qreal w = ( qreal )screenW / ( qreal )virtualScreen.width();
    qreal h = ( qreal )screenH / ( qreal )virtualScreen.height();

    qreal offsetX = ( qreal )screenX / ( qreal )virtualScreen.width();
    qreal offsetY = ( qreal )screenY / ( qreal )virtualScreen.height();

    kDebug() << "Apply Coordinate Transformation Matrix";
    kDebug() << w << "0" << offsetX;
    kDebug() << "0" << h << offsetY;
    kDebug() << "0" << "0" << "1";

    // now set the new matrix via xinput calls
    // this part is taken from the xsetwacom.c file
    int ndevices;
    XDevice *dev = NULL;
    Display *dpy = QX11Info::display();

    XDeviceInfo *info = XListInputDevices( dpy, &ndevices );
    for( int i = 0; i < ndevices; i++ ) {
        if( info[i].name == device.toLatin1() ) {
            dev = XOpenDevice( dpy, info[i].id );
            break;
        }
    }

    Atom matrix_prop = XInternAtom( dpy, "Coordinate Transformation Matrix", True );
    Atom type;
    int format;
    unsigned long nitems, bytes_after;
    float *data;
    long matrix[9] = {0};
    int i;

    if( !matrix_prop ) {
        kError() << "mapTabletToScreen :: Server does not support transformation";
        return;
    }

    /* XI1 expects 32 bit properties (including float) as long,
     * regardless of architecture */
    float fmatrix[9] = { 1, 0, 0,
                         0, 1, 0,
                         0, 0, 1
                       };
    fmatrix[2] = offsetX;
    fmatrix[5] = offsetY;
    fmatrix[0] = w;
    fmatrix[4] = h;

    for( i = 0; i < sizeof( matrix ) / sizeof( matrix[0] ); i++ ) {
        *( float * )( matrix + i ) = fmatrix[i];
    }

    XGetDeviceProperty( dpy, dev, matrix_prop, 0, 9, False,
                        AnyPropertyType, &type, &format, &nitems,
                        &bytes_after, ( unsigned char ** )&data );

    if( format != 32 || type != XInternAtom( dpy, "FLOAT", True ) ) {
        return;
    }

    XChangeDeviceProperty( dpy, dev, matrix_prop, type, format,
                           PropModeReplace, ( unsigned char * )matrix, 9 );
    XFree( data );
    XFlush( dpy );
    XFreeDeviceList( info );
    XCloseDevice( QX11Info::display(), dev );

}

void WacomInterface::setCursorSettings( const QString &device, KConfigGroup *gtprofile )
{
    int ndevices;
    XDevice *dev = NULL;
    Display *dpy = QX11Info::display();

    XDeviceInfo *info = XListInputDevices( dpy, &ndevices );
    for( int i = 0; i < ndevices; i++ ) {
        if( info[i].name == device.toLatin1() ) {
            dev = XOpenDevice( dpy, info[i].id );
            break;
        }
    }

    Atom constDecel_prop = XInternAtom( dpy, "Device Accel Constant Deceleration", True );
    Atom adaptiveDecel_prop = XInternAtom( dpy, "Device Accel Adaptive Deceleration", True );
    Atom velocityScaling_prop = XInternAtom( dpy, "Device Accel Velocity Scaling", True );
    Atom type;
    int format;
    unsigned long nitems, bytes_after;
    float *data;

    if( !constDecel_prop ||  !adaptiveDecel_prop || !velocityScaling_prop) {
        kError() << "mapTabletToScreen :: No properties on device " << device << "to change cursor speed";
        return;
    }

    // set Constant Deceleration
    XGetDeviceProperty( dpy, dev, constDecel_prop, 0, 9, False,
                        AnyPropertyType, &type, &format, &nitems,
                        &bytes_after, ( unsigned char ** )&data );

    if( format != 32 || type != XInternAtom( dpy, "FLOAT", True ) ) {
        return;
    }

    QString cursorSpeed = gtprofile->readEntry("ConstantDeceleration","1.0");

    kDebug() << "setCursorSettings :: set ConstantDeceleration to " << cursorSpeed;

    XChangeDeviceProperty( dpy, dev, constDecel_prop, type, format,
                           PropModeReplace,
                           (unsigned char *)cursorSpeed.toLatin1().data(),
                           nitems );


    // set Adaptive Deceleration
    XGetDeviceProperty( dpy, dev, adaptiveDecel_prop, 0, 9, False,
                        AnyPropertyType, &type, &format, &nitems,
                        &bytes_after, ( unsigned char ** )&data );

    if( format != 32 || type != XInternAtom( dpy, "FLOAT", True ) ) {
        return;
    }

    cursorSpeed = gtprofile->readEntry("AdaptiveDeceleration","1.0");
    kDebug() << "setCursorSettings :: set AdaptiveDeceleration to " << cursorSpeed;

    XChangeDeviceProperty( dpy, dev, adaptiveDecel_prop, type, format,
                           PropModeReplace,
                           (unsigned char *)cursorSpeed.toLatin1().data(),
                           nitems );

    // set Velocity Scaling
    XGetDeviceProperty( dpy, dev, velocityScaling_prop, 0, 9, False,
                        AnyPropertyType, &type, &format, &nitems,
                        &bytes_after, ( unsigned char ** )&data );

    if( format != 32 || type != XInternAtom( dpy, "FLOAT", True ) ) {
        return;
    }

    cursorSpeed = gtprofile->readEntry("VelocityScaling","1.0");
    kDebug() << "setCursorSettings :: set VelocityScaling to " << cursorSpeed.toLatin1().data();

    XChangeDeviceProperty( dpy, dev, velocityScaling_prop, type, format,
                           PropModeReplace,
                           (unsigned char *)cursorSpeed.toLatin1().data(),
                           nitems );
    XFree( data );
    XFlush( dpy );
    XFreeDeviceList( info );
    XCloseDevice( QX11Info::display(), dev );
}
