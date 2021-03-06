import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQml.Models 2.2
import WGControls 2.0
import "wg_view_selection.js" as WGViewSelection

WGItemView {
    id: root

    property var view: parent
    property var selectionModel: itemSelectionModel

    property Component columnDelegate: Text {
        id: defaultColumnDelegate
        objectName: "defaultColumnDelegate_" + text
        text: itemData.display
        color: palette.textColor
    }
    property var columnDelegates: []
    property var columnSequence: []
    property real columnWidth: 100
    property var columnWidths: []
    property var implicitColumnWidths: []
    property real columnSpacing: 0
    property bool clamp: false

    property var headerDelegates: []
    property var footerDelegates: []
    property Component headerDelegate: null
    property Component footerDelegate: null
    property Component header: null
    property Component footer: null

    property var __sortIndicators: []
    property Component sortIndicator: Item {
        id: indicator
        height: sortArrowImage.height
        Image {
            id: sortArrowImage
            anchors.centerIn: parent
            source: "icons/sort_blank_10x10.png"
        }
    }

    property Component headerComponent: WGHeaderRow {
        z:2
        columnDelegates: root.headerDelegates
        columnSequence: root.columnSequence
        columnWidths: root.columnWidths
        columnSpacing: root.columnSpacing
        headerData: root.headerData
        sortIndicators: root.__sortIndicators
    }

    property Component footerComponent: WGHeaderRow {
        z:2
        columnDelegates: root.footerDelegates
        columnSequence: root.columnSequence
        columnWidths: root.columnWidths
        columnSpacing: root.columnSpacing
        headerData: root.headerData
    }

	property var viewExtension: null
    property var commonExtensions: [viewExtension, columnExtension, imageExtension]

    // Pass signals up to parent
    signal currentChanged(var current, var previous)
    signal selectionChanged(var selected, var deselected)

    extensions: view.extensions.concat(commonExtensions)

    function rowCount() {
        return extendedModel.rowCount();
    }

    function columnCount() {
        var count = columnSequence.length;
        if (count == 0) {
            count = extendedModel.columnCount(null /* parent */);
        }
        return count;
    }

	function movePrevious(event) {
		var newIndex = viewExtension.getPreviousIndex(itemSelectionModel.currentIndex);
        WGViewSelection.updateKeyboardSelection(event, newIndex, itemSelectionModel, viewExtension);
	}

	function moveNext(event) {
		var newIndex = viewExtension.getNextIndex(itemSelectionModel.currentIndex);
        WGViewSelection.updateKeyboardSelection(event, newIndex, itemSelectionModel, viewExtension);
	}

	function moveBackwards(event) {
		var newIndex = viewExtension.getBackwardIndex(itemSelectionModel.currentIndex);
        WGViewSelection.updateKeyboardSelection(event, newIndex, itemSelectionModel, viewExtension);
	}

	function moveForwards(event) {
		var newIndex = viewExtension.getForwardIndex(itemSelectionModel.currentIndex);
        WGViewSelection.updateKeyboardSelection(event, newIndex, itemSelectionModel, viewExtension);
	}

    ColumnExtension {
        id: columnExtension
    }

    ImageExtension {
        id: imageExtension
    }

    /*! Ensure the columnDelegates and columnWidths lists are the same length
        as the number of columns that actually loaded into the list.
        \see WGItemRow
     */
    Component.onCompleted: {
        var tmp = columnDelegates;
        while (tmp.length < columnCount()) {
            tmp.push(columnDelegate);
        }
        columnDelegates = tmp;

        tmp = headerDelegates
        if(tmp.length > 0)
        {
            while (tmp.length < columnCount()) {
                tmp.push(headerDelegate);
                __sortIndicators.push(sortIndicator);
            }
        }
        if((tmp.length == 0) && (headerDelegate != null))
        {
            while (tmp.length < columnCount()) {
                tmp.push(headerDelegate);
                __sortIndicators.push(sortIndicator);
            }
        }
        headerDelegates = tmp;
        if(headerDelegates.length > 0)
        {
             header = headerComponent;
        }

        tmp = footerDelegates;
        if(tmp.length > 0)
        {
            while (tmp.length < columnCount()) {
                tmp.push(footerDelegate);
            }
        }
        if((tmp.length == 0) && (footerDelegate != null))
        {
            while (tmp.length < columnCount()) {
                tmp.push(footerDelegate);
            }
        }
        footerDelegates = tmp;
        if(footerDelegates.length > 0)
        {
             footer = footerComponent;
        }

        tmp = columnWidths;
        while (tmp.length < columnCount()) {
            tmp.push(Math.max(columnWidth, 1));
        }

        columnWidths = tmp;
    }

    WGColumnsFrame {
        id: columnsFrame
        x: root.view.contentItem.x + root.view.originX
        y: 0
        height: root.view.height
        columnWidths: root.view.columnWidths
        columnSpacing: root.view.columnSpacing
        availableWidth: root.view.width - Math.max(contentItem.x, 0)
        implicitColumnWidths: root.implicitColumnWidths
        clamp: root.clamp

        Component.onCompleted: {
            root.view.columnWidths = Qt.binding( function() { return columnsFrame.columnWidths } );
        }
    }

    ItemSelectionModel {
        id: itemSelectionModel
        model: extendedModel

        // Pass signals up to parent
        onCurrentChanged: root.currentChanged(current, previous);
        onSelectionChanged: root.selectionChanged(selected, deselected)

		property var connections: null

		Component.onCompleted: {
			connections = Qt.binding( function() { setCurrentIndex( root.currentIndex, ItemSelectionModel.NoUpdate ); } );
			root.currentIndex = Qt.binding( function() { return currentIndex; } );
		}
    }

	Connections {
        target: view
        onItemPressed: WGViewSelection.itemPressed(mouse, itemSelectionModel, viewExtension, rowIndex)
    }
}
