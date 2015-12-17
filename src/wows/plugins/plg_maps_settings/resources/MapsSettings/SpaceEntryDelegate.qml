import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import BWControls 1.0
import WGControls 1.0

// Component for dislaying a Space class from Python
Rectangle {
	id: spaceEntry
    color: palette.MainWindowColor
    border.color: Qt.lighter( palette.MainWindowColor, 20.0 )
	border.width: 1


	property variant value: itemData.Value
	property variant scenarios: value.scenarios

	//ListModel {
	//	id: fakeModel

	//	ListElement {
	//		enabled: true
	//		matchGroup: "pvp"
	//		scenarioStatus: "ok"
	//	}
	//	ListElement {
	//		enabled: true
	//		matchGroup: "pve"
	//		scenarioStatus: "ok"
	//	}
	//	ListElement {
	//		enabled: false
	//		matchGroup: "pvp"
	//		scenarioStatus: "ok"
	//	}
	//}

	WGListModel {
		id: scenariosModel
		source: scenarios

		ValueExtension {}
		ColumnExtension {}
		ComponentExtension {}
		SelectionExtension {
			id: scenariosModelSelection
			multiSelect: false
		}
	}

	WGColumnLayout {
		id: spaceEntryColumn
		anchors.fill: parent

		WGExpandingRowLayout {
			id: spaceEntryRow
			Layout.fillWidth: true
			Layout.fillHeight: false

			Text {
				id: mapName
				clip: true
				verticalAlignment: Text.AlignVCenter
				text: typeof itemData.Key == 'string' ?
					itemData.Key :
					"unknown"
				color: palette.TextColor
			}

			WGCheckBox {
				id: enabledCheckBox
				text: "Enabled"
				checkState: typeof value.isEnabled != 'undefined' ?
					value.isEnabled : false
			}

			//Text {
			//	id: enabledCheckBox
			//	text: "Enabled" + typeof value.isEnabled != 'undefined' ?
			//		value.isEnabled : "unknown"
			//	color: palette.TextColor
			//}
		}

		//Text {
		//	text: "Scenarios"
		//	color: palette.NeutralTextColor
		//}

		//ListView {
		//	id: fakeListView
		//	Layout.fillWidth: true
		//	Layout.fillHeight: true

		//	model: fakeModel
		//	delegate: Rectangle {
		//		color: "green"
		//		anchors.left: parent.left
		//		anchors.right: parent.right
		//		height: childRow.height

		//		Row {
		//			id:childRow
		//			Text {
		//				text: "Enabled: " + enabled
		//				color: palette.NeutralTextColor
		//			}
		//			Text {
		//				text: "Match Group " + matchGroup
		//				color: palette.NeutralTextColor
		//			}
		//			Text {
		//				text: "Status " + scenarioStatus
		//				color: palette.NeutralTextColor
		//			}
		//		}
		//	}
		//}


		WGListView {
			id: scenariosView

			model: scenariosModel
			selectionExtension: scenariosModelSelection
			// Implement a delegate for custom items in each column
			columnDelegates: [propertyDelegate]

			Layout.minimumHeight: 50
			Layout.minimumWidth: 100
			Layout.fillHeight: true
			Layout.fillWidth: true

			minimumRowHeight: 150
			// Delegate to use Reflected components for the second column.
			//property Component propertyDelegate: Loader {
			//	clip: true
			//	sourceComponent: itemData != null ? itemData.Component : null
			//}

			property Component propertyDelegate: Loader {
				id: propertyDelegate
				source: "ScenarioDelegate.qml"
			}
		}
	}
}

