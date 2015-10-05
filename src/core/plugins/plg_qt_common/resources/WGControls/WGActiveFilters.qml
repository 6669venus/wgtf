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

    property alias inlineFilters: activeFiltersInlineRect.visible

    /*! This property makes the filter tags appear to the left of the search text instead of below it.
        The default value is false
    */
    property bool inlineTags: true

    /*  This property holds the original inlineTags value.
        inLineTags can change and the original state must be kept. */
    /*! \internal */
    property bool _originalInlineTagSetting: false

    /*! \internal */
    property int _currentFilterWidth: 0

    /*! \internal */
    property int _filterTags: 0

    /*! \internal */
    // This property holds the flip state between filter tags being drawn inline or on a new line
    property bool _changeLayout: false

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

    Component.onCompleted: {
        _originalInlineTagSetting = inlineTags
    }

    onChangeFilterWidth: {
        if(add)
        {
            _currentFilterWidth += filterWidth
            _filterTags += 1
            if (_originalInlineTagSetting == true && inlineTags == true)
            {
                // are the filters taking up more than half the space?
                if (_currentFilterWidth > textFrame.width / 2)
                {
                    _filterTags = 0
                    _currentFilterWidth = 0
                    inlineTags = false
                }
            }
        }
        else
        {
            _currentFilterWidth -= filterWidth
            _filterTags -= 1
            if (inlineTags == false)
            {
                if (_currentFilterWidth > (textFrame.width / 2))
                {
                    inlineTags = false
                }
                else
                {
                    _filterTags = 0 // the list is rebuilt and this needs resetting
                    _currentFilterWidth = 0  // the list is rebuilt and this needs resetting

                    inlineTags = true
                }
            }
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
                color: palette.TextBoxColor
                Layout.fillWidth: true
                Layout.preferredHeight: childrenRect.height + defaultSpacing.standardBorderSize
                Layout.maximumHeight: childrenRect.height + defaultSpacing.standardBorderSize
                Layout.alignment: Qt.AlignLeft | Qt.AlignTop

                // can only be a single row

                WGExpandingRowLayout {
                    id: inputLine
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


                    Image {
                        id: filterIcon
                        source: "qrc:///icons/filter_16x16"
                        opacity: 0.5
                        Layout.preferredHeight: paintedHeight
                        Layout.preferredWidth: paintedWidth
                    }

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
                                checkable: true
                                checkState: Value.active
                                style: WGTagButtonStyle{}

                                Binding {
                                    target: Value
                                    property: "active"
                                    value: filterString.checkState
                                }

                                Connections {
                                    target: Value
                                    onActiveChanged: {
                                        updateStringValue();
                                    }
                                }
                            },
                            WGToolButton {
                                id: closeButton
                                iconSource: "qrc:///icons/close_sml_16x16"

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
