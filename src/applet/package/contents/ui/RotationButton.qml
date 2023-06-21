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
import org.kde.plasma.components 2.0 as PlasmaComponents

PlasmaComponents.ToolButton {
    property string rotation;

    onClicked : {
        if (tabletComboBox.currentIndex < 0) {
            return;
        }
        var service = dataSource.serviceForSource("wacomtablet");
        var operation = service.operationDescription("SetRotation");
        operation.tabletId = dataModel.get(tabletComboBox.currentIndex).id;
        operation.rotation = rotation
        service.startOperationCall(operation);
    }
}
