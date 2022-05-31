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

Rectangle
{
    id: root
    color: "white"
    height: layout.height
    width: 110
    border.width: 1
    border.color: "black"

    property int maxHeight: 300

    ColumnLayout {
        id: layout
        height: showButton.checked ? Math.min(maxHeight, models.contentHeight + showButton.height + layout.spacing) : showButton.height

        Button {
            id: showButton

            Layout.alignment: Qt.AlignTop
            Layout.preferredHeight: 50
            Layout.preferredWidth: root.width

            checkable: true
            checked: enabled

            ToolTip.delay: 1000
            ToolTip.timeout: 5000
            ToolTip.visible: hovered && !pressed
            ToolTip.text: qsTr("Show/Hide Lights")

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
                    text: qsTr("Light List")
                    font.pointSize: 8
                    Layout.leftMargin: 5
                    color: "black"
                }

                ColumnLayout {
                    Layout.leftMargin: 19//38

                    Button {
                        Layout.preferredHeight: 24
                        Layout.preferredWidth: 24
                        onClicked: renderSystem.addPointLight()
                        ToolTip.delay: 1000
                        ToolTip.timeout: 5000
                        ToolTip.visible: hovered
                        ToolTip.text: qsTr("Add Point Light")

                        background: Rectangle {
                            color: showButton.enabled? "white" : "#0000000"
                            border.width: 0
                            border.color: showButton.enabled ? "black" : "gray"
                        }

                        RowLayout {
                            Image {
                                source: "/icons/add-point-light.png"
                                Layout.preferredHeight: 24
                                Layout.preferredWidth: 24
                            }
                        }
                    }

                    Image {
                        id: buttonPointer
                        Layout.leftMargin: 8
                        source: "/icons/expand-button.png"
                        rotation: showButton.checked ? 90 : 0
                        Layout.preferredHeight: 11
                        Layout.preferredWidth: height
                    }
                }
            }
        }

        ListView
        {
            id: models
            visible: showButton.checked
            model: ModelTab.lights
            Layout.maximumHeight: 90
            Layout.fillHeight: true
            Layout.fillWidth: true
            ScrollBar.vertical: ScrollBar{}

            delegate: Item {
                id: modelRow
                width: root.width
                height: 30

                RowLayout {
                    height: 30

                    Button {
                        Layout.leftMargin: 88
                        Layout.preferredHeight: 16
                        Layout.preferredWidth: 16
                        onClicked: renderSystem.removePointLight(index)

                        ToolTip.delay: 1000
                        ToolTip.timeout: 5000
                        ToolTip.visible: hovered
                        ToolTip.text: qsTr("Remove Point Light")

                        background: Rectangle {
                            color: showButton.enabled? "white" : "#0000000"
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
                    visible: modelData.selected
                    z: -1
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: { modelData.selected = true }
                    z: -1
                }

                RowLayout {
                    width: root.width-4
                    height: 30
                    Text {
                        height: 30
                        Layout.leftMargin: 5
                        text:modelData.name
                    }
                }
            }
        }
    }
}
