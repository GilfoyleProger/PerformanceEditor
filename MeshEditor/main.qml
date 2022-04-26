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
    width: 1366
    height: 768
    color: "white"

    property int previousIndex: 0

    RowLayout {

        FileDialog {
            id: fileDialog
            title: "Please choose a file"
            folder: shortcuts.home
            onAccepted: {
                renderSystem.loadModel(fileDialog.fileUrl);
                console.log("You chose: " + fileDialog.fileUrls)
                Qt.quit()
            }
            onRejected: {
                console.log("Canceled")
                Qt.quit()
            }
        }

        GLRenderSystem {
            id: renderSystem
            width: 960
            height: 720
            Layout.margins: 15
            focus: true

            ColumnLayout {
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.topMargin: 20;
                anchors.leftMargin: 20;
                spacing: 5

                RowLayout {
                    spacing: 20

                    Button {
                        text: qsTr("Export")
                        onClicked: {
                            fileDialog.open();
                        }
                    }

                    Button {
                        text: qsTr("Import")
                        onClicked: {
                            fileDialog.open();
                        }
                    }
                }

                SceneModelList {
                    id: modelList
                    Layout.topMargin: 20
                }

                SceneLightList {
                    //Layout.alignment: Qt.AlignTop
                    anchors.top: modelList.bottom
                    anchors.topMargin: 20;
                }
            }
        }

        ColumnLayout {
            Layout.topMargin: 15
            Layout.bottomMargin: 16
            focus: true

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onEntered: {
                    renderSystem.forceActiveFocus();
                }
            }

            TabBar {
                id: bar
                Layout.fillWidth: true

                TabButton {
                    text: "Solid"
                    width: 110
                }

                TabButton {
                    text: "Light"
                    width: 110
                }

                TabButton {
                    text: "Background"
                    width: 110
                }

                onCurrentIndexChanged: {
                    previousIndex = controls.currentIndex
                    controls.currentIndex = currentIndex
                }
            }

            StackLayout {
                id: controls
                currentIndex: 0

                Rectangle {
                    color: "#EDF2F4"
                    implicitWidth: 330
                    implicitHeight: 200
                    ModelSettings{}
                }

                Rectangle {
                    color: '#EDF2F4'
                    implicitWidth: 330
                    implicitHeight: 200
                    LightSettings{}
                }

                Rectangle {
                    color: '#EDF2F4'
                    implicitWidth: 330
                    implicitHeight: 200
                }
            }
        }
    }
}
