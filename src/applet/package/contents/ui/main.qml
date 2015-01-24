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
    id: root

    Plasmoid.toolTipMainText: i18n("Wacom Tablet")
    Plasmoid.status: dataSource.data["wacomtablet"]["serviceAvailable"] && dataModel.count != 0 ? PlasmaCore.Types.ActiveStatus : PlasmaCore.Types.PassiveStatus
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
        sourceFilter: "Tablet[0-9]+"
    }

    Plasmoid.fullRepresentation: ColumnLayout {
        Row {
            PlasmaCore.IconItem {
                source: "input-tablet"
                width: units.iconSizes.Medium
                height: width
            }
            PlasmaComponents.Label {
                id: deviceNameLabel
            }
        }

        PlasmaCore.SvgItem {
            id: separator
            Layout.fillWidth: true
            elementId: "horizontal-line"
            svg: lineSvg
            height: lineSvg.elementSize("horizontal-line").height
        }

        function setProfile() {
            profileComboBox.currentIndex = dataModel.get(tabletComboBox.currentIndex).currentProfile;
        }

        GridLayout {
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
            Layout.fillWidth: true
            title: i18nc( "Groupbox Settings for the applet to change some values on the fly", "Settings" )
            visible: dataModel.count > 0
            GridLayout {
                columns: 2
                PlasmaComponents.Label {
                    visible: dataModel.get(tabletComboBox.currentIndex).hasTouch
                    text: i18nc( "Toggle between touch on/off", "Touch:" )
                }

                QtControls.CheckBox {
                    visible: dataModel.get(tabletComboBox.currentIndex).hasTouch
                    checked: dataModel.get(tabletComboBox.currentIndex).touch
                }


                PlasmaComponents.Label {
                    text: i18nc( "Rotation of the tablet pad", "Rotation:" )
                }

                RowLayout {
                    RotationButton {
                        rotation: "none"
                        icon: "input-tablet"
                    }
                    RotationButton {
                        rotation: "cw"
                        icon: "object-rotate-left"
                    }
                    RotationButton {
                        rotation: "ccw"
                        icon: "object-rotate-right"
                    }
                    RotationButton {
                        rotation: "half"
                        icon: "object-flip-vertical"
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
                        checked: dataModel.get(tabletComboBox.currentIndex).stylusMode
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
                        checked: !dataModel.get(tabletComboBox.currentIndex).stylusMode
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
}
