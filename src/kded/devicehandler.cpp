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

#include "debug.h"

#include "devicehandler.h"
#include "wacomdeviceadaptor.h"
#include "deviceinterface.h"
#include "wacominterface.h"

// common includes
#include "devicedatabase.h"
#include "x11utils.h"

//Qt includes
#include <QtDBus/QDBusArgument>


namespace Wacom {
/**
  * Private class for the DeviceHandler d-pointer.
  */
class DeviceHandlerPrivate {
public:
    DeviceDatabase        deviceDatabase;
    DeviceInformation     deviceInformation;
    DeviceInterface      *curDevice;         /**< Handler for the current device to get/set its configuration */
    bool                  isDeviceAvailable; /**< Is a tabled device connected or not? */
    QMap<QString,QString> buttonMapping;     /**< Map the hardwarebuttons 1-X to its kernel numbering scheme
                                                  @see http://sourceforge.net/mailarchive/message.php?msg_id=27512095 */
};
}

QDBusArgument &operator<<( QDBusArgument &argument, const Wacom::DeviceInformation &mystruct )
{
    argument.beginStructure();
    argument << mystruct.companyID << mystruct.deviceID << mystruct.companyName << mystruct.deviceName << mystruct.deviceModel << mystruct.deviceList << mystruct.padName << mystruct.stylusName << mystruct.eraserName << mystruct.cursorName << mystruct.touchName << mystruct.isDeviceAvailable << mystruct.hasPadButtons;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>( const QDBusArgument &argument, Wacom::DeviceInformation &mystruct )
{
    argument.beginStructure();
    argument >> mystruct.companyID >> mystruct.deviceID >> mystruct.companyName
             >> mystruct.deviceName >> mystruct.deviceModel >> mystruct.deviceList
             >> mystruct.padName >> mystruct.stylusName >> mystruct.eraserName
             >> mystruct.cursorName >> mystruct.touchName >> mystruct.isDeviceAvailable >> mystruct.hasPadButtons;
    argument.endStructure();
    return argument;
}

using namespace Wacom;

DeviceHandler::DeviceHandler()
    : d_ptr( new DeviceHandlerPrivate )
{
    Q_D( DeviceHandler );
    d->curDevice = 0;
    d->isDeviceAvailable = false;

    qDBusRegisterMetaType<Wacom::DeviceInformation>();
    qDBusRegisterMetaType< QList<Wacom::DeviceInformation> >();
}

DeviceHandler::~DeviceHandler()
{
    delete this->d_ptr->curDevice;
    delete this->d_ptr;
}

bool DeviceHandler::detectTablet()
{
    Q_D( DeviceHandler );

    DeviceInformation devinfo;

    if (!X11Utils::findTabletDevice(devinfo)) {
        kDebug() << "no input devices (pad/stylus/eraser/cursor/touch) found via xinput";
        return false;
    }

    kDebug() << "XInput found a device! ::" << devinfo.deviceID;

    if (!d->deviceDatabase.lookupDevice(devinfo, devinfo.deviceID)) {
        kDebug() << "Could not find device in database: " << devinfo.deviceID;
        return false;
    }

    d->deviceInformation  = devinfo;
    
    // lookup button mapping
    d->deviceDatabase.lookupButtonMapping(d->buttonMapping, devinfo.companyID, devinfo.deviceID);

    // set device backend
    selectDeviceBackend( d->deviceDatabase.lookupBackend(devinfo.companyID) );

    // \0/
    d->isDeviceAvailable = true;

    return true;
}

void DeviceHandler::clearDeviceInformation()
{
    Q_D( DeviceHandler );

    DeviceInformation empty;
    
    d->isDeviceAvailable = false;
    d->deviceInformation = empty;
    
    delete d->curDevice;
    d->curDevice = NULL;
    
    d->buttonMapping.clear();
}

void DeviceHandler::selectDeviceBackend( const QString &backendName )
{
    Q_D( DeviceHandler );
    //@TODO add switch statement to handle other backends too
    if( backendName == QLatin1String( "wacom-tools" ) ) {
        d->curDevice = new WacomInterface();
        d->curDevice->setButtonMapping(d->buttonMapping);
    }

    if( !d->curDevice ) {
        kError() << "unknown device backend!" << backendName;
    }
}

DeviceInformation DeviceHandler::getAllInformation() const
{
    Q_D( const DeviceHandler );
    return d->deviceInformation;
}

bool DeviceHandler::isDeviceAvailable() const
{
    Q_D( const DeviceHandler );
    return d->deviceInformation.isDeviceAvailable;
}

bool DeviceHandler::hasPadButtons() const
{
    Q_D( const DeviceHandler );
    return d->deviceInformation.hasPadButtons;
}

QString DeviceHandler::deviceId() const
{
    Q_D( const DeviceHandler );
    return d->deviceInformation.deviceID;
}

QString DeviceHandler::companyId() const
{
    Q_D( const DeviceHandler );
    return d->deviceInformation.companyID;
}

QString DeviceHandler::companyName() const
{
    Q_D( const DeviceHandler );
    return d->deviceInformation.companyName;
}

QString DeviceHandler::deviceName() const
{
    Q_D( const DeviceHandler );
    return d->deviceInformation.deviceName;
}

QString DeviceHandler::deviceModel() const
{
    Q_D( const DeviceHandler );
    return d->deviceInformation.deviceModel;
}

QStringList DeviceHandler::deviceList() const
{
    Q_D( const DeviceHandler );
    return d->deviceInformation.deviceList;
}

QString DeviceHandler::padName() const
{
    Q_D( const DeviceHandler );
    // if no pad is present, use stylus name as alternative way
    // fixes some problems with serial TabletPC that do not have a pad as such but still
    // can handle pad rotations and such when applied to the stylus settings
    if( d->deviceInformation.padName.isEmpty() ) {
        return d->deviceInformation.cursorName;
    }
    else {
        return d->deviceInformation.padName;
    }
}

QString DeviceHandler::stylusName() const
{
    Q_D( const DeviceHandler );
    return d->deviceInformation.stylusName;
}

QString DeviceHandler::eraserName() const
{
    Q_D( const DeviceHandler );
    return d->deviceInformation.eraserName;
}

QString DeviceHandler::cursorName() const
{
    Q_D( const DeviceHandler );
    return d->deviceInformation.cursorName;
}

QString DeviceHandler::touchName() const
{
    Q_D( const DeviceHandler );
    return d->deviceInformation.touchName;
}

QString DeviceHandler::name( const QString &name ) const
{
    Q_D( const DeviceHandler );
    if( name.contains( QLatin1String( "pad" ) ) ) {
        return d->deviceInformation.padName;
    }
    if( name.contains( QLatin1String( "stylus" ) ) ) {
        return d->deviceInformation.stylusName;
    }
    if( name.contains( QLatin1String( "eraser" ) ) ) {
        return d->deviceInformation.eraserName;
    }
    if( name.contains( QLatin1String( "cursor" ) ) ) {
        return d->deviceInformation.cursorName;
    }
    if( name.contains( QLatin1String( "touch" ) ) ) {
        return d->deviceInformation.touchName;
    }

    return QString();
}

void DeviceHandler::applyProfile( const TabletProfile& gtprofile )
{
    Q_D( DeviceHandler );

    if( !d->curDevice ) {
        return;
    }

    if( !d->deviceInformation.padName.isEmpty() ) {
        d->curDevice->applyProfile( d->deviceInformation.padName, QLatin1String( "pad" ), gtprofile );
    }
    if( !d->deviceInformation.stylusName.isEmpty() ) {
        d->curDevice->applyProfile( d->deviceInformation.stylusName, QLatin1String( "stylus" ), gtprofile );
    }
    if( !d->deviceInformation.eraserName.isEmpty() ) {
        d->curDevice->applyProfile( d->deviceInformation.eraserName, QLatin1String( "eraser" ), gtprofile );
    }
    if( !d->deviceInformation.touchName.isEmpty() ) {
        d->curDevice->applyProfile( d->deviceInformation.touchName, QLatin1String( "touch" ), gtprofile );
    }
    if( !d->deviceInformation.cursorName.isEmpty() ) {
        d->curDevice->applyProfile( d->deviceInformation.cursorName, QLatin1String( "cursor" ), gtprofile );
    }
}

void DeviceHandler::setConfiguration( const QString &device, const QString &param, const QString &value )
{
    Q_D( DeviceHandler );

    if( !d->curDevice ) {
        return;
    }

    const Property* property = Property::find(param);

    if (property == NULL) {
        kError() << QString::fromLatin1("Can not set invalid property '%1' to '%2'!").arg(param).arg(value);
        return;
    }

    d->curDevice->setConfiguration( device, *property, value );
}

QString DeviceHandler::getConfiguration( const QString &device, const QString &param ) const
{
    Q_D( const DeviceHandler );

    if( !d->curDevice ) {
        return QString();
    }

    const Property* property = Property::find(param);

    if (property == NULL) {
        kError() << QString::fromLatin1("Can not get invalid property '%1'!").arg(param);
        return QString();
    }

    return d->curDevice->getConfiguration( device, *property );
}

QString DeviceHandler::getDefaultConfiguration( const QString &device, const QString &param ) const
{
    Q_D( const DeviceHandler );

    if( !d->curDevice ) {
        return QString();
    }

    const Property* property = Property::find(param);

    if (property == NULL) {
        kError() << QString::fromLatin1("Can not get default value of invalid property '%1'!").arg(param);
        return QString();
    }

    return d->curDevice->getDefaultConfiguration( device, *property );
}

void DeviceHandler::toggleTouch( )
{
    Q_D( DeviceHandler );

    if( !d->curDevice || d->deviceInformation.touchName.isEmpty() ) {
        return;
    }

    d->curDevice->toggleTouch(d->deviceInformation.touchName);
}

void DeviceHandler::togglePenMode( )
{
    Q_D( DeviceHandler );

    if( !d->curDevice ) {
        return;
    }

    if(!d->deviceInformation.stylusName.isEmpty()) {
        d->curDevice->togglePenMode(d->deviceInformation.stylusName );
    }

    if(!d->deviceInformation.eraserName.isEmpty()) {
        d->curDevice->togglePenMode(d->deviceInformation.eraserName );
    }
}
