/*
 * Copyright 2009,2010 Jörg Ehrichs <joerg.ehichs@gmx.de>
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
#include "tabletwidget.h"
#include "../version.h"

#include "aboutdata.h"

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

    m_layout = new QVBoxLayout(this);
    m_layout->setMargin(0);

    m_about = new AboutData("kcm_wacomtablet", ki18n("Graphic Tablet Configuration"),
                            kcm_version, ki18n("A configurator for graphic tablets"),
                            ki18n("In this module you can configure your Wacom tablet profiles"));
    setAboutData(m_about);
    setButtons(Apply | Help);

    initModule();
}


KCMWacomTablet::~KCMWacomTablet()
{
    delete m_about;
}


void KCMWacomTablet::initModule()
{
    m_tabletWidget = new TabletWidget(this);
    m_layout->addWidget(m_tabletWidget);

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
