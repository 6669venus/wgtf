import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls 1.0

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
        When the search tags exceed half the width of the search field they are moved to a flow layout beneath the search text field
        The default value is true
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
        //remove extra whitespace at start and end and check string contains some characters
        text = text.trim()
        if (text != "")
        {
            rootFrame.dataModel.addFilter(text);
            filterText.text = "";
        }
        else
        {
            filterText.text = "";
        }
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

    // moves active filters to and from a flow layout when containter is resized
    function checkActiveFilterSize(){
        if (_originalInlineTagSetting && inlineTags)
        {
            if (_currentFilterWidth > textFrame.width / 2)
            {
                _filterTags = 0 // all filter tags are rebuilt when inlineTags changes, must reset value
                _currentFilterWidth = 0
                inlineTags = false // move to flow layout
            }
        }
        if (_originalInlineTagSetting && !inlineTags)
        {
            if (_currentFilterWidth < textFrame.width / 2)
            {
                _filterTags = 0
                _currentFilterWidth = 0
                inlineTags = true
            }
        }
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
            if (_originalInlineTagSetting && inlineTags)
            {
                // are the filters taking up more than half the space?
                if (_currentFilterWidth > (textFrame.width / 2))
                {
                    _filterTags = 0 // all filter tags are rebuilt when inlineTags changes, must reset value
                    _currentFilterWidth = 0
                    inlineTags = false
                }
            }
        }
        else // active filter being removed
        {
            _currentFilterWidth -= filterWidth
            _filterTags -= 1
            if (_originalInlineTagSetting && !inlineTags)
            {
                if (_currentFilterWidth > (textFrame.width / 2))
                {
                    inlineTags = false
                }
                else
                {
                    _filterTags = 0
                    _currentFilterWidth = 0
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

        onRowsInserted: {
            updateStringValue();
        }

        onRowsRemoved: {
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

        //------------------------------------------
        // Top Row - Text Area and Buttons
        //------------------------------------------
        WGExpandingRowLayout {
            id: inputRow
            Layout.fillWidth: true
            Layout.preferredHeight: childrenRect.height
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop

            WGPushButton {
                //Save filters and load previous filters
                id: btnListviewFilters
                iconSource: "icons/search_folder_16x16.png"

                tooltip: "Filter Options"


                menu: WGMenu {
                    title: "Filters"
                    MenuItem {
                        text: "MOCKUP ONLY"
                    }

                    MenuSeparator{}

                    MenuItem {
                        text: "Save Filter..."
                    }

                    MenuItem {
                        text: "Clear Filters"
                    }

                    MenuSeparator { }

                        WGMenu {
                            title: "Saved Filters:"

                        MenuItem {
                            text: "Saved Filter 1"
                        }
                        MenuItem {
                            text: "Saved Filter 2"
                        }
                        MenuItem {
                            text: "Saved Filter 3"
                        }
                        MenuItem {
                            text: "Saved Filter 4"
                        }
                    }
                }
            }

            WGTextBoxFrame {
                id: textFrame
                color: palette.TextBoxColor
                Layout.fillWidth: true
                Layout.preferredHeight: childrenRect.height + defaultSpacing.standardBorderSize
                Layout.maximumHeight: childrenRect.height + defaultSpacing.standardBorderSize
                Layout.alignment: Qt.AlignLeft | Qt.AlignTop

                onWidthChanged: {
                    checkActiveFilterSize()
                }

                // can only be a single row

                WGExpandingRowLayout {
                    id: inputLine
                    anchors {left: parent.left; top: parent.top; right: parent.right}
                    height: defaultSpacing.minimumRowHeight

                    Loader {
                        id: activeFiltersInlineRect
                        visible: _filterTags > 0 && inlineTags
                        Layout.preferredWidth: _currentFilterWidth + (defaultSpacing.rowSpacing * _filterTags) + defaultSpacing.rowSpacing
                        Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                        sourceComponent: inlineTags ? filterTagList : null
                    } // activeFiltersLayoutRect

                    WGTextBox {
                        id: filterText
                        Layout.fillWidth: true
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight

                        style: WGInvisTextBoxStyle{}

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
                        iconSource: "icons/close_sml_16x16.png"

                        tooltip: "Clear Filters"
                        Layout.alignment: Qt.AlignLeft | Qt.AlignTop

                        onClicked: {
                            rootFrame.dataModel.clearFilters();
                            rootFrame.internalStringValue = "";
                            _currentFilterWidth = 0
                            _filterTags = 0
                        }
                    }

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
                                iconSource: "icons/close_sml_16x16.png"

                                onClicked: {
                                    rootFrame.dataModel.removeFilter(index);
                                }
                            }
                        ]

                    }
                }
            } // activeFiltersBelowRect
        } //filterTags
    } // mainRowLayout
} // rootFrame
