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

#include "generalwidget.h"
#include "ui_generalwidget.h"

#include "profilemanagement.h"

// common
#include "deviceinfo.h"
#include "dbusdeviceinterface.h"

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
class GeneralWidgetPrivate {
    public:
        std::auto_ptr<Ui::GeneralWidget>  m_ui;                /**< Handler to the generalwidget.ui file */
        QPointer<KActionCollection>       m_actionCollection;
        QPointer<KShortcutsEditor>        m_shortcutEditor;
}; // CLASS
}  // NAMESPACE


GeneralWidget::GeneralWidget(QWidget *parent)
        : QWidget(parent), d_ptr(new GeneralWidgetPrivate)
{
    Q_D( GeneralWidget );

    d->m_ui = std::auto_ptr<Ui::GeneralWidget>(new Ui::GeneralWidget);
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

    d->m_shortcutEditor = new KShortcutsEditor(this, KShortcutsEditor::GlobalAction);
    d->m_shortcutEditor->addCollection(d->m_actionCollection, i18n("Wacom Tablet Settings"));

    d->m_ui->shortcutGroupBox->layout()->addWidget(d->m_shortcutEditor);

    connect(d->m_shortcutEditor, SIGNAL(keyChange()), this, SLOT(profileChanged()));
}


GeneralWidget::~GeneralWidget()
{
    delete this->d_ptr;
}


void GeneralWidget::saveToProfile()
{
    Q_D( GeneralWidget );

    d->m_shortcutEditor->save();
}


void GeneralWidget::loadFromProfile() {}


void GeneralWidget::profileChanged()
{
    emit changed();
}


void GeneralWidget::reloadWidget()
{
    Q_D( GeneralWidget );

    //get information via DBus
    QDBusReply<QString> deviceModel      = DBusDeviceInterface::instance().getInformation(DeviceInfo::TabletModel);
    QDBusReply<QString> deviceName       = DBusDeviceInterface::instance().getInformation(DeviceInfo::TabletName);
    QDBusReply<QString> companyName      = DBusDeviceInterface::instance().getInformation(DeviceInfo::CompanyName);
    QDBusReply<QStringList> inputDevices = DBusDeviceInterface::instance().deviceList();

    //show tablet or generic icon and some tablet information
    KIcon genericTablet( QLatin1String( "input-tablet" ));
    d->m_ui->tabletImage->setPixmap(genericTablet.pixmap(128,128));

    d->m_ui->comapnyName->setText(companyName);
    d->m_ui->tabletName->setText(deviceName);
    d->m_ui->deviceList->setText(inputDevices.value().join( QLatin1String( "\n" )));
}
