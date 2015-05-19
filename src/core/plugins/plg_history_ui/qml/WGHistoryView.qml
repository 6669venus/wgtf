import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import BWControls 1.0
import "qrc:/controls"


// This component is for displaying the history panel
Rectangle {
	id: root
	color: palette.MainWindowColor

	property QtObject panelProps: WGPanelProperties{}

	BWListModel {
		id : historyModel

		source : History

		ValueExtension {}
		ColumnExtension {}
		SelectionExtension {}
	}

	BWDataChangeNotifier {
		id: historySelection
		source: CurrentIndexSource
		// When the model changes, update the selection on the view
		onSourceChanged: {
			history.currentIndex = data
		}
		onDataChanged: {
			history.currentIndex = data
		}
	}


	WGFrame {
		id: mainFrame
		anchors.fill: parent

		WGColumnLayout {
			id: mainColumnLayout
			anchors.fill: parent
			anchors.margins: panelProps.standardMargin_

			//Placeholder buttons add to as required
			WGExpandingRowLayout {
				id: buttons
				Layout.preferredHeight: panelProps.rowHeight + panelProps.doubleBorder_
				Layout.fillWidth: true

				WGPushButton {
					id: clearButton
					text: "Clear"
					onClicked: {
						console.assert( historyModel.canClear(),
							"List is not modifiable" )
						historyModel.clear()
					}
				}

				Rectangle {
					id: spacer
					color: "transparent"
					Layout.fillWidth: true
				}

				WGPushButton {
					id: macroButton
					text: "Make Macro..."
					tooltip: "Select a history to make a macro."
					onClicked: {
						CreateMacro;
					}
				}
			}

			//Frame around the List View
			WGTextBoxFrame {
				id: historyFrame
				Layout.fillHeight: true
				Layout.fillWidth: true

				// History list
				WGMultiColumnListView {
					id: history
					model: historyModel
					defaultColumnDelegate: 
						"qrc:///plg_history_ui/WGCommandInstanceDelegate.qml"
					anchors.fill: parent
					anchors.margins: panelProps.standardMargin_

					// When the view changes, update the selection on the data model
					onCurrentIndexChanged: {
						historySelection.data = currentIndex
					}
					onLastIndexClickedChanged: {
						CurrentSelectedRowIndex = lastIndexClicked;
					}
				}
			}
		}
	}
}

