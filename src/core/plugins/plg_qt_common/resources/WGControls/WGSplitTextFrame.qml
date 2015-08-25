import QtQuick 2.3
import QtQuick.Layouts 1.1

//Frame broken up into X textboxes
//Can use separators or decimal places between each box

WGTextBoxFrame {

    id: mainFrame

	property list<QtObject> boxList

	property bool decimalSeparator: false //use a '.' instead of a '|' between fields

	property int totalBoxes : boxList.length //number of textboxes

    property string label_: ""

	property int totalWidth: 0

	property bool evenBoxes: true

    implicitHeight: {
        if (defaultSpacing.minimumRowHeight){
            defaultSpacing.minimumRowHeight
        } else {
            22
        }
    }

	implicitWidth: 40 * boxList.length

	Layout.preferredWidth: totalWidth
	width: totalWidth

	//TODO give this frame a disabled state

	Row {
		Repeater {
			model: boxList
			Rectangle {
				id: boxContainer
				width: {
					if(evenBoxes)
					{
						mainFrame.width / boxList.length
					}
					else
					{
						model.modelData.width
					}
				}

				height: mainFrame.height
				color: "transparent"

				Component.onCompleted: {
					if(boxList[index].text != undefined)
					{
						totalWidth += boxList[index].width
						boxList[index].noFrame_ = true
						boxList[index].horizontalAlignment = Text.AlignHCenter
						boxList[index].parent = this
						boxList[index].anchors.fill = boxContainer
					}
					else if (boxList[index].value != undefined)
					{
						totalWidth += boxList[index].width
						boxList[index].noFrame_ = true
						boxList[index].noArrows_ = true
						boxList[index].horizontalAlignment = Text.AlignHCenter
						boxList[index].parent = this
						boxList[index].anchors.fill = boxContainer
					}
					else
					{
						boxContainer.color = "red"
					}

				}
				Component {
					id: numBox
					WGSpinBox {
						width: evenBoxes ? mainFrame.width / boxList.length : model.modelData.width
						Component.onCompleted: {
							totalWidth += model.modelData.width
						}
						height: mainFrame.height
						value: model.modelData.value
						minimumValue: model.modelData.minimumValue
						maximumValue: model.modelData.maximumValue

						noFrame_: true
						noArrows_: true
						horizontalAlignment: Text.AlignHCenter
					}
				}
				Component {
					id: textBox
					WGTextBox {
						width: evenBoxes ? mainFrame.width / boxList.length : model.modelData.width
						height: mainFrame.height
						text: model.modelData.text
						placeholderText: model.modelData.placeholderText

						noFrame_: true
						horizontalAlignment: Text.AlignHCenter

						Component.onCompleted: {
							totalWidth += model.modelData.width
						}
					}
				}
				Component {
					id: error
					Rectangle {
						width: evenBoxes ? mainFrame.width / boxList.length : model.modelData.width
						height: mainFrame.height
						color: "red"
						Component.onCompleted: {
							totalWidth += model.modelData.width
						}
						Text {
							anchors.centerIn: parent
							font.bold: true
							color: "white"
							text: "!"
						}
					}
				}

				WGLabel {
					anchors.horizontalCenter: parent.left
					height: mainFrame.height - defaultSpacing.doubleBorderSize
					y: 3
					text: "."
					visible: {
						if (index != 0 && decimalSeparator){
							true
						} else {
							false
						}
					}
				}

				WGSeparator {
					anchors.horizontalCenter: parent.left
					height: mainFrame.height - defaultSpacing.doubleBorderSize
					anchors.verticalCenter: parent.verticalCenter
					vertical_: true

					//first separator is invisible
					visible: {
						if (index != 0 && !decimalSeparator){
							true
						} else {
							false
						}
					}
				}
			}
		}
	}
}
