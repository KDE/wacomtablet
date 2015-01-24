/*
 * Copyright (C) 2015 Weng Xuetian <wengxt@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include "wacomtabletservice.h"
#include "dbustabletinterface.h"
#include "screenrotation.h"
#include <QProcess>
#include <Plasma/ServiceJob>

using namespace Wacom;

class WacomTabletJob : public Plasma::ServiceJob
{
    Q_OBJECT
public:
    WacomTabletJob(const QString& destination, const QString& operation, const QMap< QString, QVariant >& parameters, QObject* parent = 0) : ServiceJob(destination, operation, parameters, parent) {
    }

    virtual void start() {
        if (!DBusTabletInterface::instance().isValid()) {
            return;
        }

        const auto op = operationName();
        if (op == QLatin1Literal("RunKCM")) {
            QProcess::startDetached(QLatin1Literal("kcmshell5"), QStringList() << QLatin1Literal("wacomtablet"));
            return;
        }

        const auto param = parameters();

        if (!param.contains(QLatin1Literal("tabletId"))) {
            return;
        }

        const auto tabletId = param[QLatin1Literal("tabletId")].toString();

        if (op == QLatin1Literal("SetProfile")) {
            if (param.contains(QLatin1Literal("profile"))) {
                DBusTabletInterface::instance().setProfile(tabletId, param[QLatin1Literal("profile")].toString());
            }
        } else if (op == QLatin1Literal("SetStylusMode")) {
            if (param.contains(QLatin1Literal("mode"))) {
                DBusTabletInterface::instance().setProperty(tabletId, DeviceType::Stylus.key(), Property::Mode.key(), param[QLatin1Literal("mode")].toString());
                DBusTabletInterface::instance().setProperty(tabletId, DeviceType::Eraser.key(), Property::Mode.key(), param[QLatin1Literal("mode")].toString());
            }
        } else if (op == QLatin1Literal("SetRotation")) {
            if (param.contains(QLatin1Literal("rotation"))) {
                const auto rotationName = param[QLatin1Literal("rotation")].toString();

                const ScreenRotation* rotation = nullptr;
                if (rotationName == QLatin1Literal("none")) {
                    rotation = &ScreenRotation::NONE;
                } else if (rotationName == QLatin1Literal("cw")) {
                    rotation = &ScreenRotation::CW;
                } else if (rotationName == QLatin1Literal("ccw")) {
                    rotation = &ScreenRotation::CCW;
                } else if (rotationName == QLatin1Literal("half")) {
                    rotation = &ScreenRotation::HALF;
                }

                if (rotation) {
                    DBusTabletInterface::instance().setProperty(tabletId, DeviceType::Stylus.key(), Property::Rotate.key(), rotation->key());
                    DBusTabletInterface::instance().setProperty(tabletId, DeviceType::Eraser.key(), Property::Rotate.key(), rotation->key());
                    DBusTabletInterface::instance().setProperty(tabletId, DeviceType::Touch.key(), Property::Rotate.key(), rotation->key());
                }
            }
        } else if (op == QLatin1Literal("SetTouchMode")) {
            if (param.contains(QLatin1Literal("enable"))) {
                DBusTabletInterface::instance().setProperty(tabletId, DeviceType::Touch.key(), Property::Touch.key(), param[QLatin1Literal("enable")].toBool() ? QLatin1String( "on" ) : QLatin1String( "off" ));
            }
        }
    }
};

WacomTabletService::WacomTabletService(const QString& destination, QObject* parent): Service(parent)
{
    setName(QLatin1Literal("wacomtablet"));
    setObjectName(destination);
    setDestination(destination);
    setOperationEnabled(QLatin1Literal("SetProfile"), true);
    setOperationEnabled(QLatin1Literal("SetStylusMode"), true);
    setOperationEnabled(QLatin1Literal("SetRotation"), true);
    setOperationEnabled(QLatin1Literal("SetTouchMode"), true);
}

WacomTabletService::~WacomTabletService()
{

}

Plasma::ServiceJob* WacomTabletService::createJob(const QString& operation, QMap< QString, QVariant >& parameters)
{
    return new WacomTabletJob(destination(), operation, parameters, this);
}

#include "wacomtabletservice.moc"
