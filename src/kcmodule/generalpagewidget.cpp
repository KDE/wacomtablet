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

#include "generalpagewidget.h"
#include "ui_generalpagewidget.h"

#include "profilemanagement.h"

// common
#include "tabletinfo.h"
#include "dbustabletinterface.h"

// stdlib
#include <memory>

//KDE includes
#include <KDE/KStandardDirs>
#include <KDE/KIcon>
#include <KDE/KComponentData>
#include <KDE/KShortcutsEditor>
#include <KDE/KActionCollection>
#include <KDE/KAction>

//Qt includes
#include <QtCore/QPointer>
#include <QtCore/QStringList>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>

#include <QDebug>

using namespace Wacom;

namespace Wacom {
/**
  * Private class for the d-pointer.
  */
class GeneralPageWidgetPrivate {
    public:
        ~GeneralPageWidgetPrivate() {
            delete m_actionCollection;
            delete m_shortcutEditor;
        }

        std::auto_ptr<Ui::GeneralPageWidget>  m_ui;                /**< Handler to the generalwidget.ui file */
        QPointer<KActionCollection>       m_actionCollection;
        QPointer<KShortcutsEditor>        m_shortcutEditor;
}; // CLASS
}  // NAMESPACE


GeneralPageWidget::GeneralPageWidget(QWidget *parent)
        : QWidget(parent), d_ptr(new GeneralPageWidgetPrivate)
{
    Q_D( GeneralPageWidget );

    d->m_ui = std::auto_ptr<Ui::GeneralPageWidget>(new Ui::GeneralPageWidget);
    d->m_ui->setupUi(this);

    //if someone adds another action also add it to kded/tabletdeamon.cpp
    d->m_actionCollection = new KActionCollection(this, KComponentData("wacomtablet"));
    d->m_actionCollection->setConfigGlobal(true);

    KAction *action = d->m_actionCollection->addAction(QLatin1String("Toggle touch tool"));
    action->setText( i18nc( "@action", "Enable/Disable the Touch Tool" ) );
    action->setIcon( KIcon( QLatin1String( "input-tablet" ) ) );
    action->setGlobalShortcut( KShortcut( Qt::CTRL + Qt::META + Qt::Key_T ) );

    action = d->m_actionCollection->addAction(QLatin1String("Toggle stylus mode"));
    action->setText( i18nc( "@action", "Toggle the Stylus Tool Relative/Absolute" ) );
    action->setIcon( KIcon( QLatin1String( "draw-path" ) ) );
    action->setGlobalShortcut( KShortcut( Qt::CTRL + Qt::META + Qt::Key_S ));

    action = d->m_actionCollection->addAction(QLatin1String("Toggle screen map selection"));
    action->setText( i18nc( "@action", "Toggle between all screens" ) );
    action->setIcon( KIcon( QLatin1String( "draw-path" ) ) );
    action->setGlobalShortcut( KShortcut( Qt::CTRL + Qt::META + Qt::Key_M ) );

    action = d->m_actionCollection->addAction(QLatin1String("Map to fullscreen"));
    action->setText( i18nc( "@action Maps the area of the tablet to all available screen space (space depends on connected monitors)", "Map to fullscreen" ) );
    action->setIcon( KIcon( QLatin1String( "video-display" ) ) );
    action->setGlobalShortcut( KShortcut( Qt::CTRL + Qt::META + Qt::Key_F ) );

    action = d->m_actionCollection->addAction(QLatin1String("Map to screen 1"));
    action->setText( i18nc( "@action", "Map to screen 1" ) );
    action->setIcon( KIcon( QLatin1String( "video-display" ) ) );
    action->setGlobalShortcut( KShortcut( Qt::CTRL + Qt::META + Qt::Key_1 ) );

    action = d->m_actionCollection->addAction(QLatin1String("Map to screen 2"));
    action->setText( i18nc( "@action", "Map to screen 2" ) );
    action->setIcon( KIcon( QLatin1String( "video-display" ) ) );
    action->setGlobalShortcut( KShortcut( Qt::CTRL + Qt::META + Qt::Key_2 ) );

    d->m_shortcutEditor = new KShortcutsEditor(this, KShortcutsEditor::GlobalAction);
    d->m_shortcutEditor->addCollection(d->m_actionCollection, i18n("Wacom Tablet Settings"));

    d->m_ui->shortcutGroupBox->layout()->addWidget(d->m_shortcutEditor);

    connect(d->m_shortcutEditor, SIGNAL(keyChange()), this, SLOT(profileChanged()));
}


GeneralPageWidget::~GeneralPageWidget()
{
    delete this->d_ptr;
}


void GeneralPageWidget::saveToProfile()
{
    Q_D( GeneralPageWidget );

    d->m_shortcutEditor->save();
}


void GeneralPageWidget::loadFromProfile() {}


void GeneralPageWidget::profileChanged()
{
    emit changed();
}


void GeneralPageWidget::reloadWidget()
{
    Q_D( GeneralPageWidget );

    //get information via DBus
    QDBusReply<QString> deviceModel      = DBusTabletInterface::instance().getInformation(TabletInfo::TabletModel);
    QDBusReply<QString> deviceName       = DBusTabletInterface::instance().getInformation(TabletInfo::TabletName);
    QDBusReply<QString> companyName      = DBusTabletInterface::instance().getInformation(TabletInfo::CompanyName);
    QDBusReply<QStringList> inputDevices = DBusTabletInterface::instance().getDeviceList();

    //show tablet or generic icon and some tablet information
    KIcon genericTablet( QLatin1String( "input-tablet" ));
    d->m_ui->tabletImage->setPixmap(genericTablet.pixmap(128,128));

    d->m_ui->comapnyName->setText(companyName);
    d->m_ui->tabletName->setText(deviceName);
    d->m_ui->deviceList->setText(inputDevices.value().join( QLatin1String( "\n" )));
}
