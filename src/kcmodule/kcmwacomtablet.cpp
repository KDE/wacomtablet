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
#include "../version.h"

#include "aboutdata.h"
#include "profilemanagement.h"
#include "dbustabletinterface.h"

// KDE includes
#include <KDE/KCModuleLoader>
#include <KDE/KPluginFactory>
#include <KDE/KPluginLoader>
#include <KDE/KAboutData>
#include <KDE/KLocalizedString>

//Qt includes
#include <QtGui/QVBoxLayout>

using namespace Wacom;

K_PLUGIN_FACTORY(KCMWacomTabletFactory, registerPlugin<KCMWacomTablet>();)
K_EXPORT_PLUGIN(KCMWacomTabletFactory("kcm_wacomtablet"))

KCMWacomTablet::KCMWacomTablet(QWidget *parent, const QVariantList &)
        : KCModule(KCMWacomTabletFactory::componentData(), parent),
        m_changed(false)
{
    KGlobal::locale()->insertCatalog( QLatin1String( "wacomtablet" ));
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
    DBusTabletInterface::instance().setProfile( ProfileManagement::instance().profileName() );
}


void KCMWacomTablet::initUi()
{
    // about data will be deleted by KCModule
    AboutData* about = new AboutData("kcm_wacomtablet", ki18n("Graphic Tablet Configuration"),
                            kcm_version, ki18n("A configurator for graphic tablets"),
                            ki18n("In this module you can configure your Wacom tablet profiles"));

    // setup kcm module
    setAboutData(about);
    setButtons(Apply | Help);

    // setup module ui
    m_tabletWidget = new KCMWacomTabletWidget(this);
    m_layout       = new QVBoxLayout(this);
    m_layout->setMargin(0);
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
