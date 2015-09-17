import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls 1.0

//TODO: Style and improve documentation.

/*!
 \brief A control used to represent the presence of a filter.

Example:
\code{.js}
WGActiveFilters {
    id: activeFilters
    dataModel: filtersModel
}
\endcode
*/

Item {
    id: rootFrame

    // Public properties
    /*! This property holds the dataModel containing all filters */
    property var dataModel
    /*! This property holds the filter string
        The default value is an empty string
    */
    property var stringValue: internalStringValue

    // Locals for referencing interior fields
    /*! \internal */
    property var internalStringValue: ""

    /*! \internal */
    property var filterText_: filterText

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
        anchors {left: parent.left; top: parent.top; right: parent.right}
        anchors.margins: defaultSpacing.standardMargin

        //------------------------------------------
        // Top Row - Text Area and Buttons
        //------------------------------------------
        WGExpandingRowLayout {
            id: inputRow
            Layout.fillWidth: true
            Layout.minimumHeight: defaultSpacing.minimumRowHeight + defaultSpacing.doubleBorderSize
            Layout.preferredHeight: defaultSpacing.minimumRowHeight + defaultSpacing.doubleBorderSize

            WGTextBox {
                id: filterText
                Layout.fillWidth: true
                Layout.preferredHeight: defaultSpacing.minimumRowHeight
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

                tooltip: "Add Filter"

                onClicked: {
                    addFilter( filterText_.text );
                }
            }

            WGToolButton {
                id: clearFiltersButton
                iconSource: "qrc:///icons/delete_16x16"

                tooltip: "Clear Filters"

                onClicked: {
                    rootFrame.dataModel.clearFilters;
                    rootFrame.internalStringValue = "";
                }
            }

            WGToolButton {
                id: saveFiltersButton
                iconSource: "qrc:///icons/save_16x16"
                tooltip: "Save Filters"

                onClicked: {
                    //TODO
                    console.log("WGActiveFilters - saving coming soon!");
                }
            }

            WGToolButton {
                id: loadFiltersButton
                iconSource: "qrc:///icons/open_16x16"

                tooltip: "Load Filters"

                onClicked: {
                    //TODO
                    console.log("WGActiveFilters - loading coming soon!");
                }
            }
        } // inputRow

        //------------------------------------------
        // Bottom Area with Filter Entries
        //------------------------------------------
        Item {
            id: activeFiltersLayoutRect
            Layout.fillWidth: true
            Layout.preferredHeight: filterRepeater.count > 0 ? childrenRect.height + defaultSpacing.standardMargin : 0

            Flow {
                id: activeFiltersLayout
                anchors {left: parent.left; top: parent.top; right: parent.right}
                spacing: defaultSpacing.rowSpacing

                Repeater {
                    id: filterRepeater
                    model: filtersModel
                    delegate: WGButtonBar {
                        evenBoxes: false
                        buttonList: [
                            WGPushButton {
                                id: filterString
                                text: Value
                                checkable: true
                                checked: true

                                onClicked: {
                                    //TODO: Toggle filter on and off by pressing label
                                }
                            },
                            WGPushButton {
                                id: closeButton
                                iconSource: "qrc:///icons/close_16x16"

                                onClicked: {
                                    //TODO: Real handling for the mouse click to remove
                                    //       (likely should be in a child button - leave up to artists to decide)
                                    // JIRA: http://jira.bigworldtech.com/browse/NGT-887
                                    rootFrame.dataModel.removeFilter = index;
                                }
                            }
                        ]

                    }
                }
            } // activeFiltersLayout
        } // activeFiltersLayoutRect
    } // mainRowLayout
} // rootFrame
