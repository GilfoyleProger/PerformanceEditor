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


id:layout

    Text {
        text: "Background Settings"
        font.pointSize: 15
        font.bold: true
        Layout.topMargin: 10
        Layout.leftMargin: 20
    }


    Label {
        Layout.topMargin: 10
        Layout.leftMargin: 20
        text: "Select Color"
    }

    ColumnLayout {
        Layout.leftMargin: 30
        /*Switch {
            id: emissionSwitch
            property bool isEnable: true
            text: isEnable ? qsTr("Enable") : qsTr("Disable")
            onClicked: {
                isEnable =! isEnable
                //ModelTab.currentNode.materials[materialList.currentMaterialIndex].emissionEnabled = isEnable
            }
        }*/

        RowLayout {
            Layout.leftMargin: 10
            //enabled: emissionSwitch.isEnable

            Label {
                color: "black"//emissionSwitch.isEnable ? "black" : "gray"
                text: ModelTab.backgroundColor === null ? "" :
                      ModelTab.backgroundColor.x.toFixed(2) + ", "
                    + ModelTab.backgroundColor.y.toFixed(2) + ", "
                    + ModelTab.backgroundColor.z.toFixed(2) + " "
            }
Component.onCompleted:{
    emissionColorDialog.color.r = ModelTab.backgroundColor.x
    emissionColorDialog.color.g = ModelTab.backgroundColor.y
    emissionColorDialog.color.b = ModelTab.backgroundColor.z

}
            Connections {
                target: ModelTab
                function onCurrentNodeChanged() {
                    emissionColorDialog.color.r = ModelTab.backgroundColor.x
                    emissionColorDialog.color.g = ModelTab.backgroundColor.y
                    emissionColorDialog.color.b = ModelTab.backgroundColor.z
                  //  materialList.currentMaterialIndex = 0
                   // emissionSwitch.isEnable = ModelTab.currentNode.materials[materialList.currentMaterialIndex].emissionEnabled
                   //// emissionSwitch.checked = emissionSwitch.isEnable
                   // emissionColorDialog.color.r = ModelTab.currentNode.materials[materialList.currentMaterialIndex].emission.x
                   // emissionColorDialog.color.g = ModelTab.currentNode.materials[materialList.currentMaterialIndex].emission.y
                   // emissionColorDialog.color.b = ModelTab.currentNode.materials[materialList.currentMaterialIndex].emission.z

                   // lightingSwitch.isEnable = ModelTab.lightingEnabled
                   // lightingSwitch.checked = lightingSwitch.isEnable
                }
            }

            Button {
                id: selectEmissionColorButton
                background: Rectangle {
                    implicitWidth: 50
                    implicitHeight: 20
                    color: emissionColorDialog.currentColor//emissionSwitch.isEnable ? emissionColorDialog.currentColor:"gray"
                    border.color: "#26282a"
                    border.width: 1
                    radius: 4
                }
                onClicked: emissionColorDialog.open()
            }

            ColorDialog {
                id: emissionColorDialog
                title: "Please choose background color"
                onAccepted: {
                    ModelTab.backgroundColor = Qt.vector3d(emissionColorDialog.currentColor.r, emissionColorDialog.currentColor.g, emissionColorDialog.currentColor.b)
                }
            }
        }
    }


}


