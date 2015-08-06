import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.2

CheckBox{
	id: baseContainer
	objectName: "childCheckBox"
    anchors.fill: parent

    //TODO: Link to proper palette color
    property color colorHighlight_ : "#4097e9"

	onClicked : {
		baseContainer.parent.setChecked( baseContainer.checkedState == Qt.Checked )
    }
    style: CheckBoxStyle {
        indicator: Rectangle {
            id: checkboxFrame
            implicitWidth: 14
            implicitHeight: 14
            radius: 3
            gradient: Gradient {
                         GradientStop { position: 0.0; color: "#20000000" }
                         GradientStop { position: 1.0; color: "#40000000" }
                     }
            border.color: "#40000000"
            border.width: 1
            Rectangle {
                visible: control.checkedState != Qt.Unchecked // visible if checked or partially checked
                color: control.checkedState == Qt.Checked ? Qt.rgba(colorHighlight_.r,colorHighlight_.g,colorHighlight_.b,0.7) : Qt.rgba(colorHighlight_.r,colorHighlight_.g,colorHighlight_.b,0.5)
                radius: 3
                anchors.fill: parent
                anchors.margins: control.checkedState == Qt.Checked ? 1 : 2 //smaller blue square if partially checked
                Rectangle {
                    visible: control.checkedState == Qt.Checked //invisible if partially checked
                    color: colorHighlight_
                    anchors.fill: parent
                    anchors.margins: 1
                    radius: 3
                }

                Text {
                    id : checkOn
                    color : "#FFFFFF"
                    font.family : "Marlett"
                    font.pixelSize: checkboxFrame.height + 4
                    text : "a"
                    visible : control.checkedState == Qt.Checked //invisible if partially checked
                    anchors.fill: parent
                    anchors.leftMargin: 1
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                }
            }
        }
    }
}

