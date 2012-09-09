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
#include "deviceprofile.h"
#include "property.h"
#include "xinputadaptor.h"
#include "xinputproperty.h"
#include "xsetwacomadaptor.h"
#include "xsetwacomproperty.h"
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

void WacomInterface::applyProfile( const QString& device, const QString& section, const TabletProfile& tabletProfile )
{
    DeviceProfile deviceProfile = tabletProfile.getDevice(section);

    bool useButtonMapping = false;
    if (section == QLatin1String("pad")) {
        useButtonMapping = true;
    }

    // get all properties xsetwacom supports and set them
    // this will also make sure that they are set in the correct order
    foreach (const Property& property, XsetwacomProperty::ids()) {
        setConfiguration (device, property, deviceProfile.getProperty(property), useButtonMapping);
    }

    // this will invert touch gesture scrolling (up/down)
    if ( deviceProfile.getInvertScroll() == QLatin1String( "true" ) ) {
        setConfiguration (device, Property::Button4, QLatin1String("5"));
        setConfiguration (device, Property::Button5, QLatin1String("4"));
    }
    else {
        setConfiguration (device, Property::Button4, QLatin1String("4"));
        setConfiguration (device, Property::Button5, QLatin1String("5"));
    }

    // apply xinput parameters
    foreach (const Property& property, XinputProperty::ids()) {
        setConfiguration (device, property, deviceProfile.getProperty(property), false);
    }
}



void WacomInterface::setConfiguration( const QString& device, const Property& property, const QString& value, bool activateButtonMapping )
{
    if( value.isEmpty() ) {
        return;
    }

    std::auto_ptr<XsetwacomAdaptor> xsetwacomAdaptor;
    XinputAdaptor                   xinputAdaptor(device);

    if (activateButtonMapping) {
        xsetwacomAdaptor = std::auto_ptr<XsetwacomAdaptor>(new XsetwacomAdaptor(device, m_buttonMapping));
    } else {
        xsetwacomAdaptor = std::auto_ptr<XsetwacomAdaptor>(new XsetwacomAdaptor(device));
    }

    bool success = false;

    if (xsetwacomAdaptor->supportsProperty(property)) {
        success = xsetwacomAdaptor->setProperty(property, value);

    } else if (xinputAdaptor.supportsProperty(property)) {
        success = xinputAdaptor.setProperty(property, value);
    }

    if (!success) {
        kError() << QString::fromLatin1("Could not set property '%1' to '%2'!").arg(property.key()).arg(value);
    }
}



QString WacomInterface::getConfiguration( const QString& device, const Property& property ) const
{
    // we might have to do a button mapping (+4), however the mapping table should take care of that
    XsetwacomAdaptor xsetwacomAdaptor(device, m_buttonMapping);
    XinputAdaptor    xinputAdaptor(device);

    if (xinputAdaptor.supportsProperty(property)) {
        return xinputAdaptor.getProperty(property);
    }

    return xsetwacomAdaptor.getProperty(property);
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
