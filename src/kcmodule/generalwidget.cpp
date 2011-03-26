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

//Qt includes
#include <QtCore/QStringList>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>

using namespace Wacom;

GeneralWidget::GeneralWidget(QDBusInterface *deviceInterface, ProfileManagement *profileManager, QWidget *parent)
        : QWidget(parent),
        m_ui(new Ui::GeneralWidget),
        m_deviceInterface(deviceInterface),
        m_profileManagement(profileManager)
{
    m_ui->setupUi(this);

    reloadWidget();
}

GeneralWidget::~GeneralWidget()
{
    delete m_ui;
}

void GeneralWidget::saveToProfile()
{
    KConfigGroup padConfig = m_profileManagement->configGroup(QLatin1String( "pad" ));

    if(m_ui->tabletPCButtonCheckBox->isChecked() ) {
        padConfig.writeEntry("TabletPCButton", "on");
    }
    else {
        padConfig.writeEntry("TabletPCButton", "off");
    }
    
    if(m_ui->touchEventsCheckBox->isChecked() ) {
        padConfig.writeEntry("Touch", "on");
    }
    else {
        padConfig.writeEntry("Touch", "off");
    }
    
    if(m_ui->gesturesCheckBox->isChecked() ) {
        padConfig.writeEntry("Gesture", "on");
    }
    else {
        padConfig.writeEntry("Gesture", "off");
    }

    padConfig.writeEntry("ZoomDistance", m_ui->zoomDistanceBox->value());
    padConfig.writeEntry("ScrollDistance", m_ui->scrollDistanceBox->value());
    
    padConfig.sync();
}

void GeneralWidget::loadFromProfile()
{
    KConfigGroup padConfig = m_profileManagement->configGroup(QLatin1String( "pad" ));
    
    QString tabletPCButton = padConfig.readEntry( QLatin1String("TabletPCButton" ));
    if(tabletPCButton == QLatin1String("on")) {
        m_ui->tabletPCButtonCheckBox->setChecked(true);
    }
    else {
        m_ui->tabletPCButtonCheckBox->setChecked(false);
    }
    
    QString touch = padConfig.readEntry(QLatin1String("Touch"));
    if(touch == QLatin1String("on")) {
        m_ui->touchEventsCheckBox->setChecked(true);
    }
    else {
        m_ui->touchEventsCheckBox->setChecked(false);
    }
    
    QString gesture = padConfig.readEntry(QLatin1String("Gesture"));
    if(gesture == QLatin1String("on")) {
        m_ui->gesturesCheckBox->setChecked(true);
    }
    else {
        m_ui->gesturesCheckBox->setChecked(false);
    }
    
    int zoomDistance = padConfig.readEntry(QLatin1String("ZoomDistance")).toInt();
    m_ui->zoomDistanceBox->setValue(zoomDistance);
    
    int scrollDistance = padConfig.readEntry(QLatin1String("ScrollDistance")).toInt();
    m_ui->scrollDistanceBox->setValue(scrollDistance);
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
