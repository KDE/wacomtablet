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

#include "presscurvedialog.h"
#include "ui_presscurvedialog.h"

#include "presscurve.h"
#include "dbustabletinterface.h"

//KDE includes
#include <KDE/KDebug>

//Qt includes
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>

using namespace Wacom;

PressCurveDialog::PressCurveDialog(QWidget *parent) :
        QDialog(parent),
        m_ui(new Ui::PressCurveDialog)
{
    m_ui->setupUi(this);

    connect(m_ui->pc_Widget, SIGNAL(controlPointsChanged(QString)), SLOT(updateControlPoints(QString)));
}

PressCurveDialog::~PressCurveDialog()
{
    delete m_ui;
}

void PressCurveDialog::setDeviceType (const DeviceType& deviceType)
{
    m_device = deviceType.key();
}

void PressCurveDialog::setControllPoints(const QString & points)
{
    QStringList splitPoints = points.split(QLatin1Char( ' ' ));

    if (splitPoints.count() != 4) {
        splitPoints.insert(0, QLatin1String("0"));
        splitPoints.insert(1, QLatin1String("0"));
        splitPoints.insert(2, QLatin1String("100"));
        splitPoints.insert(3, QLatin1String("100"));
    }

    qreal p1 = splitPoints.at(0).toDouble();
    qreal p2 = splitPoints.at(1).toDouble();
    qreal p3 = splitPoints.at(2).toDouble();
    qreal p4 = splitPoints.at(3).toDouble();
    m_ui->pc_Widget->setControlPoints(p1, p2, p3, p4);
    m_ui->pc_Values->setText(QString::fromLatin1("%1 %2 %3 %4").arg(p1).arg(p2).arg(p3).arg(p4));
}

QString PressCurveDialog::getControllPoints()
{
    return m_ui->pc_Values->text();
}

void PressCurveDialog::updateControlPoints(const QString & points)
{
    m_ui->pc_Values->setText(points);
    DBusTabletInterface::instance().setProperty(*DeviceType::find(m_device), Property::PressureCurve, points);
}

void PressCurveDialog::accept()
{
    done(KDialog::Accepted);
}
void PressCurveDialog::reject()
{
    done(KDialog::Rejected);
}
