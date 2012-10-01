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

#include "screenwidget.h"
#include "ui_screenwidget.h"

#include "profilemanagement.h"

#include "property.h"
#include "deviceprofile.h"

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
    DeviceProfile stylusProfile = m_profileManagement->loadDeviceProfile(QLatin1String( "stylus" ));
    DeviceProfile eraserProfile = m_profileManagement->loadDeviceProfile(QLatin1String( "eraser" ));

    switch(m_ui->twinview_comboBox->currentIndex())
    {
      case 0:
        stylusProfile.setProperty(Property::TwinView, "none");
        eraserProfile.setProperty(Property::TwinView, "none");
        break;
      case 1:
        stylusProfile.setProperty(Property::TwinView, "vertical");
        eraserProfile.setProperty(Property::TwinView, "vertical");
        break;
      case 2:
        stylusProfile.setProperty(Property::TwinView, "horizontal");
        eraserProfile.setProperty(Property::TwinView, "horizontal");
        break;
      default:
        stylusProfile.setProperty(Property::TwinView, "none");
        eraserProfile.setProperty(Property::TwinView, "none");
        break;
    }

    //stylusProfile.setProperty("0TwinView", m_ui->twinview_comboBox->currentIndex());
    //eraserProfile.setProperty("0TwinView", m_ui->twinview_comboBox->currentIndex());

    if (m_ui->mmonitor_checkBox->isChecked()) {
        stylusProfile.setProperty(Property::MMonitor, "1");
        eraserProfile.setProperty(Property::MMonitor, "1");
    } else {
        stylusProfile.setProperty(Property::MMonitor, "0");
        eraserProfile.setProperty(Property::MMonitor, "0");
    }

    stylusProfile.setProperty(Property::TVResolution0, QString::fromLatin1("%1 %2").arg(m_ui->spinBox_0_x->value()).arg(m_ui->spinBox_0_y->value()));
    stylusProfile.setProperty(Property::TVResolution1, QString::fromLatin1("%1 %2").arg(m_ui->spinBox_1_x->value()).arg(m_ui->spinBox_1_y->value()));
    eraserProfile.setProperty(Property::TVResolution0, QString::fromLatin1("%1 %2").arg(m_ui->spinBox_0_x->value()).arg(m_ui->spinBox_0_y->value()));
    eraserProfile.setProperty(Property::TVResolution1, QString::fromLatin1("%1 %2").arg(m_ui->spinBox_1_x->value()).arg(m_ui->spinBox_1_y->value()));

    stylusProfile.setProperty(Property::ScreenNo, (m_ui->screenComboBox->currentIndex() - 1));
    eraserProfile.setProperty(Property::ScreenNo, (m_ui->screenComboBox->currentIndex() - 1));

    m_profileManagement->saveDeviceProfile(stylusProfile);
    m_profileManagement->saveDeviceProfile(eraserProfile);
}

void ScreenWidget::loadFromProfile()
{
    // because both stylus and eraser are treated the same way we take just the
    // stylus config and assume it is the same for both.
    DeviceProfile deviceProfile = m_profileManagement->loadDeviceProfile(QLatin1String( "stylus" ));

    QString tvMode = deviceProfile.getProperty(Property::TwinView);
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
    if (deviceProfile.getProperty(Property::MMonitor).toInt() == 1) {
        m_ui->mmonitor_checkBox->setChecked(true);
    } else {
        m_ui->mmonitor_checkBox->setChecked(false);
    }

    QStringList res0 = deviceProfile.getProperty(Property::TVResolution0).split(QLatin1Char( ' ' ));
    QStringList res1 = deviceProfile.getProperty(Property::TVResolution1).split(QLatin1Char( ' ' ));

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

    m_ui->screenComboBox->setCurrentIndex((deviceProfile.getProperty(Property::ScreenNo).toInt() + 1));
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
