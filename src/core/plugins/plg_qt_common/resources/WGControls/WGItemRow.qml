import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import BWControls 1.0

Item {
	id: itemRow
    objectName: "WGItemRow"

	width: childrenRect.width != 0 ? childrenRect.width : 1024
	height: childrenRect.height != 0 ? childrenRect.height : 1024

	property var columnDelegates: []
    property var columnSequence: []
	property var columnWidths: []
	property alias columnSpacing: row.spacing

	property bool selected: false

    /*! Stores which item is currently in focus by the keyboard.
        Often this will correspond to the selected item, but not always.
        E.g. pressing ctrl+up will move the current index, but not the selected index.
    */
    property bool isCurrent: false
	signal itemPressed(var mouse, var itemIndex)
    signal itemClicked(var mouse, var itemIndex)
    signal itemDoubleClicked(var mouse, var itemIndex)

	/* MOVE INTO STYLE*/
	Rectangle {
		id: backgroundArea
		anchors.fill: row
        color: palette.highlightShade
		opacity: selected ? 1 : 0.5 
        visible: hoverArea.containsMouse || selected
    }

	MouseArea {
		id: hoverArea
		anchors.fill: backgroundArea
		hoverEnabled: true
	}
    // Keyboard focus highlight
    Rectangle {
        id: keyboardFocusArea
        anchors.fill: row
        color: palette.highlightShade
        opacity: 0.25
        visible: isCurrent
    }
	/**/

	Row {
		id: row

		Repeater {
			model: SequenceList {
				model: columnModel
				sequence: columnSequence
			}

			Item {
				width: columnWidths[index]
				height: childrenRect.height
				clip: true

				MouseArea {
					width: columnWidths[index]
					height: row.height
					acceptedButtons: Qt.RightButton | Qt.LeftButton;

					onPressed: itemPressed(mouse, modelIndex)
					onClicked: itemClicked(mouse, modelIndex)
					onDoubleClicked: itemDoubleClicked(mouse, modelIndex)
				}

				Row {
					
					/* MOVE INTO STYLE*/
					Row {
						id: iconArea
						anchors.verticalCenter: parent.verticalCenter

						width: childrenRect.width
						height: childrenRect.height

						visible: __isTree && index == 0

						property bool __isTree: typeof expanded != "undefined"
						property real __depth: __isTree ? depth : 0
						property bool __hasChildren: __isTree ? hasChildren : false
						property bool __expanded: __isTree ? expanded : false

						Item {
							width: iconArea.__depth * 10
							height: 1
						}

						Text {
							color: iconArea.__hasChildren ? iconArea.__expanded ? palette.textColor : palette.neutralTextColor : "transparent"
							font.family : "Marlett"
							text : iconArea.__expanded ? "\uF036" : "\uF034"
							verticalAlignment: Text.AlignVCenter
							horizontalAlignment: Text.AlignHCenter

							MouseArea {
								anchors.fill: parent
								enabled: iconArea.__hasChildren
								onPressed: {
									expanded = !expanded
								}
							}
						}
					}
					/**/

					Loader {
						property var itemData: model
						property var itemWidth: columnWidths[index] - x
                        property var isCurrent: itemRow.isCurrent
						sourceComponent: itemRow.columnDelegates[index]
					}
				}
			}
		}
    }
}
