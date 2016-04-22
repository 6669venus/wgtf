import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls 1.0 as WG1
import WGControls 2.0

WG1.WGPanel {
	title: "ListModel Test"
	layoutHints: { 'test': 0.1 }

    property var sourceModel: useModel ? source : null
	color: palette.mainWindowColor
	
    property var useModel: 1
    property var topControlsHeight: 20

    Button {
        id: switchModelButton
        anchors.top: parent.top
        anchors.left: parent.left
        width: 150
        height: topControlsHeight
        text: useModel ? "Switch Model Off" : "Switch Model On"

        onClicked: {
            useModel = useModel == 0 ? 1 : 0;
        }
    }

    //Temporary code to test if model has data.
    /*ScrollView {
		anchors.top: switchModelButton.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom

		ListView {
			//leftMargin: 50
			//rightMargin: 50
			//topMargin: 50
			//bottomMargin: 50
			model: sourceModel

			delegate: Text {
				text: display
			}
		}
	}*/


	ScrollView {
		anchors.top: switchModelButton.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom

		WGListView {
			//anchors.margins: 10
			//leftMargin: 50
			//rightMargin: 50
			//topMargin: 50
			//bottomMargin: 50
			columnWidth: 50
			columnSpacing: 1
			columnDelegates: [columnDelegate, colorDelegate]
			roles: ["value"]
			model: sourceModel

			Component {
				id: colorDelegate

				Item {
					width: itemWidth
					implicitWidth: textItem.implicitWidth
					implicitHeight: 24

					Rectangle {
						id: colorItem

						anchors.fill: parent
						anchors.margins: 1
						color: {
							if (typeof itemData.value === "string")
							{
								return "transparent";
							}
						
							var colour = itemData.value;
							var r = colour > 9999 ? (colour / 10000) % 100 + 156 : 0;
							var g = colour > 99 ? (colour / 100) % 100 + 156 : 0;
							var b = colour % 100 + 156;
						
							return Qt.rgba(r / 255, g / 255, b / 255, 1);
						}
					}

					Text {
						id: textItem

						visible: typeof itemData.value === "string"
						text: typeof itemData.value === "string" ? itemData.value : ""
						color: palette.textColor
					}
				}
			}
		}
	}
}
