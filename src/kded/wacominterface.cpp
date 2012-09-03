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
#include "deviceprofile.h"
#include "deviceprofilexsetwacomadaptor.h"
#include "property.h"
#include "x11utils.h"

//KDE includes
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

using namespace Wacom;

WacomInterface::WacomInterface() : DeviceInterface() {}

WacomInterface::~WacomInterface() {}

void WacomInterface::applyProfile( const QString& device, const QString& section, const TabletProfile& gtprofile )
{
    DeviceProfile                 deviceProfile = gtprofile.getDevice(section);
    DeviceProfileXsetwacomAdaptor profileAdapter(deviceProfile);

    bool useButtonMapping = false;
    if(section == QLatin1String("pad")) {
        useButtonMapping = true;
    }

    // get all xsetwacom properties supported by the device profile
    // this will also make sure that they are set in the correct order
    foreach( const XsetwacomProperty& property, profileAdapter.getXsetwacomProperties()) {
        setConfiguration( device, property.id(), profileAdapter.getXsetwacomProperty( property ), useButtonMapping);
    }

    //this will invert touch gesture scrolling (up/down)
    if ( deviceProfile.getInvertScroll() == QLatin1String( "true" ) ) {
        setConfiguration(device, Property::Button4, QLatin1String("5"));
        setConfiguration(device, Property::Button5, QLatin1String("4"));
    }
    else {
        setConfiguration(device, Property::Button4, QLatin1String("4"));
        setConfiguration(device, Property::Button5, QLatin1String("5"));
    }
    
    // apply the MapToOutput at the end.
    // this ensures we rotated the device beforehand
    mapTabletToScreen( device, deviceProfile.getScreenSpace() );
}

void WacomInterface::setConfiguration( const QString& device, const Property& property, const QString& value, bool activateButtonMapping )
{
    if( value.isEmpty() ) {
        return;
    }

    const XsetwacomProperty* xsetproperty = XsetwacomProperty::map(property);

    if (xsetproperty == NULL) {
        kError() << QString::fromLatin1("Can not set unsupported property '%1' to '%2' using xsetwacom!").arg(property.key()).arg(value);
        return;
    }

    QString modifiedParam = xsetproperty->key();

    // Here we translate the hardware button number which go from 1-X
    // to the real numebrs as used by the kernel driver
    // the mapping is read from the wacom_devicelist as hwbutton1=1 etc entries
    // this is necessary to cope with some changes where for example the Pen& Touch devices have hw Button 2 as kernel Button 9 etc
    // also often button 4 is button 8 because 4-7 are used for scrolling
    if(activateButtonMapping) {
        QRegExp rx(QLatin1String( "^Button\\s*([0-9]+)$" ));
        int pos = 0;

        if ((pos = rx.indexIn(modifiedParam, pos)) != -1) {
            QString hwButtonNumber = rx.cap(1);
            QString kernelButtonNumber = m_buttonMapping.value(hwButtonNumber);
            if(!kernelButtonNumber.isEmpty())
                modifiedParam = QString(QLatin1String("Button%1")).arg(kernelButtonNumber);
        }
    }

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

    kDebug() << cmd;

    if( !errorOutput.isEmpty() ) {
        kDebug() << errorOutput;
    }
}

QString WacomInterface::getConfiguration( const QString& device, const Property& property ) const
{
    const XsetwacomProperty* xsetproperty = XsetwacomProperty::map(property);

    if (xsetproperty == NULL) {
        kError() << QString::fromLatin1("Can not get unsupported property '%1' using xsetwacom!").arg(property.key());
        return QString();
    }

    QString modifiedParam = xsetproperty->key();

    // small *hack* to cope with linux button settings
    // button 4,5,6,7 are not buttons but scrolling
    // hus button 4 is in reality button 8
    QRegExp rx(QLatin1String( "^Button([0-9])" ));
    int pos = 0;

    while ((pos = rx.indexIn(modifiedParam, pos)) != -1) {
        QString button = rx.cap(1);
        int buttonNumber = button.toInt();
        if(buttonNumber >= 4) {
            buttonNumber += 4;
            modifiedParam = QString(QLatin1String("Button%1")).arg(buttonNumber);
        }
    }

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

QString WacomInterface::getDefaultConfiguration( const QString &device, const Property &property ) const
{
    return getConfiguration(device, property);
}

void WacomInterface::toggleTouch( const QString &touchDevice )
{
    QString touchMode = getConfiguration( touchDevice, Property::Touch );

    if( touchMode.compare( QLatin1String( "off" ), Qt::CaseInsensitive) == 0 ) {
        setConfiguration( touchDevice, Property::Touch, QLatin1String( "on" ) );
    }
    else {
        setConfiguration( touchDevice, Property::Touch, QLatin1String( "off" ) );
    }
}

void WacomInterface::togglePenMode( const QString &device )
{
    QString touchMode = getConfiguration( device, Property::Mode );

    if( touchMode.compare( QLatin1String( "Absolute" ), Qt::CaseInsensitive ) == 0 ) {
        setConfiguration( device, Property::Mode, QLatin1String( "Relative" ) );
    }
    else {
        setConfiguration( device, Property::Mode, QLatin1String( "Absolute" ) );
    }
}

void WacomInterface::mapTabletToScreen( const QString &device, const QString &screenArea )
{
    // what we need is the Coordinate Transformation Matrix
    // in the normal case where the whole screen is used we end up with a 3x3 identity matrix

    //in our case we want to change that
    // | w  0  offsetX |
    // | 0  h  offsetY |
    // | 0  0     1    |

    QStringList screenList = screenArea.split( QLatin1String( " " ) );

    if( screenList.isEmpty() || screenList.size() != 4 ) {
        kError() << "mapTabletToScreen :: can't parse ScreenSpace entry '" << screenArea << "' => device:" << device;
        return;
    }

    // read in what the user wants to use
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
    kDebug() << "virtual screen" << virtualScreen;

    // and now the values of the new matrix
    qreal w = ( qreal )screenW / ( qreal )virtualScreen.width();
    qreal h = ( qreal )screenH / ( qreal )virtualScreen.height();

    qreal offsetX = ( qreal )screenX / ( qreal )virtualScreen.width();
    qreal offsetY = ( qreal )screenY / ( qreal )virtualScreen.height();

    kDebug() << "Apply Coordinate Transformation Matrix";
    kDebug() << w << "0" << offsetX;
    kDebug() << "0" << h << offsetY;
    kDebug() << "0" << "0" << "1";

    X11Utils::mapTabletToScreen(device, offsetX, offsetY, w, h);
}
