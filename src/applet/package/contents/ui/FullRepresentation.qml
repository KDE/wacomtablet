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
import QtQuick.Controls 1.0 as QtControls
import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents

Item {
    function defaultValue(value, d) {
        return (typeof value == 'undefined') ? d : value;
    }

    function deviceLabel() {
        if (dataSource.data["wacomtablet"]["serviceAvailable"]) {
            if (dataModel.count == 0) {
                return i18n("Graphic Tablet - Device not detected.");
            } else {
                if (tabletComboBox.currentIndex >= 0) {
                    return dataModel.get(tabletComboBox.currentIndex).name;
                }
                return "";
            }
        }

        return i18n("Error - Tablet service not available.");
    }

    Row {
        id: title
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        height: childrenRect.height

        PlasmaCore.IconItem {
            source: "input-tablet"
            width: units.iconSizes.Medium
            height: width
        }
        PlasmaComponents.Label {
            id: deviceNameLabel
            text: deviceLabel()
        }
    }

    PlasmaCore.SvgItem {
        id: separator
        anchors {
            top: title.bottom
            left: parent.left
            right: parent.right
        }
        elementId: "horizontal-line"
        svg: lineSvg
        height: lineSvg.elementSize("horizontal-line").height
    }

    Item {
        anchors {
            top: separator.bottom
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            topMargin: units.smallSpacing
        }
        visible: !root.active
        PlasmaCore.IconItem {
            id: errorIcon
            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
            }
            width: units.iconSizes.medium
            height: width
            source: "dialog-warning"
        }

        PlasmaComponents.Label {
            id: errorLabel
            anchors {
                left: errorIcon.right
                right: parent.right
                verticalCenter: parent.verticalCenter
            }
            text: dataSource.data["wacomtablet"]["serviceAvailable"] ?
                i18n("This widget is inactive because your tablet device is not connected or currently not supported.") :
                i18n("Please start the KDE wacom tablet service.\nThe service is required for tablet detection and profile support.")
            wrapMode: Text.Wrap
        }
    }

    function setProfile() {
        profileComboBox.currentIndex = dataModel.get(tabletComboBox.currentIndex).currentProfile;
    }

    GridLayout {
        id: selector
        anchors {
            left: parent.left
            right: parent.right
            top: separator.bottom
            topMargin: units.smallSpacing
        }
        visible: root.active
        columns: 2
        PlasmaComponents.Label {
            text: i18n("Select Tablet:")
        }

        QtControls.ComboBox {
            id: tabletComboBox
            Layout.fillWidth: true
            model: dataModel
            textRole: "name"
            onCurrentIndexChanged: {
                profileModel.clear();
                profileComboBox.currentIndex = -1;
                if (tabletComboBox.currentIndex < 0) {
                    return;
                }

                var profiles = dataModel.get(tabletComboBox.currentIndex).profiles;
                if (typeof profiles == "undefined") {
                    return;
                }
                for (var i = 0; i < profiles.length; i++) {
                    profileModel.append({"name" : profiles[i]});
                }

                setProfile();
            }
        }

        PlasmaComponents.Label {
            text: i18n("Select Profile:")
        }

        ListModel {
            id: profileModel
        }

        QtControls.ComboBox {
            id: profileComboBox
            Layout.fillWidth: true
            model: profileModel
            textRole: "name"

            onActivated: {
                if (tabletComboBox.currentIndex < 0) {
                    return;
                }
                var service = dataSource.serviceForSource("wacomtablet");
                var operation = service.operationDescription("SetProfile");
                operation.tabletId = dataModel.get(tabletComboBox.currentIndex).id;
                operation.profile = profileModel.get(index).name;
                service.startOperationCall(operation);
            }
        }
    }

    Connections {
        target: dataSource
        onConnectedSourcesChanged : {
            tabletComboBox.currentIndex = -1;
            if (dataModel.count > 0) {
                tabletComboBox.currentIndex = 0;
            }
        }

        onNewData: {
            if (tabletComboBox.currentIndex < 0) {
                return;
            }

            var current = dataModel.get(tabletComboBox.currentIndex)
            if (sourceName == current.DataEngineSource) {
                setProfile();
            }
        }
    }

    QtControls.GroupBox {
        visible: root.active
        anchors {
            left: parent.left
            right: parent.right
            top: selector.bottom
            bottom: parent.bottom
            topMargin: units.smallSpacing
        }
        title: i18nc( "Groupbox Settings for the applet to change some values on the fly", "Settings" )
        GridLayout {
            columns: 2
            PlasmaComponents.Label {
                visible: defaultValue(dataModel.get(tabletComboBox.currentIndex).hasTouch, false)
                text: i18nc( "Toggle between touch on/off", "Touch:" )
            }

            QtControls.CheckBox {
                visible: defaultValue(dataModel.get(tabletComboBox.currentIndex).hasTouch, false);
                checked: defaultValue(dataModel.get(tabletComboBox.currentIndex).touch, false);
            }


            PlasmaComponents.Label {
                text: i18nc( "Rotation of the tablet pad", "Rotation:" )
            }

            RowLayout {
                RotationButton {
                    rotation: "none"
                    iconSource: "input-tablet"
                    tooltip: i18nc("Either no orientation or the current screen orientation is applied to the tablet.", "Default Orientation");
                }
                RotationButton {
                    rotation: "cw"
                    iconSource: "object-rotate-left"
                    tooltip: i18nc("The tablet will be rotated clockwise.", "Rotate Tablet Clockwise")
                }
                RotationButton {
                    rotation: "ccw"
                    iconSource: "object-rotate-right"
                    tooltip: i18nc("The tablet will be rotated counterclockwise.", "Rotate Tablet Counterclockwise")
                }
                RotationButton {
                    rotation: "half"
                    iconSource: "object-flip-vertical"
                    tooltip: i18nc("The tablet will be rotated up side down.", "Rotate Tablet Upside-Down")
                }
            }
            PlasmaComponents.Label {
                text: i18nc( "Toggle between absolute/relative penmode", "Mode:" )
            }
            RowLayout {
                QtControls.ExclusiveGroup { id: modeGroup }
                QtControls.RadioButton {
                    text: i18nc( "absolute pen movement (pen mode)", "Absolute" )
                    exclusiveGroup: modeGroup
                    checked: defaultValue(dataModel.get(tabletComboBox.currentIndex).stylusMode, true);
                    onClicked : {
                        if (tabletComboBox.currentIndex < 0) {
                            return;
                        }
                        var service = dataSource.serviceForSource("wacomtablet");
                        var operation = service.operationDescription("SetStylusMode");
                        operation.tabletId = dataModel.get(tabletComboBox.currentIndex).id;
                        operation.mode = "absolute";
                        service.startOperationCall(operation);
                    }
                }
                QtControls.RadioButton {
                    text: i18nc( "relative pen movement (mouse mode)", "Relative" )
                    exclusiveGroup: modeGroup
                    onClicked : {
                        if (tabletComboBox.currentIndex < 0) {
                            return;
                        }
                        var service = dataSource.serviceForSource("wacomtablet");
                        var operation = service.operationDescription("SetStylusMode");
                        operation.tabletId = dataModel.get(tabletComboBox.currentIndex).id;
                        operation.mode = "relative";
                        service.startOperationCall(operation);
                    }
                }
            }
        }
    }

}
