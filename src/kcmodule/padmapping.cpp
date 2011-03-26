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

#include "padmapping.h"
#include "ui_padmapping.h"

#include "profilemanagement.h"

using namespace Wacom;

PadMapping::PadMapping(ProfileManagement *profileManager, QWidget *parent)
        : QWidget(parent),
        m_ui(new Ui::PadMapping),
        m_profileManagement(profileManager)
{
    m_ui->setupUi(this);
}

PadMapping::~PadMapping()
{
    delete m_ui;
}

void PadMapping::saveToProfile()
{
    KConfigGroup padConfig = m_profileManagement->configGroup(QLatin1String( "pad" ));

    //@DEBUG write out rotation settings name not index numbers
    padConfig.writeEntry("Rotate", m_ui->rotationComboBox->currentIndex());

    padConfig.writeEntry("TopX", m_ui->topX->value());
    padConfig.writeEntry("TopY", m_ui->topY->value());
    padConfig.writeEntry("BottomX", m_ui->bottomX->value());
    padConfig.writeEntry("BottomY", m_ui->bottomY->value());

    padConfig.sync();
}

void PadMapping::loadFromProfile()
{
    KConfigGroup padConfig = m_profileManagement->configGroup(QLatin1String( "pad" ));

    //@DEBUG write out rotation settings name not index numbers
    m_ui->rotationComboBox->setCurrentIndex(padConfig.readEntry("Rotate").toInt());

    m_ui->topX->setValue(padConfig.readEntry("TopX").toInt());
    m_ui->topY->setValue(padConfig.readEntry("TopY").toInt());
    m_ui->bottomX->setValue(padConfig.readEntry("BottomX").toInt());
    m_ui->bottomY->setValue(padConfig.readEntry("BottomY").toInt());
}


void PadMapping::profileChanged()
{
    emit changed();
}

void PadMapping::reloadWidget()
{
}
