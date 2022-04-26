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
    id:root
    color: "white"
    height: layout.height
    width: 100
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

            ToolTip
            {
            text:qsTr("Show/Hide Models")
            }
Layout.alignment: Qt.AlignTop
Layout.preferredHeight: 30
Layout.preferredWidth: root.width
            enabled: 0 < models.count
            background: Rectangle{
            color: showButton.enabled? "white":"#0000000"
            border.width: 1
            border.color: showButton.enabled ? "black" : "gray"
            }

            RowLayout
            {
            id: buttonContent
            anchors.fill: parent
            spacing: 0
            opacity: showButton.enabled ? 1 : 0.3

                Text{
                    id:buttonName
                    text: qsTr("Models")
                    Layout.leftMargin: 6
                    color: "black"
                }
                Image{
                    id:buttonPointer
                    source: "/icons/expand-button.png"
                    rotation: showButton.checked ? 180 : 0
                    Layout.preferredHeight: 10
                    Layout.preferredWidth: height
                }
            }
        }

        ListView
        {
        id:models
        visible: showButton.checked
        model:ModelTab.models//["Cube", "Car", "Box", "Fox"]
      //  height: 20
        Layout.maximumHeight: 90
        Layout.fillHeight: true
        Layout.fillWidth: true
        ScrollBar.vertical: ScrollBar{}



        delegate:
            Item{
            id:modelRow
            width: root.width
            height: 30

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    console.log(index)
                    root.selectedIndex = index
                    modelData.selected = true
               }
            }

            Component.onCompleted: {}

           // onClicked:{}
          //  onSelectedChanged:{}
          //  onSelectedChanged:{}
            //onSele
           // onSe
Rectangle{
id:highlight
color:"transparent"
border.color: "blue"
border.width: 5
anchors.fill: parent
visible:modelData.selected
z:-1
}
            RowLayout{
           Text{
              Layout.leftMargin: 10
            text:modelData.name
            }
            /*
            MouseArea {
               anchors.fill: parent
                onClicked: { root.selectedIndex = modelRow.index }
            }*/
            }

        }
        }
    }
}
