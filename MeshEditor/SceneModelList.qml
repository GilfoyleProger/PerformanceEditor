import QtQuick 2.15
import MyRenderLibrary 1.0
import QtQuick.Scene3D 2.15
import Qt3D.Render 2.15
import Qt3D.Logic 2.15
import Qt3D.Extras 2.15
import Qt3D.Core 2.15
import QtQml 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.3

Rectangle {
    id: root
    color: "white"
    height: layout.height
    width: 110
    border.width: 1
    border.color: "black"

    property int selectedIndex: -1
    property int maxHeight: 300

    ColumnLayout {
        id:layout
        height: showButton.checked ? Math.min(maxHeight, models.contentHeight + showButton.height + layout.spacing) : showButton.height

        Button {
            id:showButton
            checkable: true
            checked: enabled

            ToolTip.delay: 1000
            ToolTip.timeout: 5000
            ToolTip.visible: hovered && !pressed
            ToolTip.text: qsTr("Show/Hide Solids")

            Layout.alignment: Qt.AlignTop
            Layout.preferredHeight: 50
            Layout.preferredWidth: root.width

            background: Rectangle {
                color: showButton.enabled ? "white" : "#0000000"
                border.width: 1
                border.color: showButton.enabled ? "black" : "gray"
            }

            RowLayout {
                id: buttonContent
                anchors.fill: parent
                spacing: 0
                opacity: showButton.enabled ? 1 : 0.3

                Text {
                    id: buttonName
                    text: qsTr("Solid List")
                    Layout.leftMargin: 5
                    color: "black"
                }

                Image {
                    id:buttonPointer
                    source: "/icons/expand-button.png"
                    rotation: showButton.checked ? 90 : 0
                    Layout.preferredHeight: 11
                    Layout.preferredWidth: height
                    Layout.rightMargin: 2
                }
            }
        }

        ListView {
            id: models
            visible: showButton.checked
            model: ModelTab.models

            Layout.maximumHeight: 90
            Layout.fillHeight: true
            Layout.fillWidth: true
            ScrollBar.vertical: ScrollBar{}


            delegate: Item {
                id: modelRow
                width: root.width
                height: 30

                ToolTip {
                    id: modelNameToolTip
                    delay: 1000
                    timeout: 5000
                    visible: mouseArea.containsMouse
                    text: modelData.name
                }

                RowLayout {
                    height: 30

                    Button {
                        Layout.leftMargin: 88
                        Layout.preferredHeight: 16
                        Layout.preferredWidth: 16
                        onClicked: renderSystem.removeModel(index)

                        ToolTip.delay: 1000
                        ToolTip.timeout: 5000
                        ToolTip.visible: hovered
                        ToolTip.text: qsTr("Remove Solid")

                        background: Rectangle {
                            color: showButton.enabled? "white":"#0000000"
                            border.width: 0
                            border.color: showButton.enabled ? "black" : "gray"
                        }

                        RowLayout {
                            Image {
                                source: "/icons/delete.png"
                                Layout.preferredHeight: 16
                                Layout.preferredWidth: 16
                            }
                        }
                    }
                }

                Rectangle {
                    id: highlight
                    color:"transparent"
                    border.color: "blue"
                    border.width: 2
                    anchors.fill: parent
                    visible: modelData.recursivelySelected
                    z: -1
                }

                MouseArea {
                    id: mouseArea
                    anchors.fill: parent
                    hoverEnabled: true

                    onClicked: {
                        modelData.recursivelySelected = true
                        modelNameToolTip.hide()
                    }
                    z: -1
                }

                RowLayout {
                    width: root.width - 4
                    height: 30

                    Text {
                        id: modelNameText
                        Layout.preferredWidth: 80
                        height: 30
                        Layout.leftMargin: 5
                        text: modelData.name
                        elide: Text.ElideRight
                    }
                }
            }
        }
    }
}
