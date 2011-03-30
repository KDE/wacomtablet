/*
 * Copyright 2011 Jörg Ehrichs <joerg.ehichs@gmx.de>
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

#include "touchwidget.h"
#include "ui_touchwidget.h"

#include "profilemanagement.h"

//Qt includes
#include <QtCore/QStringList>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>

using namespace Wacom;

TouchWidget::TouchWidget(ProfileManagement *profileManager, QWidget *parent)
        : QWidget(parent),
        m_ui(new Ui::TouchWidget),
        m_profileManagement(profileManager)
{
    m_ui->setupUi(this);

    reloadWidget();
}

TouchWidget::~TouchWidget()
{
    delete m_ui;
}

void TouchWidget::saveToProfile()
{
    KConfigGroup touchConfig = m_profileManagement->configGroup(QLatin1String( "touch" ));

    if(m_ui->touchEventsCheckBox->isChecked() ) {
        touchConfig.writeEntry("Touch", "on");
    }
    else {
        touchConfig.writeEntry("Touch", "off");
    }

    if(m_ui->gesturesCheckBox->isChecked() ) {
        touchConfig.writeEntry("Gesture", "on");
    }
    else {
        touchConfig.writeEntry("Gesture", "off");
    }

    touchConfig.writeEntry("ZoomDistance", m_ui->zoomDistanceBox->value());
    touchConfig.writeEntry("ScrollDistance", m_ui->scrollDistanceBox->value());
    touchConfig.writeEntry("TapTime", m_ui->tapTimeBox->value());

    touchConfig.sync();
}

void TouchWidget::loadFromProfile()
{
    KConfigGroup touchConfig = m_profileManagement->configGroup(QLatin1String( "touch" ));

    QString touch = touchConfig.readEntry(QLatin1String("Touch"));
    if(touch == QLatin1String("on")) {
        m_ui->touchEventsCheckBox->setChecked(true);
    }
    else {
        m_ui->touchEventsCheckBox->setChecked(false);
    }

    QString gesture = touchConfig.readEntry(QLatin1String("Gesture"));
    if(gesture == QLatin1String("on")) {
        m_ui->gesturesCheckBox->setChecked(true);
    }
    else {
        m_ui->gesturesCheckBox->setChecked(false);
    }

    int zoomDistance = touchConfig.readEntry(QLatin1String("ZoomDistance")).toInt();
    m_ui->zoomDistanceBox->setValue(zoomDistance);

    int scrollDistance = touchConfig.readEntry(QLatin1String("ScrollDistance")).toInt();
    m_ui->scrollDistanceBox->setValue(scrollDistance);

    int tapTime = touchConfig.readEntry(QLatin1String("ScrollDistance")).toInt();
    m_ui->tapTimeBox->setValue(tapTime);
}

void TouchWidget::profileChanged()
{
    emit changed();
}

void TouchWidget::reloadWidget()
{
}
