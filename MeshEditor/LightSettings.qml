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

ColumnLayout {
/*
    Rectangle {
            color: "#EDF2F4" //"#F5F5F5"
            anchors.fill:parent
            z:-1
    }
*/
    Text {
        text: "Light Settings"
        font.pointSize: 15
        font.bold: true
        Layout.topMargin: 10
        Layout.leftMargin: 20
    }

    Switch {
        id: lightEnableSwitch
        Layout.topMargin: 10
        Layout.leftMargin: 20
        property bool isEnable: true
        text: isEnable ? qsTr("Light Enabled") : qsTr("Light Disabled")
        onClicked: {
            isEnable = !isEnable
            ModelTab.currentLight.params.enabled = isEnable
        }
    }

    Switch {
        id: lightVisibleSwitch
        Layout.topMargin: 10
        Layout.leftMargin: 20
        property bool isEnable: true
        text: isEnable ? qsTr("Light Visible") : qsTr("Light Not Visible")
        onClicked: {
            isEnable = !isEnable
            ModelTab.currentLight.params.visible = isEnable
        }
    }

    Text {
        Layout.leftMargin: 20
        Layout.topMargin: 10
        text: "Change Illumination"
        font.bold: true
    }

    Label {
        Layout.leftMargin: 20
        text: "ambient"
    }

    ColumnLayout {
        Layout.leftMargin: 30

        Switch {
            id: ambientSwitch
            property bool isEnable: true
            text: isEnable ? qsTr("Enabled") : qsTr("Disabled")
            checked: true
            onClicked: {
                isEnable =! isEnable
                ModelTab.currentLight.params.ambientEnabled = isEnable
            }
        }

        RowLayout {
            Layout.leftMargin: 10
            enabled: ambientSwitch.isEnable

            Label {
                text:
                    ModelTab.currentLight === null ? "" :
                    ModelTab.currentLight.params.ambient.x.toFixed(2) + ", "
                  + ModelTab.currentLight.params.ambient.y.toFixed(2) + ", "
                  + ModelTab.currentLight.params.ambient.z.toFixed(2) + " "
            }

            Button {
                id: selectAmbientColorButton
                background: Rectangle {
                    implicitWidth: 50
                    implicitHeight: 20
                    color: ambientSwitch.isEnable ? ambientColorDialog.currentColor: "gray"
                    border.color: "#26282a"
                    border.width: 1
                    radius: 4
                }
                onClicked: ambientColorDialog.open()
            }

            Connections {
                target: ModelTab
                function onCurrentLightChanged() {
                    ambientSwitch.isEnable = ModelTab.currentLight.params.ambientEnabled
                    ambientSwitch.checked = ambientSwitch.isEnable

                    lightEnableSwitch.isEnable = ModelTab.currentLight.params.enabled
                    lightEnableSwitch.checked = lightEnableSwitch.isEnable

                    lightVisibleSwitch.isEnable = ModelTab.currentLight.params.enabled
                    lightVisibleSwitch.checked = lightVisibleSwitch.isEnable

                    ambientColorDialog.color.r = ModelTab.currentLight.params.ambient.x
                    ambientColorDialog.color.g = ModelTab.currentLight.params.ambient.y
                    ambientColorDialog.color.b = ModelTab.currentLight.params.ambient.z
                }
            }

            ColorDialog {
                id: ambientColorDialog
                title: "Please choose ambient value"
                onAccepted: {
                    ModelTab.currentLight.params.ambient = Qt.vector3d(ambientColorDialog.currentColor.r, ambientColorDialog.currentColor.g, ambientColorDialog.currentColor.b)
                }
            }
        }
    }

    Label {
        Layout.leftMargin: 20
        text: "diffuse"
    }

    ColumnLayout {
        Layout.leftMargin: 30

        Switch {
            id: diffuseSwitch
            property bool isEnable: true
            text: isEnable ? qsTr("Enabled") : qsTr("Disabled")
            checked: true
            onClicked: {
                isEnable =! isEnable
                ModelTab.currentLight.params.diffuseEnabled = isEnable
            }
        }

        RowLayout {
            Layout.leftMargin: 10
            enabled: diffuseSwitch.isEnable

            Label {
                color: diffuseSwitch.isEnable ? "black" : "gray"
                text: ModelTab.currentLight === null ? "" :
                      ModelTab.currentLight.params.diffuse.x.toFixed(2) + ", "
                    + ModelTab.currentLight.params.diffuse.y.toFixed(2) + ", "
                    + ModelTab.currentLight.params.diffuse.z.toFixed(2) + " "
            }

            Connections {
                target: ModelTab
                function onCurrentLightChanged() {
                    diffuseSwitch.isEnable = ModelTab.currentLight.params.diffuseEnabled
                    diffuseSwitch.checked = diffuseSwitch.isEnable
                    lightEnableSwitch.isEnable = ModelTab.currentLight.params.enabled
                    lightEnableSwitch.checked = lightEnableSwitch.isEnable

                    lightVisibleSwitch.isEnable = ModelTab.currentLight.params.enabled
                    lightVisibleSwitch.checked = lightVisibleSwitch.isEnable

                    diffuseColorDialog.color.r = ModelTab.currentLight.params.diffuse.x
                    diffuseColorDialog.color.g = ModelTab.currentLight.params.diffuse.y
                    diffuseColorDialog.color.b = ModelTab.currentLight.params.diffuse.z
                }
            }

            Button {
                id: selectDiffuseColorButton
                background: Rectangle {
                    implicitWidth: 50
                    implicitHeight: 20
                    color: diffuseSwitch.isEnable ? diffuseColorDialog.currentColor : "gray"
                    border.color: "#26282a"
                    border.width: 1
                    radius: 4
                }
                onClicked: diffuseColorDialog.open()
            }

            ColorDialog {
                id: diffuseColorDialog
                title: "Please choose diffuse illumination"
                onAccepted: {
                    ModelTab.currentLight.params.diffuse = Qt.vector3d(diffuseColorDialog.currentColor.r, diffuseColorDialog.currentColor.g, diffuseColorDialog.currentColor.b)
                }
            }
        }
    }

    Label {
        Layout.leftMargin: 20
        text: "specular"
    }

    ColumnLayout {
        Layout.leftMargin: 30

        Switch {
            id: specularSwitch
            property bool isEnable: true
            text: isEnable ? qsTr("Enabled") : qsTr("Disabled")
            checked: true
            onClicked: {

                isEnable =! isEnable
                ModelTab.currentLight.params.specularEnabled = isEnable
            }
        }

        RowLayout {

            Layout.leftMargin: 10
            enabled: specularSwitch.isEnable

            Label {
                color: specularSwitch.isEnable ? "black" : "gray"
                text: ModelTab.currentLight === null ? "" :
                      ModelTab.currentLight.params.specular.x.toFixed(2) + ", "
                    + ModelTab.currentLight.params.specular.y.toFixed(2) + ", "
                    + ModelTab.currentLight.params.specular.z.toFixed(2) + " "
            }

            Connections {
                target: ModelTab
                function onCurrentLightChanged() {
                    specularSwitch.isEnable = ModelTab.currentLight.params.specularEnabled
                    specularSwitch.checked =  specularSwitch.isEnable
                    lightEnableSwitch.isEnable = ModelTab.currentLight.params.enabled
                    lightEnableSwitch.checked = lightEnableSwitch.isEnable

                    lightVisibleSwitch.isEnable = ModelTab.currentLight.params.enabled
                    lightVisibleSwitch.checked = lightVisibleSwitch.isEnable

                    specularColorDialog.color.r = ModelTab.currentLight.params.specular.x
                    specularColorDialog.color.g = ModelTab.currentLight.params.specular.y
                    specularColorDialog.color.b = ModelTab.currentLight.params.specular.z
                }
            }

            Button {
                id: selectSpecularColorButton
                background: Rectangle {
                    implicitWidth: 50
                    implicitHeight: 20
                    color: specularSwitch.isEnable ? specularColorDialog.currentColor : "gray"
                    border.color: "#26282a"
                    border.width: 1
                    radius: 4
                }
                onClicked: specularColorDialog.open()
            }

            ColorDialog {
                id: specularColorDialog
                title: "Please choose specular illumination"
                onAccepted: {
                    ModelTab.currentLight.params.specular = Qt.vector3d(specularColorDialog.currentColor.r, specularColorDialog.currentColor.g, specularColorDialog.currentColor.b)
                }
            }
        }
    }
}
