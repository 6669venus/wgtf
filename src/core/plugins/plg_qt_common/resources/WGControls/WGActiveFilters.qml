import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls 1.0

Rectangle {
	id: rootFrame

	// Public properties
	property var dataModel
	property var stringValue: internalStringValue

	// Locals for referencing interior fields
	property var internalStringValue: ""
	property var filterText_: filterText

	// Apperance Settings
	anchors.fill: parent
	anchors.margins: defaultSpacing.standardMargin
	color: palette.MainWindowColor
		
	//------------------------------------------
	// Functions
	//------------------------------------------

	// Handles the addition of a new filter to the active filters list
	function addFilter( text ) {
		rootFrame.dataModel.addFilter = text;
		filterText.text = "";
	}

	// Handles updating the string value when the active filters list model 
	// has been changed (additions or removals)
	function updateStringValue() {
		var combinedStr = "";
		var iteration = 0;
		var filtersIter = iterator( rootFrame.dataModel.filters );
		while (filtersIter.moveNext()) {		
			if (iteration != 0) {
				combinedStr += " ";
			}
			
			combinedStr += filtersIter.current;
			++iteration;
		}

		internalStringValue = combinedStr;
	}
	
	//------------------------------------------
	// List View Model for Active Filters
	//------------------------------------------
	WGListModel {
		id: filtersModel
		source: rootFrame.dataModel.filters

		onRowsInsertedThread: {
			updateStringValue();
		}

		onRowsRemovedThread: {
			updateStringValue();
		}

		ValueExtension {}

        ColumnExtension {}
        ComponentExtension {}
        TreeExtension {}
        ThumbnailExtension {}
		SelectionExtension {}
	}
	
	//------------------------------------------
	// Main Layout
	//------------------------------------------
	ColumnLayout {
		id: mainRowLayout
		anchors.fill: parent
		height: defaultSpacing.minimumRowHeight + defaultSpacing.doubleBorderSize
	
		Rectangle {
			id: mainColumnRect
			Layout.fillWidth: true
			Layout.preferredHeight: defaultSpacing.minimumRowHeight + defaultSpacing.doubleBorderSize
			color: "transparent"

			ColumnLayout {
				id: mainColumn
				anchors.fill: parent
				anchors.margins: defaultSpacing.standardMargin

				//------------------------------------------
				// Top Row - Text Area and Buttons
				//------------------------------------------
				WGExpandingRowLayout {
					id: inputRow
					Layout.fillWidth: true
					Layout.preferredHeight: defaultSpacing.minimumRowHeight + defaultSpacing.doubleBorderSize

					WGTextBox {
						id: filterText
						Layout.fillWidth: true
						noFrame_: false
						placeholderText: "Filter"

						Keys.onReturnPressed: {
							addFilter( text );
						}

						Keys.onEnterPressed: {
							addFilter( text );
						}
					}

					WGToolButton {
						id: addFilterButton
						iconSource: "qrc:///icons/add_16x16"
						noFrame_: false

						tooltip: "Add Filter"

						onClicked: {
							addFilter( filterText_.text );
						}
					}

					WGToolButton {
						id: clearFiltersButton
						iconSource: "qrc:///icons/delete_16x16"
						noFrame_: false

						tooltip: "Clear Filters"

						onClicked: {
							rootFrame.dataModel.clearFilters;
							rootFrame.internalStringValue = "";
						}
					}

					WGToolButton {
						id: saveFiltersButton
						iconSource: "qrc:///icons/save_16x16"
						noFrame_: false

						tooltip: "Save Filters"

						onClicked: {
							//TODO
							console.log("WGActiveFilters - saving coming soon!");
						}
					}

					WGToolButton {
						id: loadFiltersButton
						iconSource: "qrc:///icons/open_16x16"
						noFrame_: false

						tooltip: "Load Filters"

						onClicked: {
							//TODO
							console.log("WGActiveFilters - loading coming soon!");
						}
					}
				} // inputRow
			} // mainColumn
		} // mainColumnRect

		//------------------------------------------
		// Bottom Area with Filter Entries
		//------------------------------------------
		Rectangle {
			id: activeFiltersLayoutRect
			Layout.fillWidth: false
			Layout.preferredHeight: defaultSpacing.minimumRowHeight + defaultSpacing.doubleBorderSize
			color: "transparent"

			WGExpandingRowLayout {
				id: activeFiltersLayout
				anchors.fill: parent
				Layout.preferredHeight: defaultSpacing.minimumRowHeight

				Repeater {
					model: filtersModel
					delegate: Row {
						WGLabel { 
							id: activeFilterLabel
							Layout.fillWidth: false
							Layout.preferredHeight: defaultSpacing.minimumRowHeight
							
							text: Value

							font.bold: false
							font.pointSize: 10

							color: palette.TextColor

							MouseArea {
								id: activeFilterMouseArea
								anchors.fill: parent
								cursorShape: Qt.PointingHandCursor

								onPressed: {
									//TODO: Real handling for the mouse click to remove 
									//       (likely should be in a child button - leave up to artists to decide)
									// JIRA: http://jira.bigworldtech.com/browse/NGT-887
									rootFrame.dataModel.removeFilter = index;
								}
							}
						}
					}
				}
			} // activeFiltersLayout
		} // activeFiltersLayoutRect
	} // mainRowLayout
} // rootFrame