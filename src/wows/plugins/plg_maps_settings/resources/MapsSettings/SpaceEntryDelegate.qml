import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import BWControls 1.0
import WGControls 1.0

// Component for dislaying a Space class from Python
Item {
	id: spaceEntry

	property variant value: itemData.Value
	property variant scenarios: value.scenarios

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
		}

		//Text {
		//	text: "Scenarios"
		//	color: palette.NeutralTextColor
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

			property Component propertyDelegate: Loader {
				id: propertyDelegate
				source: "ScenarioDelegate.qml"
			}
		}
	}
}

