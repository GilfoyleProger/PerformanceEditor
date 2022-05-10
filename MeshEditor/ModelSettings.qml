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

Flickable{
   // width: 200; height: 200
    contentHeight: layout.height
    contentWidth: layout.width
clip:true
boundsBehavior: Flickable.StopAtBounds
    ScrollBar.vertical: ScrollBar{}
/*ScrollBar {
    id: vbar
    hoverEnabled: true
    active: true//hovered || pressed
    orientation: Qt.Vertical
    size: layout.height / content.height
    anchors.top: parent.top
    anchors.right: parent.right
    anchors.bottom: parent.bottom
}*/
ColumnLayout {
    Rectangle {
            color: "#EDF2F4" //"#F5F5F5"
            anchors.fill:parent
            z:-1
    }
id:layout
//Layout.fillHeight: true
 //   Layout.fillWidth: true
//width:330
//height: 768
    Text {
        text: "Model Settings"
        font.pointSize: 15
        font.bold: true
        Layout.topMargin: 10
        Layout.leftMargin: 20
    }

    Switch {
        id: lightingSwitch
        Layout.topMargin: 5//10
        Layout.leftMargin: 20
        property bool isEnable: true
        text: isEnable ? qsTr("Lighting Enabled") : qsTr("Lighting Disabled")
        onClicked: {
            isEnable = !isEnable
            ModelTab.lightingEnabled = isEnable
        }
    }


    Text {
        Layout.leftMargin: 20
        Layout.topMargin: 5//10
        text: "Select current material"
        font.bold: true
    }

    Rectangle {
        id: materialSelector

        Layout.leftMargin: 20
        Layout.rightMargin: 20
        Layout.bottomMargin: 10

        height: 100
        width: 300
        color: "white"

        ListView {
            id: materialList
            boundsBehavior: Flickable.StopAtBounds
            height: 100
            width: 300
            spacing: 5
            clip: true
            model: ModelTab.currentNode === null ? 0 : ModelTab.currentNode.materials

            property int currentMaterialIndex: 0

            ScrollBar.vertical: ScrollBar {
                id:scroll
                active: true
                visible: true
            }

            delegate: Item {
                width: materialSelector.width - scroll.width
                height: materialElementHeight

                property int materialElementHeight: 20

                Rectangle {
                    id: modeHighlight
                    anchors.fill: parent
                    border.color: (materialList.currentMaterialIndex === index) ? "black" : "white"
                    visible: true
                    z: -1
                }

                Text {
                    font.family: "Sans Serif"
                    text: " " + modelData.name //TODO:
                    font.pointSize: 8
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {

                        lightingSwitch.isEnable = ModelTab.lightingEnabled
                        lightingSwitch.checked = lightingSwitch.isEnable

                        materialList.currentMaterialIndex = index

                        ambientSwitch.isEnable = ModelTab.currentNode.materials[materialList.currentMaterialIndex].ambientEnabled
                        ambientSwitch.checked = ambientSwitch.isEnable
                        diffuseSwitch.isEnable = ModelTab.currentNode.materials[materialList.currentMaterialIndex].diffuseEnabled
                        diffuseSwitch.checked = diffuseSwitch.isEnable
                        specularSwitch.isEnable = ModelTab.currentNode.materials[materialList.currentMaterialIndex].specularEnabled
                        specularSwitch.checked = specularSwitch.isEnable

                        ambientColorDialog.color.r = ModelTab.currentNode.materials[materialList.currentMaterialIndex].ambient.x
                        ambientColorDialog.color.g = ModelTab.currentNode.materials[materialList.currentMaterialIndex].ambient.y
                        ambientColorDialog.color.b = ModelTab.currentNode.materials[materialList.currentMaterialIndex].ambient.z

                        diffuseColorDialog.color.r = ModelTab.currentNode.materials[materialList.currentMaterialIndex].diffuse.x
                        diffuseColorDialog.color.g = ModelTab.currentNode.materials[materialList.currentMaterialIndex].diffuse.y
                        diffuseColorDialog.color.b = ModelTab.currentNode.materials[materialList.currentMaterialIndex].diffuse.z

                        specularColorDialog.color.r = ModelTab.currentNode.materials[materialList.currentMaterialIndex].specular.x
                        specularColorDialog.color.g = ModelTab.currentNode.materials[materialList.currentMaterialIndex].specular.y
                        specularColorDialog.color.b = ModelTab.currentNode.materials[materialList.currentMaterialIndex].specular.z
                    }
                }
            }
        }
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
            text: isEnable ? qsTr("Enable") : qsTr("Disable")
            onClicked: {
                isEnable = !isEnable
                ModelTab.currentNode.materials[materialList.currentMaterialIndex].ambientEnabled = isEnable
            }
        }

        RowLayout {
            Layout.leftMargin: 10
            enabled: ambientSwitch.isEnable

            Label {
                text:
                    ModelTab.currentNode === null ? "" :
                    ModelTab.currentNode.materials[materialList.currentMaterialIndex].ambient.x.toFixed(2) + ", "
                  + ModelTab.currentNode.materials[materialList.currentMaterialIndex].ambient.y.toFixed(2) + ", "
                  + ModelTab.currentNode.materials[materialList.currentMaterialIndex].ambient.z.toFixed(2) + " "
            }

            Button {
                id: selectAmbientColorButton
                background: Rectangle {
                    implicitWidth: 50
                    implicitHeight: 20
                    color: ambientSwitch.isEnable ? ambientColorDialog.currentColor:"gray"
                    border.color: "#26282a"
                    border.width: 1
                    radius: 4
                }
                onClicked: ambientColorDialog.open()
            }

            Connections {
                target: ModelTab
                function onCurrentNodeChanged() {
                    materialList.currentMaterialIndex = 0
                    ambientSwitch.isEnable = ModelTab.currentNode.materials[materialList.currentMaterialIndex].ambientEnabled
                    ambientSwitch.checked = ambientSwitch.isEnable
                    ambientColorDialog.color.r = ModelTab.currentNode.materials[materialList.currentMaterialIndex].ambient.x
                    ambientColorDialog.color.g = ModelTab.currentNode.materials[materialList.currentMaterialIndex].ambient.y
                    ambientColorDialog.color.b = ModelTab.currentNode.materials[materialList.currentMaterialIndex].ambient.z

                    lightingSwitch.isEnable = ModelTab.lightingEnabled
                    lightingSwitch.checked = lightingSwitch.isEnable
                }
            }

            ColorDialog {
                id: ambientColorDialog
                title: "Please choose ambient value"
                onAccepted: {
                    ModelTab.currentNode.materials[materialList.currentMaterialIndex].ambient = Qt.vector3d(ambientColorDialog.currentColor.r, ambientColorDialog.currentColor.g, ambientColorDialog.currentColor.b)
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
            text: (isEnable)? qsTr("Enable") : qsTr("Disable")
            onClicked: {
                isEnable =! isEnable
                ModelTab.currentNode.materials[materialList.currentMaterialIndex].diffuseEnabled = isEnable
            }
        }

        RowLayout {
            Layout.leftMargin: 10
            enabled: diffuseSwitch.isEnable

            Label {
                color: diffuseSwitch.isEnable ? "black" : "gray"
                text: ModelTab.currentNode === null ? "" :
                      ModelTab.currentNode.materials[materialList.currentMaterialIndex].diffuse.x.toFixed(2) + ", "
                    + ModelTab.currentNode.materials[materialList.currentMaterialIndex].diffuse.y.toFixed(2) + ", "
                    + ModelTab.currentNode.materials[materialList.currentMaterialIndex].diffuse.z.toFixed(2) + " "
            }

            Connections {
                target: ModelTab
                function onCurrentNodeChanged() {
                    materialList.currentMaterialIndex = 0
                    diffuseSwitch.isEnable = ModelTab.currentNode.materials[materialList.currentMaterialIndex].diffuseEnabled
                    diffuseSwitch.checked = diffuseSwitch.isEnable
                    diffuseColorDialog.color.r = ModelTab.currentNode.materials[materialList.currentMaterialIndex].diffuse.x
                    diffuseColorDialog.color.g = ModelTab.currentNode.materials[materialList.currentMaterialIndex].diffuse.y
                    diffuseColorDialog.color.b = ModelTab.currentNode.materials[materialList.currentMaterialIndex].diffuse.z

                    lightingSwitch.isEnable = ModelTab.lightingEnabled
                    lightingSwitch.checked = lightingSwitch.isEnable
                }
            }

            Button {
                id: selectDiffuseColorButton
                background: Rectangle {
                    implicitWidth: 50
                    implicitHeight: 20
                    color: diffuseSwitch.isEnable ? diffuseColorDialog.currentColor:"gray"
                    border.color: "#26282a"
                    border.width: 1
                    radius: 4
                }
                onClicked: diffuseColorDialog.open()
            }

            ColorDialog {
                id: diffuseColorDialog
                title: "Please choose diffuse value"
                onAccepted: {
                    ModelTab.currentNode.materials[materialList.currentMaterialIndex].diffuse = Qt.vector3d(diffuseColorDialog.currentColor.r, diffuseColorDialog.currentColor.g, diffuseColorDialog.currentColor.b)
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
            text: isEnable ? qsTr("Enable") : qsTr("Disable")
            onClicked: {
                isEnable =! isEnable
                ModelTab.currentNode.materials[materialList.currentMaterialIndex].specularEnabled = isEnable
            }
        }

        RowLayout {

            Layout.leftMargin: 10
            enabled: specularSwitch.isEnable

            Label {
                color: specularSwitch.isEnable ? "black" : "gray"
                text: ModelTab.currentNode === null ? "" :
                      ModelTab.currentNode.materials[materialList.currentMaterialIndex].specular.x.toFixed(2) + ", "
                    + ModelTab.currentNode.materials[materialList.currentMaterialIndex].specular.y.toFixed(2) + ", "
                    + ModelTab.currentNode.materials[materialList.currentMaterialIndex].specular.z.toFixed(2) + " "
            }

            Connections {
                target: ModelTab
                function onCurrentNodeChanged() {
                    materialList.currentMaterialIndex = 0
                    specularSwitch.isEnable = ModelTab.currentNode.materials[materialList.currentMaterialIndex].specularEnabled
                    specularSwitch.checked = specularSwitch.isEnable
                    specularColorDialog.color.r = ModelTab.currentNode.materials[materialList.currentMaterialIndex].specular.x
                    specularColorDialog.color.g = ModelTab.currentNode.materials[materialList.currentMaterialIndex].specular.y
                    specularColorDialog.color.b = ModelTab.currentNode.materials[materialList.currentMaterialIndex].specular.z

                    lightingSwitch.isEnable = ModelTab.lightingEnabled
                    lightingSwitch.checked = lightingSwitch.isEnable
                }
            }

            Button {
                id: selectSpecularColorButton
                background: Rectangle {
                    implicitWidth: 50
                    implicitHeight: 20
                    color: specularSwitch.isEnable ? specularColorDialog.currentColor:"gray"
                    border.color: "#26282a"
                    border.width: 1
                    radius: 4
                }
                onClicked: specularColorDialog.open()
            }

            ColorDialog {
                id: specularColorDialog
                title: "Please choose specular value"
                onAccepted: {
                    ModelTab.currentNode.materials[materialList.currentMaterialIndex].specular = Qt.vector3d(specularColorDialog.currentColor.r, specularColorDialog.currentColor.g, specularColorDialog.currentColor.b)
                }
            }
        }
    }

    Label {
        Layout.leftMargin: 20
        text: "shininess"
    }

    ColumnLayout {
        Layout.leftMargin: 30

        RowLayout {
            Layout.leftMargin: 10
            enabled: specularSwitch.isEnable

            TextField {
                color: specularSwitch.isEnable ? "black" : "gray"
                implicitWidth: 100
                implicitHeight: 28
                text: ModelTab.currentNode === null ? "0.0" :
                ModelTab.currentNode.materials[materialList.currentMaterialIndex].shininess.toFixed(2)
            }
        }
    }

    Label {
        Layout.leftMargin: 20
        text: "emission"
    }

    ColumnLayout {
        Layout.leftMargin: 30
        Switch {
            id: emissionSwitch
            property bool isEnable: true
            text: isEnable ? qsTr("Enable") : qsTr("Disable")
            onClicked: {
                isEnable =! isEnable
                ModelTab.currentNode.materials[materialList.currentMaterialIndex].emissionEnabled = isEnable
            }
        }

        RowLayout {
            Layout.leftMargin: 10
            enabled: emissionSwitch.isEnable

            Label {
                color: emissionSwitch.isEnable ? "black" : "gray"
                text: ModelTab.currentNode === null ? "" :
                      ModelTab.currentNode.materials[materialList.currentMaterialIndex].emission.x.toFixed(2) + ", "
                    + ModelTab.currentNode.materials[materialList.currentMaterialIndex].emission.y.toFixed(2) + ", "
                    + ModelTab.currentNode.materials[materialList.currentMaterialIndex].emission.z.toFixed(2) + " "
            }

            Connections {
                target: ModelTab
                function onCurrentNodeChanged() {
                    materialList.currentMaterialIndex = 0
                    emissionSwitch.isEnable = ModelTab.currentNode.materials[materialList.currentMaterialIndex].emissionEnabled
                    emissionSwitch.checked = emissionSwitch.isEnable
                    emissionColorDialog.color.r = ModelTab.currentNode.materials[materialList.currentMaterialIndex].emission.x
                    emissionColorDialog.color.g = ModelTab.currentNode.materials[materialList.currentMaterialIndex].emission.y
                    emissionColorDialog.color.b = ModelTab.currentNode.materials[materialList.currentMaterialIndex].emission.z

                    lightingSwitch.isEnable = ModelTab.lightingEnabled
                    lightingSwitch.checked = lightingSwitch.isEnable
                }
            }

            Button {
                id: selectEmissionColorButton
                background: Rectangle {
                    implicitWidth: 50
                    implicitHeight: 20
                    color: emissionSwitch.isEnable ? emissionColorDialog.currentColor:"gray"
                    border.color: "#26282a"
                    border.width: 1
                    radius: 4
                }
                onClicked: emissionColorDialog.open()
            }

            ColorDialog {
                id: emissionColorDialog
                title: "Please choose specular value"
                onAccepted: {
                    ModelTab.currentNode.materials[materialList.currentMaterialIndex].emission = Qt.vector3d(emissionColorDialog.currentColor.r, emissionColorDialog.currentColor.g, emissionColorDialog.currentColor.b)
                }
            }
        }
    }



    Label {
        Layout.leftMargin: 20
        text: "diffuse map"
    }

    ColumnLayout {

        Layout.leftMargin: 30
        Switch {
            id: diffuseMapSwitch
            property bool isEnable: true
            text: isEnable ? qsTr("Enable") : qsTr("Disable")
            onClicked: {
                isEnable =! isEnable
                ModelTab.currentNode.materials[materialList.currentMaterialIndex].diffuseMapEnabled = isEnable
            }
        }

        RowLayout {
            Layout.leftMargin: 10
            enabled: diffuseMapSwitch.isEnable

            Label {
                color: diffuseMapSwitch.isEnable ? "black" : "gray"
                text:
                      ModelTab.currentNode === null ? "name: none" :
                      "name: " + ModelTab.currentNode.materials[materialList.currentMaterialIndex].diffuseMapName
            }

            Connections {
                target: ModelTab
                function onCurrentNodeChanged() {
                    materialList.currentMaterialIndex = 0
                    diffuseMapSwitch.isEnable = ModelTab.currentNode.materials[materialList.currentMaterialIndex].diffuseMapEnabled
                    diffuseMapSwitch.checked = diffuseMapSwitch.isEnable
                }
            }
        }
    }


    Label {
        Layout.leftMargin: 20
        text: "specular map"
    }

    ColumnLayout {
        Layout.leftMargin: 30
        Switch {
            id: specularMapSwitch
            property bool isEnable: true
            text: isEnable ? qsTr("Enable") : qsTr("Disable")
            onClicked: {
                isEnable =! isEnable
                ModelTab.currentNode.materials[materialList.currentMaterialIndex].specularMapEnabled = isEnable
            }
        }

        RowLayout {
            Layout.leftMargin: 10
            enabled: specularMapSwitch.isEnable

            Label {
                color: specularMapSwitch.isEnable ? "black" : "gray"
                text:  ModelTab.currentNode === null ? "name: none" :
                       "name: " + ModelTab.currentNode.materials[materialList.currentMaterialIndex].specularMapName

            }

            Connections {
                target: ModelTab
                function onCurrentNodeChanged() {
                    materialList.currentMaterialIndex = 0
                    specularMapSwitch.isEnable = ModelTab.currentNode.materials[materialList.currentMaterialIndex].specularMapEnabled
                    specularMapSwitch.checked = specularMapSwitch.isEnable
                }
            }
        }
    }

    Label {
        Layout.leftMargin: 20
        text: "normal map"
    }

    ColumnLayout {
        Layout.leftMargin: 30
        Layout.bottomMargin: 10
        Switch {
            id: normalMapSwitch
            property bool isEnable: true
            text: isEnable ? qsTr("Enable") : qsTr("Disable")
            onClicked: {
                isEnable =! isEnable
                ModelTab.currentNode.materials[materialList.currentMaterialIndex].specularMapEnabled = isEnable
            }
        }

        RowLayout {
            Layout.leftMargin: 10
            enabled: normalMapSwitch.isEnable

            Label {
                color: normalMapSwitch.isEnable ? "black" : "gray"
                text: ModelTab.currentNode === null ? "name: none" :
                      "name: " + ModelTab.currentNode.materials[materialList.currentMaterialIndex].normalMapName

            }

            Connections {
                target: ModelTab
                function onCurrentNodeChanged() {
                    materialList.currentMaterialIndex = 0
                    normalMapSwitch.isEnable = ModelTab.currentNode.materials[materialList.currentMaterialIndex].normalMapEnabled
                    normalMapSwitch.checked = normalMapSwitch.isEnable
                }
            }
        }
    }


}

}
