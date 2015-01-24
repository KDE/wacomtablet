/*
 * Copyright (C) 2015 Weng Xuetian <wengxt@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include "wacomtabletengine.h"
#include "wacomtabletservice.h"
#include "dbustabletinterface.h"
#include <QDBusServiceWatcher>

using namespace Wacom;

WacomTabletEngine::WacomTabletEngine(QObject* parent, const QVariantList& args):
    DataEngine(parent, args)
   ,m_source(QLatin1Literal("wacomtablet"))
{
    // init dbus service watcher
    QDBusServiceWatcher* dbusServiceWatcher = new QDBusServiceWatcher(this);
    dbusServiceWatcher->setWatchedServices (QStringList(QLatin1Literal("org.kde.Wacom")));
    dbusServiceWatcher->setWatchMode (QDBusServiceWatcher::WatchForRegistration | QDBusServiceWatcher::WatchForUnregistration);
    dbusServiceWatcher->setConnection (QDBusConnection::sessionBus());

    connect(dbusServiceWatcher, SIGNAL(serviceRegistered(QString)),   this, SLOT(onDBusConnected()));
    connect(dbusServiceWatcher, SIGNAL(serviceUnregistered(QString)), this, SLOT(onDBusDisconnected()));

    onDBusConnected();
}

WacomTabletEngine::~WacomTabletEngine()
{

}

void WacomTabletEngine::onDBusConnected()
{
    DBusTabletInterface::resetInterface();

    if( !DBusTabletInterface::instance().isValid() ) {
        onDBusDisconnected();
        return;
    }

    setData(m_source, QLatin1Literal("serviceAvailable"), true);

    connect( &DBusTabletInterface::instance(), SIGNAL(tabletAdded(QString)),    this, SLOT(onTabletAdded(QString)) );
    connect( &DBusTabletInterface::instance(), SIGNAL(tabletRemoved(QString)),  this, SLOT(onTabletRemoved(QString)) );
    connect( &DBusTabletInterface::instance(), SIGNAL(profileChanged(QString,QString)), this, SLOT(setProfile(QString,QString)) );

    // get list of connected tablets
    QDBusReply<QStringList> connectedTablets = DBusTabletInterface::instance().getTabletList();

    foreach(const QString &tabletId, connectedTablets.value()) {
        onTabletAdded(tabletId);
    }
}

void WacomTabletEngine::onDBusDisconnected()
{
    setData(m_source, QLatin1Literal("serviceAvailable"), false);
    const auto keys = m_tablets.keys();
    for(auto iter = keys.begin(); iter != keys.end(); iter++) {
        onTabletRemoved(*iter);
    }
    m_tablets.clear();
}

void WacomTabletEngine::onTabletAdded(const QString& tabletId)
{
    if (m_tablets.contains(tabletId)) {
        return;
    }
    QDBusReply<QString> deviceName = DBusTabletInterface::instance().getInformation(tabletId, TabletInfo::TabletName.key());
    QDBusReply<QStringList> profileListReply = DBusTabletInterface::instance().listProfiles(tabletId);
    const auto profileList = profileListReply.value();
    const QString sourceName = QString(QLatin1Literal("Tablet%1")).arg(tabletId);
    auto& tabletData = m_tablets[tabletId];
    tabletData.name = deviceName.value();
    tabletData.profiles = profileList;

    QDBusReply<QString> profileName = DBusTabletInterface::instance().getProfile(tabletId);
    int item = profileList.indexOf(profileName.value());
    tabletData.currentProfile = item;

    QDBusReply<QString> stylusMode = DBusTabletInterface::instance().getProperty(tabletId, DeviceType::Stylus.key(), Property::Mode.key());
    QDBusPendingReply< QString > dbusReply = DBusTabletInterface::instance().getDeviceName(tabletId, DeviceType::Touch.key());
    tabletData.hasTouch = (dbusReply.isValid() && !dbusReply.value().isEmpty());

    if (tabletData.hasTouch) {
        QDBusReply<QString> touchMode = DBusTabletInterface::instance().getProperty(tabletId, DeviceType::Touch.key(), Property::Touch.key());
        tabletData.touch = QString( touchMode ).contains( QLatin1String( "on" ));
    } else {
        tabletData.touch = false;
    }

    tabletData.stylusMode = ( QString( stylusMode ).contains( QLatin1String( "absolute" )) || QString( stylusMode ).contains( QLatin1String( "Absolute" )) );
    setData(sourceName, QLatin1Literal("currentProfile"), tabletData.currentProfile);
    setData(sourceName, QLatin1Literal("hasTouch"), tabletData.hasTouch);
    setData(sourceName, QLatin1Literal("touch"), tabletData.touch);
    setData(sourceName, QLatin1Literal("profiles"), tabletData.profiles);
    setData(sourceName, QLatin1Literal("stylusMode"), tabletData.stylusMode);
    setData(sourceName, QLatin1Literal("name"), tabletData.name);
    setData(sourceName, QLatin1Literal("id"), tabletId);

}

void WacomTabletEngine::onTabletRemoved(const QString& tabletId)
{
    const QString sourceName = QString(QLatin1Literal("Tablet%1")).arg(tabletId);
    m_tablets.remove(tabletId);
    removeSource(sourceName);
}

void WacomTabletEngine::setProfile(const QString& tabletId, const QString& profile)
{
    if (!m_tablets.contains(tabletId)) {
        return;
    }
    int item = m_tablets[tabletId].currentProfile = m_tablets[tabletId].profiles.indexOf(profile);
    const QString sourceName = QString(QLatin1Literal("Tablet%1")).arg(tabletId);
    setData(sourceName, QLatin1Literal("currentProfile"), item);
}

Plasma::Service* WacomTabletEngine::serviceForSource(const QString& source)
{
    if (source == m_source) {
        return new WacomTabletService(source, this);
    }

    return Plasma::DataEngine::serviceForSource(source);
}

K_EXPORT_PLASMA_DATAENGINE_WITH_JSON(wacomtablet, WacomTabletEngine, "plasma-dataengine-wacomtablet.json")

#include "wacomtabletengine.moc"
