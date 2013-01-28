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

#include "dbustabletinterface.h"

#include "debug.h"
#include "dbustabletservice.h"
#include "devicetype.h"
#include "property.h"
#include "tabletinfo.h"
#include "wacomadaptor.h"

#include <QtDBus/QDBusArgument>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusMetaType>

using namespace Wacom;

namespace Wacom
{
    class DBusTabletServicePrivate
    {
        public:
            WacomAdaptor*           wacomAdaptor;
            TabletInformation       tabletInformation;
            TabletHandlerInterface* tabletHandler;
            QString                 currentProfile;
    }; // CLASS
} // NAMESPACE

DBusTabletService::DBusTabletService(TabletHandlerInterface& tabletHandler)
    : QObject(), d_ptr(new DBusTabletServicePrivate)
{
    Q_D ( DBusTabletService );

    d->tabletHandler = &tabletHandler;
    d->tabletInformation.setAvailable(false);

    DBusTabletInterface::registerMetaTypes();

    d->wacomAdaptor = new WacomAdaptor( this );
    QDBusConnection::sessionBus().registerObject( QLatin1String( "/Tablet" ), this );
    QDBusConnection::sessionBus().registerService( QLatin1String( "org.kde.Wacom" ) );
}


DBusTabletService::~DBusTabletService()
{
    QDBusConnection::sessionBus().unregisterService( QLatin1String( "org.kde.Wacom" ) );
    QDBusConnection::sessionBus().unregisterObject( QLatin1String( "/Tablet" ));
    delete d_ptr->wacomAdaptor;

    delete d_ptr;
}



const QStringList DBusTabletService::getDeviceList() const
{
    Q_D ( const DBusTabletService );
    return d->tabletInformation.getDeviceList();
}



const QString& DBusTabletService::getDeviceName(const QString& device) const
{
    Q_D ( const DBusTabletService );

    static const QString unknown;

    const DeviceType *type = DeviceType::find(device);

    if (type == NULL) {
        kError() << QString::fromLatin1("Unsupported device type '%1'!").arg(device);
        return unknown;
    }

    return d->tabletInformation.getDeviceName(*type);
}



const QString& DBusTabletService::getInformation(const QString& info) const
{
    Q_D ( const DBusTabletService );

    static const QString unknown;

    const TabletInfo* devinfo = TabletInfo::find(info);

    if (devinfo == NULL) {
        kError() << QString::fromLatin1("Can not get unsupported tablet information '%1'!").arg(info);
        return unknown;
    }

    return d->tabletInformation.get(*devinfo);
}



QString DBusTabletService::getProfile() const
{
    Q_D ( const DBusTabletService );
    return d->currentProfile;
}



QString DBusTabletService::getProperty(const QString& deviceType, const QString& property) const
{
    Q_D ( const DBusTabletService );

    const DeviceType* type = DeviceType::find(deviceType);

    if (type == NULL) {
        kError() << QString::fromLatin1("Can not get property '%1' from invalid device '%2'!").arg(property).arg(deviceType);
        return QString();
    }

    const Property* prop = Property::find(property);

    if (prop == NULL) {
        kError() << QString::fromLatin1("Can not get invalid property '%1' from device '%2'!").arg(property).arg(deviceType);
        return QString();
    }

    return d->tabletHandler->getProperty(*type, *prop);
}



bool DBusTabletService::hasPadButtons() const
{
    Q_D ( const DBusTabletService );
    return d->tabletInformation.hasButtons();
}



bool DBusTabletService::isAvailable() const
{
    Q_D ( const DBusTabletService );
    return d->tabletInformation.isAvailable();
}



QStringList DBusTabletService::listProfiles()
{
    Q_D ( const DBusTabletService );
    return d->tabletHandler->listProfiles();
}



void DBusTabletService::setProfile(const QString& profile)
{
    Q_D ( DBusTabletService );
    d->tabletHandler->setProfile(profile);
}



void DBusTabletService::setProperty(const QString& deviceType, const QString& property, const QString& value)
{
    Q_D ( DBusTabletService );

    const DeviceType* type = DeviceType::find(deviceType);

    if (type == NULL) {
        kError() << QString::fromLatin1("Can not set property '%1' on invalid device '%2' to '%3'!").arg(property).arg(deviceType).arg(value);
        return;
    }

    const Property* prop = Property::find(property);

    if (prop == NULL) {
        kError() << QString::fromLatin1("Can not set invalid property '%1' on device '%2' to '%3'!").arg(property).arg(deviceType).arg(value);
        return;
    }

    d->tabletHandler->setProperty(*type, *prop, value);
}



void DBusTabletService::onProfileChanged(const QString& profile)
{
    Q_D ( DBusTabletService );

    d->currentProfile = profile;

    emit profileChanged(profile);
}



void DBusTabletService::onTabletAdded(const TabletInformation& info)
{
    Q_D ( DBusTabletService );

    d->tabletInformation = info;
    d->tabletInformation.setAvailable(true);

    emit tabletAdded();
}



void DBusTabletService::onTabletRemoved()
{
    Q_D ( DBusTabletService );
    TabletInformation empty;

    d->currentProfile    = QString();
    d->tabletInformation = empty;
    d->tabletInformation.setAvailable(false);

    emit tabletRemoved();
}
