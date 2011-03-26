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

#include "screenwidget.h"
#include "ui_screenwidget.h"

#include "profilemanagement.h"

using namespace Wacom;

ScreenWidget::ScreenWidget(ProfileManagement *profileManager, QWidget *parent)
        : QWidget(parent),
        m_ui(new Ui::ScreenWidget),
        m_profileManagement(profileManager)
{
    m_ui->setupUi(this);
}

ScreenWidget::~ScreenWidget()
{
    delete m_ui;
}

void ScreenWidget::saveToProfile()
{
    // stylus and eraser are handled the same way
    // makes no sense to have different screen config for both sides individually
    KConfigGroup screenConfigStylus = m_profileManagement->configGroup(QLatin1String( "stylus" ));
    KConfigGroup screenConfigEraser = m_profileManagement->configGroup(QLatin1String( "eraser" ));

    switch(m_ui->twinview_comboBox->currentIndex())
    {
      case 0:
        screenConfigStylus.writeEntry("0TwinView", "none");
        screenConfigEraser.writeEntry("0TwinView", "none");
        break;
      case 1:
        screenConfigStylus.writeEntry("0TwinView", "vertical");
        screenConfigEraser.writeEntry("0TwinView", "vertical");
        break;
      case 2:
        screenConfigStylus.writeEntry("0TwinView", "horizontal");
        screenConfigEraser.writeEntry("0TwinView", "horizontal");
        break;
      default:
        screenConfigStylus.writeEntry("0TwinView", "none");
        screenConfigEraser.writeEntry("0TwinView", "none");
        break;
    }

    //screenConfigStylus.writeEntry("0TwinView", m_ui->twinview_comboBox->currentIndex());
    //screenConfigEraser.writeEntry("0TwinView", m_ui->twinview_comboBox->currentIndex());

    if (m_ui->mmonitor_checkBox->isChecked()) {
        screenConfigStylus.writeEntry("mmonitor", "1");
        screenConfigEraser.writeEntry("mmonitor", "1");
    } else {
        screenConfigStylus.writeEntry("mmonitor", "0");
        screenConfigEraser.writeEntry("mmonitor", "0");
    }

    screenConfigStylus.writeEntry("1TVResolution0", QString::fromLatin1("%1 %2").arg(m_ui->spinBox_0_x->value()).arg(m_ui->spinBox_0_y->value()));
    screenConfigStylus.writeEntry("1TVResolution1", QString::fromLatin1("%1 %2").arg(m_ui->spinBox_1_x->value()).arg(m_ui->spinBox_1_y->value()));
    screenConfigEraser.writeEntry("1TVResolution0", QString::fromLatin1("%1 %2").arg(m_ui->spinBox_0_x->value()).arg(m_ui->spinBox_0_y->value()));
    screenConfigEraser.writeEntry("1TVResolution1", QString::fromLatin1("%1 %2").arg(m_ui->spinBox_1_x->value()).arg(m_ui->spinBox_1_y->value()));

    screenConfigStylus.writeEntry("Screen_No", (m_ui->screenComboBox->currentIndex() - 1));
    screenConfigEraser.writeEntry("Screen_No", (m_ui->screenComboBox->currentIndex() - 1));
}

void ScreenWidget::loadFromProfile()
{
    // because both stylus and eraser are treated the same way we take just the
    // stylus config and assume it is the same for both.
    KConfigGroup screenConfig = m_profileManagement->configGroup(QLatin1String( "stylus" ));

    QString tvMode = screenConfig.readEntry("0TwinView");
    int index = 0;

    if(tvMode == QLatin1String("vertical")) {
      index = 1;
    }
    if(tvMode == QLatin1String("horizontal")) {
      index = 2;
    }

    m_ui->twinview_comboBox->setCurrentIndex(index);
    switchTwinView(index);

    //Screen Setup
    if (screenConfig.readEntry("mmonitor").toInt() == 1) {
        m_ui->mmonitor_checkBox->setChecked(true);
    } else {
        m_ui->mmonitor_checkBox->setChecked(false);
    }

    QStringList res0 = screenConfig.readEntry("1TVResolution0").split(QLatin1Char( ' ' ));
    QStringList res1 = screenConfig.readEntry("1TVResolution1").split(QLatin1Char( ' ' ));

    // do some checks before we show the value
    if (res0.count() != 2) {
        res0.insert(0, QLatin1String("0"));
        res0.insert(1, QLatin1String("0"));
    }

    if (res1.count() != 2) {
        res1.insert(0, QLatin1String("0"));
        res1.insert(1, QLatin1String("0"));
    }

    m_ui->spinBox_0_x->setValue(res0.at(0).toInt());
    m_ui->spinBox_0_y->setValue(res0.at(1).toInt());
    m_ui->spinBox_1_x->setValue(res1.at(0).toInt());
    m_ui->spinBox_1_y->setValue(res1.at(1).toInt());

    m_ui->screenComboBox->setCurrentIndex((screenConfig.readEntry("Screen_No").toInt() + 1));
}

void ScreenWidget::reloadWidget()
{
}

void ScreenWidget::profileChanged()
{
    emit changed();
}

void ScreenWidget::switchTwinView(int currentIndex)
{
    bool enable = false;

    if (currentIndex > 0) {
        enable = true;
    }

    m_ui->mmonitor_checkBox->setEnabled(enable);
    m_ui->screenComboBox->setEnabled(enable);
    m_ui->spinBox_0_x->setEnabled(enable);
    m_ui->spinBox_0_y->setEnabled(enable);
    m_ui->spinBox_1_x->setEnabled(enable);
    m_ui->spinBox_1_y->setEnabled(enable);
}
