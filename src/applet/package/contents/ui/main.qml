// -*- coding: iso-8859-1 -*-
/*
 *   Copyright 2015 Weng Xuetian <wengxt@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick 2.0
import QtQuick.Layouts 1.0
import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.core 2.0 as PlasmaCore

Item {
    id: root

    function action_wacomtabletkcm() {
        var service = dataSource.serviceForSource("wacomtablet");
        var operation = service.operationDescription("RunKCM");
        service.startOperationCall(operation);
    }

    Component.onCompleted: {
        plasmoid.removeAction("configure");
        plasmoid.setAction("wacomtabletkcm", i18n("&Configure Graphics Tablet..."), "input-tablet");
    }

    property bool active: dataSource.data["wacomtablet"]["serviceAvailable"] && dataModel.count != 0

    Plasmoid.toolTipMainText: i18n("Wacom Tablet")
    Plasmoid.status: active ? PlasmaCore.Types.ActiveStatus : PlasmaCore.Types.PassiveStatus
    Plasmoid.icon: "input-tablet"
    PlasmaCore.Svg {
        id: lineSvg
        imagePath: "widgets/line"
    }

    PlasmaCore.DataSource {
        id: dataSource
        engine: "wacomtablet"
        connectedSources: dataSource.sources
    }
    PlasmaCore.DataModel {
        id: dataModel
        dataSource: dataSource
        sourceFilter: "Tablet.*"
    }

    Plasmoid.fullRepresentation: FullRepresentation { }
}
