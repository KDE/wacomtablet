/*
 * Copyright 2012 Alexander Maret-Huskinson <alex@maret.de>
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
#include "dbustabletservice.h"

#include "tabletinfo.h"
#include "deviceinterface.h"
#include "devicetype.h"
#include "dbustabletinterface.h" // required to copy TabletInformation from/to QDBusArgument
#include "deviceprofile.h"
#include "tabletdatabase.h"
#include "mainconfig.h"
#include "profilemanager.h"
#include "tabletprofile.h"
#include "wacomadaptor.h"
#include "x11utils.h"

#include <KDE/KLocalizedString>

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

    // TODO DO WE REALLY NEED THIS?
    //qDBusRegisterMetaType<Wacom::TabletInformation>();
    //qDBusRegisterMetaType< QList<Wacom::TabletInformation> >();

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



const QStringList& DBusTabletService::getDeviceList() const
{
    Q_D ( const DBusTabletService );
    return d->tabletInformation.getDeviceList();
}



const QString& DBusTabletService::getDeviceName(const QString& device) const
{
    Q_D ( const DBusTabletService );
    return d->tabletInformation.getDeviceName(device);
}



TabletInformation DBusTabletService::getInformation() const
{
    Q_D ( const DBusTabletService );
    return d->tabletInformation;
}



const QString& DBusTabletService::getInformation(const QString& info) const
{
    Q_D ( const DBusTabletService );
    return d->tabletInformation.get(info);
}



QString DBusTabletService::getProfile() const
{
    Q_D ( const DBusTabletService );
    return d->currentProfile;
}



QString DBusTabletService::getProperty(const QString& device, const QString& property) const
{
    Q_D ( const DBusTabletService );
    return d->tabletHandler->getProperty(device, property);
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



QStringList DBusTabletService::listProfiles() const
{
    Q_D ( const DBusTabletService );
    return d->tabletHandler->listProfiles();
}



void DBusTabletService::setProfile(const QString& profile)
{
    Q_D ( DBusTabletService );
    d->tabletHandler->setProfile(profile);
}



void DBusTabletService::setProperty(const QString& device, const QString& property, const QString& value)
{
    Q_D ( DBusTabletService );
    d->tabletHandler->setProperty(device, property, value);
}



void DBusTabletService::togglePenMode()
{
    Q_D ( DBusTabletService );
    d->tabletHandler->togglePenMode();
}



void DBusTabletService::toggleTouch()
{
    Q_D ( DBusTabletService );
    d->tabletHandler->toggleTouch();
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

    d->tabletInformation = empty;
    d->tabletInformation.setAvailable(false);

    emit tabletRemoved();
}





