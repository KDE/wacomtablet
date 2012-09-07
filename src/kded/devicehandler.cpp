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
#include "dbusdeviceinterface.h" // required to copy DeviceInformation from/to QDBusArgument
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
    DeviceInterface      *currentDevice;     //!< Handler for the current device to get/set its configuration.
    bool                  isDeviceAvailable; //!< Is a tabled device connected or not?
    QMap<QString,QString> buttonMapping;     /**< Map the hardwarebuttons 1-X to its kernel numbering scheme
                                                  @see http://sourceforge.net/mailarchive/message.php?msg_id=27512095 */
};
}

using namespace Wacom;

DeviceHandler::DeviceHandler()
    : d_ptr( new DeviceHandlerPrivate )
{
    Q_D( DeviceHandler );
    d->currentDevice = 0;
    d->isDeviceAvailable = false;

    qDBusRegisterMetaType<Wacom::DeviceInformation>();
    qDBusRegisterMetaType< QList<Wacom::DeviceInformation> >();
}


DeviceHandler::~DeviceHandler()
{
    delete this->d_ptr->currentDevice;
    delete this->d_ptr;
}



void DeviceHandler::applyProfile( const TabletProfile& gtprofile )
{
    Q_D( DeviceHandler );

    if( !d->currentDevice ) {
        return;
    }

    if( !d->deviceInformation.padName.isEmpty() ) {
        d->currentDevice->applyProfile( d->deviceInformation.padName, QLatin1String( "pad" ), gtprofile );
    }
    if( !d->deviceInformation.stylusName.isEmpty() ) {
        d->currentDevice->applyProfile( d->deviceInformation.stylusName, QLatin1String( "stylus" ), gtprofile );
    }
    if( !d->deviceInformation.eraserName.isEmpty() ) {
        d->currentDevice->applyProfile( d->deviceInformation.eraserName, QLatin1String( "eraser" ), gtprofile );
    }
    if( !d->deviceInformation.touchName.isEmpty() ) {
        d->currentDevice->applyProfile( d->deviceInformation.touchName, QLatin1String( "touch" ), gtprofile );
    }
    if( !d->deviceInformation.cursorName.isEmpty() ) {
        d->currentDevice->applyProfile( d->deviceInformation.cursorName, QLatin1String( "cursor" ), gtprofile );
    }
}



void DeviceHandler::clearDeviceInformation()
{
    Q_D( DeviceHandler );

    DeviceInformation empty;

    d->isDeviceAvailable = false;
    d->deviceInformation = empty;

    delete d->currentDevice;
    d->currentDevice = NULL;

    d->buttonMapping.clear();
}



bool DeviceHandler::detectTablet()
{
    Q_D( DeviceHandler );

    DeviceInformation devinfo;

    if (!X11Utils::findTabletDevice(devinfo)) {
        kDebug() << "no input devices (pad/stylus/eraser/cursor/touch) found via xinput";
        return false;
    }

    kDebug() << "XInput found a device! ::" << devinfo.tabletId;

    if (!d->deviceDatabase.lookupDevice(devinfo, devinfo.tabletId)) {
        kDebug() << "Could not find device in database: " << devinfo.tabletId;
        return false;
    }

    d->deviceInformation  = devinfo;

    // lookup button mapping
    d->deviceDatabase.lookupButtonMapping(d->buttonMapping, devinfo.companyId, devinfo.tabletId);

    // set device backend
    selectDeviceBackend( d->deviceDatabase.lookupBackend(devinfo.companyId) );

    // \0/
    d->isDeviceAvailable = true;

    return true;
}



QString DeviceHandler::getConfiguration(const QString& device, const Property& property) const
{
    Q_D( const DeviceHandler );
    return d->currentDevice->getConfiguration(device, property);
}



const QString& DeviceHandler::getDeviceName(const DeviceType& device) const
{
    Q_D( const DeviceHandler );
    return d->deviceInformation.getDeviceName(device);
}



const QString& DeviceHandler::getInformation(const DeviceInfo& info) const
{
    Q_D( const DeviceHandler );
    return d->deviceInformation.get(info);
}



void DeviceHandler::setConfiguration(const QString& device, const Property& property, const QString& value)
{
    Q_D( DeviceHandler );
    d->currentDevice->setConfiguration(device, property, value);
}



void DeviceHandler::selectDeviceBackend( const QString &backendName )
{
    Q_D( DeviceHandler );
    //@TODO add switch statement to handle other backends too
    if( backendName == QLatin1String( "wacom-tools" ) ) {
        d->currentDevice = new WacomInterface();
        d->currentDevice->setButtonMapping(d->buttonMapping);
    }

    if( !d->currentDevice ) {
        kError() << "unknown device backend!" << backendName;
    }
}



DeviceInformation DeviceHandler::getAllInformation() const
{
    Q_D( const DeviceHandler );
    return d->deviceInformation;
}



QString DeviceHandler::getConfiguration( const QString &device, const QString &param ) const
{
    Q_D( const DeviceHandler );

    if( !d->currentDevice ) {
        return QString();
    }

    const Property* property = Property::find(param);

    if (property == NULL) {
        kError() << QString::fromLatin1("Can not get invalid property '%1'!").arg(param);
        return QString();
    }

    return d->currentDevice->getConfiguration( device, *property );
}



const QStringList& DeviceHandler::getDeviceList() const
{
    Q_D( const DeviceHandler );
    return d->deviceInformation.getDeviceList();
}



const QString& DeviceHandler::getDeviceName(const QString& device) const
{
    Q_D( const DeviceHandler );
    const DeviceType *type = DeviceType::find(device);

    if (type == NULL) {
        kError() << QString::fromLatin1("Unsupported device type '%1'!").arg(device);
        return d->deviceInformation.unknown;
    }

    return d->deviceInformation.getDeviceName(*type);
}



const QString& DeviceHandler::getInformation(const QString& info) const
{
    Q_D( const DeviceHandler );
    const DeviceInfo *devinfo = DeviceInfo::find(info);

    if (devinfo == NULL) {
        kError() << QString::fromLatin1("Unsupported device info '%1'!").arg(info);
        return d->deviceInformation.unknown;
    }

    return d->deviceInformation.get(*devinfo);
}



bool DeviceHandler::hasPadButtons() const
{
    Q_D( const DeviceHandler );
    return d->deviceInformation.hasButtons();
}



bool DeviceHandler::isDeviceAvailable() const
{
    Q_D( const DeviceHandler );
    return d->deviceInformation.isAvailable();
}



void DeviceHandler::setConfiguration( const QString &device, const QString &param, const QString &value )
{
    Q_D( DeviceHandler );

    if( !d->currentDevice ) {
        return;
    }

    const Property* property = Property::find(param);

    if (property == NULL) {
        kError() << QString::fromLatin1("Can not set invalid property '%1' to '%2'!").arg(param).arg(value);
        return;
    }

    d->currentDevice->setConfiguration( device, *property, value );
}



void DeviceHandler::togglePenMode( )
{
    Q_D( DeviceHandler );

    if( !d->currentDevice ) {
        return;
    }

    if(!d->deviceInformation.stylusName.isEmpty()) {
        d->currentDevice->togglePenMode(d->deviceInformation.stylusName );
    }

    if(!d->deviceInformation.eraserName.isEmpty()) {
        d->currentDevice->togglePenMode(d->deviceInformation.eraserName );
    }
}



void DeviceHandler::toggleTouch( )
{
    Q_D( DeviceHandler );

    if( !d->currentDevice || d->deviceInformation.touchName.isEmpty() ) {
        return;
    }

    d->currentDevice->toggleTouch(d->deviceInformation.touchName);
}

