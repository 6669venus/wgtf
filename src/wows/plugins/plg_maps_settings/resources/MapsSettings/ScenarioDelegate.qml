import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import BWControls 1.0
import WGControls 1.0

// Component for dislaying a Scenario class from Python
Rectangle {
	id: scenarioEntry
    color: Qt.darker( palette.MainWindowColor, 1.1 )

	property variant value: itemData.Value

	WGListModel {
		id: levelsModel
		source: value.levels

		ValueExtension {}
		ColumnExtension {}
		ComponentExtension {}
		SelectionExtension {
			id: levelsModelSelection
			multiSelect: false
		}
	}

	WGColumnLayout {
		id: scenarioColumn
		anchors.fill: parent

		WGExpandingRowLayout {
			id: scenarioRow
			Layout.fillWidth: true
			Layout.fillHeight: false

			WGCheckBox {
				id: enabledCheckBox
				checkState: typeof value.enabled != 'undefined' ?
					value.enabled :
					false
			}

			Text {
				id: scenarioText
				text: typeof value.scenario == "string" ?
					value.scenario :
					typeof value.scenario
				color: palette.NeutralTextColor
			}
			Text {
				id: matchGroupText
				text: typeof value.matchGroup == "string" ?
					value.matchGroup :
					typeof value.matchGroup
				color: palette.NeutralTextColor
			}
			Text {
				id: logicText
				text: typeof value.logic == "string" ?
					value.logic :
					typeof value.logic
				color: palette.NeutralTextColor
			}
			Text {
				id: statusText
				text: typeof value.status == "string" ?
					value.status :
					typeof value.status
				color: palette.NeutralTextColor
			}
		}

		Text {
			id: levelsText
			text: "Levels"
			color: palette.NeutralTextColor
		}

		WGListView {
			id: levelsView

			model: levelsModel
			selectionExtension: levelsModelSelection
			// Implement a delegate for custom items in each column
			columnDelegates: [propertyDelegate]

			Layout.minimumHeight: 50
			Layout.minimumWidth: 100
			Layout.fillHeight: true
			Layout.fillWidth: true

			property Component propertyDelegate: Loader {
				id: propertyDelegate
				source: "LevelDelegate.qml"
			}
		}
	}
}

