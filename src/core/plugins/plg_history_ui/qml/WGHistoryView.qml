import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import BWControls 1.0
import WGControls 1.0

// This component is for displaying the history panel
Rectangle {
	id: root
	color: palette.MainWindowColor

	property var title: "History"
	property alias historySelectionExtension: historyModelSelectionExtension

	WGListModel {
		id : historyModel
		source : History

		ValueExtension {}
		ColumnExtension {}
		SelectionExtension {
			id: historyModelSelectionExtension
			multiSelect: true
			onSelectionChanged: {
				historySelectionHelper.select( getSelection() );
			}
		}
	}

	SelectionHelper {
		id: historySelectionHelper
		source: SelectionHandlerSource
		onSourceChanged: {
			select( historyModelSelectionExtension.getSelection() );
		}
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
			anchors.margins: defaultSpacing.standardMargin

			//Placeholder buttons add to as required
			WGExpandingRowLayout {
				id: buttons
				Layout.preferredHeight: defaultSpacing.minimumRowHeight + defaultSpacing.doubleBorderSize
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
				WGListView {
					id: history
					model: historyModel
					anchors.fill: parent
					anchors.margins: defaultSpacing.standardMargin
					selectionExtension: root.historySelectionExtension
					columnDelegates: [columnDelegate]

					Component {
						id: columnDelegate

						Loader {
							source: "qrc:///plg_history_ui/WGTimelineEntryDelegate.qml"
						}
					}

					onCurrentIndexChanged: {
						historySelection.data = currentIndex
					}
					
					onRowDoubleClicked: {
						history.currentIndex = historyModel.indexRow(modelIndex);
					}
				}
			}
		}
	}
}

