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

#include "kcmwacomtablet.h"
#include "kcmwacomtabletwidget.h"
#include "../wacomtablet-version.h"

#include "aboutdata.h"
#include "profilemanagement.h"
#include "dbustabletinterface.h"

// KDE includes
#include <KPluginFactory>
#include <KAboutData>
#include <KLocalizedString>

//Qt includes
#include <QVBoxLayout>

using namespace Wacom;

K_PLUGIN_CLASS_WITH_JSON(KCMWacomTablet, "kcm_wacomtablet.json")

KCMWacomTablet::KCMWacomTablet(QWidget *parent, const QVariantList & args)
        : KCModule(parent, args)
        , m_changed(false)
{
    initUi();
}


KCMWacomTablet::~KCMWacomTablet()
{
    if (m_layout) {
        delete m_layout;
    }

    if (m_tabletWidget) {
        delete m_tabletWidget;
    }

    // This makes sure all global shortcuts are restored by our daemon.
    // Unfortunately KAction disables all global shortcuts on destruction.
    // There is no way to stop it from doing that so we have to use this
    // workaround and reload the current profile here.
    //BUG restore global shortcuts on exit
    //DBusTabletInterface::instance().setProfile( d->tabletId, , ProfileManagement::instance().profileName() );
}


void KCMWacomTablet::initUi()
{
    // about data will be deleted by KCModule
    AboutData *about = new AboutData(QLatin1String("kcm_wacomtablet"), i18n("Graphic Tablet Configuration"),
                                     QLatin1String(WACOMTABLET_VERSION_STRING), i18n("A configurator for graphic tablets"),
                                     i18n("In this module you can configure your Wacom tablet profiles"));

    // setup kcm module
    setAboutData(about);
    setButtons(Apply | Help);

    // setup module ui
    m_tabletWidget = new KCMWacomTabletWidget(this);
    m_layout       = new QVBoxLayout(this);
    m_layout->setContentsMargins({});
    m_layout->addWidget(m_tabletWidget);

    // connect signals
    connect(m_tabletWidget, SIGNAL(changed(bool)), SIGNAL(changed(bool)));
}

void KCMWacomTablet::load()
{
    if (m_tabletWidget) {
        m_tabletWidget->reloadProfile();
    }

    emit changed(false);
}

void KCMWacomTablet::save()
{
    if (m_tabletWidget) {
        m_tabletWidget->saveProfile();
    }

    emit changed(false);
}

#include "kcmwacomtablet.moc"
