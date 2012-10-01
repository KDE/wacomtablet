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
#include <QtGui/QGraphicsSceneContextMenuEvent>

using namespace Wacom;

static const char DEFAULT_ICON_NAME[] = "input-tablet";

K_EXPORT_PLASMA_APPLET(tabletsettings, WacomTabletSettings)

WacomTabletSettings::WacomTabletSettings(QObject *parent, const QVariantList &args)
        : Plasma::PopupApplet(parent, args),
        m_applet(0),
        m_settingsWidget(0)
{
    KGlobal::locale()->insertCatalog( QLatin1String( "wacomtablet" ));

    setBackgroundHints(StandardBackground);
    setAspectRatioMode(Plasma::IgnoreAspectRatio);
}

WacomTabletSettings::~WacomTabletSettings()
{
    delete m_applet;
}

void WacomTabletSettings::init()
{
    setPopupIcon(QLatin1String( DEFAULT_ICON_NAME ));

    if (!m_applet) {
        m_applet = new TabletApplet(this);
    }

    m_watcher = new QDBusServiceWatcher( QLatin1String("org.kde.Wacom"), QDBusConnection::sessionBus(),
                                         QDBusServiceWatcher::WatchForRegistration | QDBusServiceWatcher::WatchForUnregistration, this);

    connect(m_watcher, SIGNAL(serviceRegistered(QString)), m_applet, SLOT(onDBusConnected()));
    connect(m_watcher, SIGNAL(serviceUnregistered(QString)), m_applet, SLOT(onDBusDisconnected()));

    m_applet->onDBusConnected();
}


QGraphicsWidget *WacomTabletSettings::graphicsWidget()
{
    if (!m_applet) {
        m_applet = new TabletApplet(this);
    }

    return m_applet->dialog();
}

void WacomTabletSettings::createConfigurationInterface(KConfigDialog *parent)
{
    m_settingsWidget = new KCModuleProxy(QLatin1String( "kcm_wacomtablet" ));

    parent->addPage(m_settingsWidget, m_settingsWidget->moduleInfo().moduleName(),
                    m_settingsWidget->moduleInfo().icon());

    parent->setButtons(KDialog::Ok | KDialog::Cancel);
    connect(parent, SIGNAL(okClicked()), this, SLOT(configAccepted()));
    connect(parent, SIGNAL(applyClicked()), this, SLOT(configAccepted()));
}

void WacomTabletSettings::configAccepted()
{
    //Save the configurations of the embedded KCMs
    m_settingsWidget->save();
    m_applet->updateProfile();
}

