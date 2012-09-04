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
#include "xsetwacomadaptor.h"
#include "x11utils.h"

// stdlib
#include <memory>

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

    std::auto_ptr<XsetwacomAdaptor> xsetwacomAdaptor;

    if (activateButtonMapping) {
        xsetwacomAdaptor = std::auto_ptr<XsetwacomAdaptor>(new XsetwacomAdaptor(device, m_buttonMapping));
    } else {
        xsetwacomAdaptor = std::auto_ptr<XsetwacomAdaptor>(new XsetwacomAdaptor(device));
    }

    if (!xsetwacomAdaptor->setProperty(property, value)) {
        kError() << QString::fromLatin1("Could not set property '%1' to '%2' using xsetwacom!").arg(property.key()).arg(value);
    }
}



QString WacomInterface::getConfiguration( const QString& device, const Property& property ) const
{
    XsetwacomAdaptor xsetwacomAdaptor(device, m_buttonMapping);

    // we might have to do a button mapping (+4), however the mapping table should take care of that
    return xsetwacomAdaptor.getProperty(property);
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
