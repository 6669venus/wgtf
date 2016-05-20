import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQml.Models 2.2
import WGControls 2.0

WGItemView {
	id: root

	property var view: parent
	property var selectionModel: itemSelectionModel

	property Component columnDelegate: Text {
		text: itemData.display
		color: palette.textColor
	}
	property var columnDelegates: []
	property var columnSequence: []
	property real columnWidth: 0
	property var columnWidths: []
	property real columnSpacing: 0

    property bool showColumnHeaders: false
    property bool showColumnFooters: false
    property var columnHeaderDelegates: []
    property var columnFooterDelegates: []
    property Component columnHeaderDelegate: defaultColumnHeaderDelegate
    property Component columnFooterDelegate: defaultColumnFooterDelegate
    property Component header: !showColumnHeaders ? null : headerComponent
    property Component footer: !showColumnFooters ? null : footerComponent

    property Component headerComponent: WGHeaderRow {
        z:2
        columnDelegates: root.columnHeaderDelegates
		columnSequence: root.columnSequence
		columnWidths: root.columnWidths
		columnSpacing: root.columnSpacing
        headerData: root.headerData
    }

    property Component footerComponent: WGHeaderRow {
        z:2
        columnDelegates: root.columnFooterDelegates
		columnSequence: root.columnSequence
		columnWidths: root.columnWidths
		columnSpacing: root.columnSpacing
        headerData: root.headerData
    }

    property Component defaultColumnHeaderDelegate: Text {
        id: textBoxHeader
        color: palette.textColor
        text: itemData.headerText
        height: 24
    }

    property Component defaultColumnFooterDelegate: Text {
        id: textBoxFooter
        color: palette.textColor
        text: itemData.footerText
        height: 24
    }


	property var commonExtensions: [columnExtension, imageExtension]
	extensions: commonExtensions

	function rowCount() {
        return extendedModel.rowCount();
	}

	function columnCount() {
        var count = columnSequence.length;
		if (count == 0) {
            count = extendedModel.columnCount(null);
		}
        return count;
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

        var tmpHeader = columnHeaderDelegates
		while (tmpHeader.length < columnCount()) {
			tmpHeader.push(columnHeaderDelegate);
		}
		columnHeaderDelegates = tmpHeader;

        var tmpFooter = columnFooterDelegates
		while (tmpFooter.length < columnCount()) {
			tmpFooter.push(columnFooterDelegate);
		}
		columnFooterDelegates = tmpFooter;

		var tmp = columnWidths;
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

		Component.onCompleted: {
            root.view.columnWidths = Qt.binding( function() { return columnsFrame.columnWidths } );
		}
	}

	ItemSelectionModel {
		id: itemSelectionModel
		model: extendedModel
	}
}
