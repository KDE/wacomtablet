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

//KDE includes
#include <KDE/KStandardDirs>
#include <KDE/KIcon>
#include <KDE/KComponentData>
#include <KDE/KShortcutsEditor>
#include <KDE/KActionCollection>
#include <KDE/KAction>

//Qt includes
#include <QtCore/QStringList>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>

#include <QDebug>

using namespace Wacom;

GeneralWidget::GeneralWidget(QDBusInterface *deviceInterface, ProfileManagement *profileManager, QWidget *parent)
        : QWidget(parent),
        m_ui(new Ui::GeneralWidget),
        m_deviceInterface(deviceInterface),
        m_profileManagement(profileManager),
        m_shortcutEditor(0)
{
    m_ui->setupUi(this);

    //if someone adds another action also add it to kded/tabletdeamon.cpp
    m_actionCollection = new KActionCollection(this, KComponentData("wacomtablet"));
    m_actionCollection->setConfigGlobal(true);

    KAction *action = m_actionCollection->addAction(QLatin1String("toggle-touch-mode"));
    action->setText( i18nc( "@action", "Enable/Disable the Touch Tool" ) );
    action->setIcon( KIcon( QLatin1String( "input-tablet" ) ) );
    action->setGlobalShortcut( KShortcut( Qt::CTRL + Qt::META + Qt::Key_T ) );

    action = m_actionCollection->addAction(QLatin1String("toggle-stylus-mode"));
    action->setText( i18nc( "@action", "Toggle the Stylus Tool Relative/Absolute" ) );
    action->setIcon( KIcon( QLatin1String( "draw-path" ) ) );
    action->setGlobalShortcut( KShortcut( Qt::CTRL + Qt::META + Qt::Key_S ));
    m_shortcutEditor = new KShortcutsEditor(this, KShortcutsEditor::GlobalAction);
    m_shortcutEditor->addCollection(m_actionCollection, i18n("Wacom Tablet Settings"));

    m_ui->shortcutGroupBox->layout()->addWidget(m_shortcutEditor);

    connect(m_shortcutEditor, SIGNAL(keyChange()), this, SLOT(profileChanged()));
}

GeneralWidget::~GeneralWidget()
{
    delete m_ui;
}

void GeneralWidget::saveToProfile()
{
    m_shortcutEditor->save();
}

void GeneralWidget::loadFromProfile()
{
}

void GeneralWidget::profileChanged()
{
    emit changed();
}

void GeneralWidget::reloadWidget()
{
    //get information via DBus
    QDBusReply<QString> deviceModel = m_deviceInterface->call(QLatin1String( "deviceModel" ));
    QDBusReply<QString> deviceName  = m_deviceInterface->call(QLatin1String( "deviceName" ));
    QDBusReply<QString> companyName = m_deviceInterface->call(QLatin1String( "companyName" ));
    QDBusReply<QStringList> inputDevices = m_deviceInterface->call(QLatin1String( "deviceList" ));

    //show tablet or generic icon and some tablet information
    KIcon genericTablet( QLatin1String( "input-tablet" ));
    m_ui->tabletImage->setPixmap(genericTablet.pixmap(128,128));

    m_ui->comapnyName->setText(companyName);
    m_ui->tabletName->setText(deviceName);
    m_ui->deviceList->setText(inputDevices.value().join( QLatin1String( "\n" )));
}
