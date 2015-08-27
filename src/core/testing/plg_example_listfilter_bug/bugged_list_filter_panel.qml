import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls 1.0


Rectangle {
	property var title: "List Filter Bug Example"
	property var layoutHints: { 'test': 0.1 }

	property var filterTextBox_: filterTextBox

	color: palette.MainWindowColor
	
	WGListFilter {
		id: listFilter
		source: sampleDataToFilter
		filter: filterTextBox_.text

		onFilterChanged: {
			console.log("Bugged List Filter Panel: WGListFilter::onFilterChanged received.");
		}

		onSourceChanged: {
			console.log("Bugged List Filter Panel: WGListFilter::onSourceChanged received.");
		}
	}

	WGListModel {
		id: sampleDataListModel
		source: listFilter.filteredSource
		//source: sampleDataToFilter // Works just fine if you don't have filtered model as input.

		ValueExtension {}
		ColumnExtension {}
		SelectionExtension {
			id: listModelSelection
			multiSelect: true
		}

		onRowsInsertedThread: {
			console.log("Bugged List Filter Panel: WGListModel::onRowsInsertedThread received.");
		}

		onRowsRemovedThread: {
			console.log("Bugged List Filter Panel: WGListModel::onRowsRemovedThread received.");
		}

		onSourceChanged: {
			console.log("Bugged List Filter Panel: WGListModel::onSourceChanged received.");
		}
	}
	
	ColumnLayout {
		anchors.fill: parent

		Rectangle {
			id: activeFiltersRect
			Layout.fillWidth: true
			
			WGTextBox {
				id: filterTextBox
				clip: false
				Layout.fillWidth: true
			}
		}

		WGSeparator {
			id: separator
		}

		Rectangle {
			anchors.top: separator.bottom
			Layout.fillWidth: true
			color: "transparent"

			ColumnLayout {
				anchors.fill: parent

				Rectangle {
					Layout.fillWidth: true
					Layout.preferredHeight: 50
					color: "transparent"
					
					WGPushButton {
						id: clearButton
						text: "Generate New List"
						onClicked: {
							// Cache the filter text box value and clear the textbox before generating the list.
							// This will help reset the filtered indicies on the WGListFilter.
							var tempText = filterTextBox_.text;
							filterTextBox_.text = "";

							// Generate a new list. Will return true when complete.
							generateNewList;

							// Once the source has been changed, we can reapply the filter.
							filterTextBox_.text = tempText;
						}
					}
				}

				Rectangle {
					Layout.fillWidth: true
					Layout.preferredHeight: 500
					color: "transparent"

					WGListView {
						id: sampleDataListView
						model: sampleDataListModel
						anchors.fill: parent
						columnDelegates: [columnDelegate]
					}

					Component {
						id: columnDelegate

                        Item {
							Layout.fillWidth: true
							Layout.preferredHeight: defaultSpacing.minimumRowHeight

							Rectangle {
								color: "transparent"
								anchors.fill: parent

								WGLabel {
									text: itemData.Value
									anchors.fill: parent
								}
							}
                        }
					}
				}
			}
		}
	}
}