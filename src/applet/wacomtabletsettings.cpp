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

#include "wacomtabletsettings.h"
#include "tabletapplet.h"

//KDE includes
#include <KDE/KConfigDialog>
#include <KDE/KGlobal>
#include <KDE/KStandardDirs>
#include <KDE/KCModuleProxy>
#include <KDE/KCModuleInfo>
#include <KDE/KDebug>

//Qt includes
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusServiceWatcher>
#include <QtDBus/QDBusReply>
#include <QGraphicsSceneContextMenuEvent>


namespace Wacom
{
    class WacomTabletSettingsPrivate
    {
        public:
            static const QString DBUS_SERVICE_NAME; // TODO this should be a global definition
            static const QString DEFAULT_ICON_NAME;
            static const QString I18N_CATALOG_NAME; // TODO this should be a global definition
            static const QString KCM_MODULE_NAME;   // TODO this should be a global definition

            ~WacomTabletSettingsPrivate() {
                delete tabletApplet;
                delete settingsWidget;
            }

            QPointer<TabletApplet>  tabletApplet;        //!< The plasma applet widget.
            QPointer<KCModuleProxy> settingsWidget;      //!< A KCM proxy which loads our normal settings widget.
            QDBusServiceWatcher     dbusServiceWatcher;  //!< A D-Bus watcher which monitors the availability of the Tablet D-Bus Service.

    }; // PRIVATE CLASS

    /*
     * Initialize static members.
     */
    const QString WacomTabletSettingsPrivate::DBUS_SERVICE_NAME = QLatin1String("org.kde.Wacom");
    const QString WacomTabletSettingsPrivate::DEFAULT_ICON_NAME = QLatin1String("input-tablet");
    const QString WacomTabletSettingsPrivate::I18N_CATALOG_NAME = QLatin1String("wacomtablet");
    const QString WacomTabletSettingsPrivate::KCM_MODULE_NAME   = QLatin1String("kcm_wacomtablet");

} // NAMESPACE WACOM



using namespace Wacom;

K_EXPORT_PLASMA_APPLET (tabletsettings, WacomTabletSettings)

WacomTabletSettings::WacomTabletSettings(QObject *parent, const QVariantList &args)
        : Plasma::PopupApplet(parent, args),
          d_ptr (new WacomTabletSettingsPrivate)
{
    // init locale
    KGlobal::locale()->insertCatalog (WacomTabletSettingsPrivate::I18N_CATALOG_NAME);

    // init plasma widget
    setBackgroundHints (StandardBackground);
    setAspectRatioMode (Plasma::IgnoreAspectRatio);
    setPopupIcon (WacomTabletSettingsPrivate::DEFAULT_ICON_NAME);
}



WacomTabletSettings::~WacomTabletSettings()
{
    delete this->d_ptr;
}



void WacomTabletSettings::init()
{
    Q_D (WacomTabletSettings);

    // create applet if is does not yet exist
    if (!d->tabletApplet) {
        d->tabletApplet = new TabletApplet(this);
    }

    // init dbus service watcher
    d->dbusServiceWatcher.setParent(this);
    d->dbusServiceWatcher.setWatchedServices (QStringList(WacomTabletSettingsPrivate::DBUS_SERVICE_NAME));
    d->dbusServiceWatcher.setWatchMode (QDBusServiceWatcher::WatchForRegistration | QDBusServiceWatcher::WatchForUnregistration);
    d->dbusServiceWatcher.setConnection (QDBusConnection::sessionBus());

    connect(&(d->dbusServiceWatcher), SIGNAL(serviceRegistered(QString)),   d->tabletApplet, SLOT(onDBusConnected()));
    connect(&(d->dbusServiceWatcher), SIGNAL(serviceUnregistered(QString)), d->tabletApplet, SLOT(onDBusDisconnected()));

    d->tabletApplet->onDBusConnected();
}



QGraphicsWidget *WacomTabletSettings::graphicsWidget()
{
    Q_D (WacomTabletSettings);

    if (!d->tabletApplet) {
        d->tabletApplet = new TabletApplet(this);
    }

    return d->tabletApplet->dialog();
}



void WacomTabletSettings::createConfigurationInterface(KConfigDialog *parent)
{
    Q_D (WacomTabletSettings);

    if (!d->settingsWidget) {
        d->settingsWidget = new KCModuleProxy (WacomTabletSettingsPrivate::KCM_MODULE_NAME);
    }

    parent->addPage(d->settingsWidget, d->settingsWidget->moduleInfo().moduleName(), d->settingsWidget->moduleInfo().icon());
    parent->setButtons(KDialog::Ok | KDialog::Cancel);

    connect(parent, SIGNAL(okClicked()), this, SLOT(configAccepted()));
    connect(parent, SIGNAL(applyClicked()), this, SLOT(configAccepted()));
}



void WacomTabletSettings::configAccepted()
{
    Q_D (WacomTabletSettings);

    //Save the configurations of the embedded KCMs
    d->settingsWidget->save();
    d->tabletApplet->updateProfile();
}

