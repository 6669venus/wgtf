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

    /*! This property makes the filter tags appear to the left of the search text instead of below it.
        The default value is false
    */
    property bool inlineTags: false

    /*! \internal */
    property int _currentFilterWidth: 0

    /*! \internal */
    property int _filterTags: 0

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

			if (filtersIter.current.active == true) {
				combinedStr += filtersIter.current.value;
				++iteration;
			}
        }

        internalStringValue = combinedStr;
    }

    signal changeFilterWidth(int filterWidth, bool add)

    onChangeFilterWidth: {
        if(add)
        {
            _currentFilterWidth += filterWidth
            _filterTags += 1
        }
        else
        {
            _currentFilterWidth -= filterWidth
            _filterTags -= 1
        }
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
            Layout.preferredHeight: childrenRect.height
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            WGTextBoxFrame {
                id: textFrame
                Layout.fillWidth: true
                Layout.preferredHeight: childrenRect.height + defaultSpacing.standardBorderSize
                Layout.maximumHeight: childrenRect.height + defaultSpacing.standardBorderSize
                Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                WGExpandingRowLayout {
                    anchors {left: parent.left; top: parent.top; right: parent.right}
                    height: _filterTags > 0 ? childrenRect.height : defaultSpacing.minimumRowHeight

                    Loader {
                        id: activeFiltersInlineRect
                        visible: _filterTags > 0 && inlineTags
                        Layout.preferredWidth: _currentFilterWidth + (defaultSpacing.rowSpacing * _filterTags) + defaultSpacing.rowSpacing
                        Layout.maximumWidth: textFrame.width / 2
                        Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                        sourceComponent: inlineTags ? filterTagList : null
                    } // activeFiltersLayoutRect

                    WGTextBox {
                        id: filterText
                        Layout.fillWidth: true
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight
                        noFrame_: true
                        placeholderText: "Filter"
                        Layout.alignment: Qt.AlignLeft | Qt.AlignTop

                        Keys.onReturnPressed: {
                            addFilter( text );
                        }

                        Keys.onEnterPressed: {
                            addFilter( text );
                        }

                    }
                    WGToolButton {
                        id: clearFiltersButton
                        iconSource: "qrc:///icons/close_sml_16x16"

                        tooltip: "Clear Filters"
                        Layout.alignment: Qt.AlignLeft | Qt.AlignTop

                        onClicked: {
                            rootFrame.dataModel.clearFilters;
                            rootFrame.internalStringValue = "";
                            _currentFilterWidth = 0
                            _filterTags = 0
                        }
                    }
                }
            }

            WGToolButton {
                id: addFilterButton
                iconSource: "qrc:///icons/add_16x16"
                Layout.alignment: Qt.AlignLeft | Qt.AlignTop

                tooltip: "Add Filter"

                onClicked: {
                    addFilter( filterText_.text );
                }
            }
            WGToolButton {
                id: saveFiltersButton
                iconSource: "qrc:///icons/save_16x16"
                tooltip: "Save Filters"
                Layout.alignment: Qt.AlignLeft | Qt.AlignTop

                onClicked: {
                    //TODO
                    console.log("WGActiveFilters - saving coming soon!");
                }
            }

            WGToolButton {
                id: loadFiltersButton
                iconSource: "qrc:///icons/open_16x16"
                Layout.alignment: Qt.AlignLeft | Qt.AlignTop

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
        Loader {
            id: activeFiltersBelowLoader
            visible: !inlineTags && _filterTags > 0
            Layout.fillWidth: true
            sourceComponent: inlineTags ? null : filterTagList
        }

        Rectangle {
            id: spacer
            visible: !inlineTags && _filterTags > 0
            Layout.fillWidth: true
            Layout.minimumHeight: defaultSpacing.doubleBorderSize
            color: "transparent"
        }

        Component {
            id: filterTagList
            Flow {
                id: activeFiltersLayout
                anchors {left: parent.left; top: parent.top; right: parent.right}
                spacing: defaultSpacing.rowSpacing

                Repeater {
                    id: filterRepeater
                    model: filtersModel
                    onItemAdded: {
                        changeFilterWidth(item.width, true)
                    }
                    onItemRemoved: {
                        changeFilterWidth(item.width, false)
                    }

                    delegate: WGButtonBar {
                        showSeparators: false
                        evenBoxes: false
                        buttonList: [
                            WGPushButton {
                                id: filterString
                                text: Value.value
                                textCheckedHighlight: true
                                noFrame_: true
                                checkable: true
                                checked: Value.active
                                activeFocusOnPress: false

                                onClicked: {
                                    Value.active = !(Value.active);
									updateStringValue();
                                }
                            },
                            WGPushButton {
                                id: closeButton
                                iconSource: "qrc:///icons/close_sml_16x16"
                                width: height + defaultSpacing.doubleMargin
                                noFrame_: true
                                activeFocusOnPress: false

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
            } // activeFiltersBelowRect
        } //filterTags
    } // mainRowLayout
} // rootFrame
