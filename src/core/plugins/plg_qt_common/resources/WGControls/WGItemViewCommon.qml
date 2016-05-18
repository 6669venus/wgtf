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
    property Component columnHeaderDelegate: defaultColumnHeaderDelegate
    property Component columnFooterDelegate: defaultColumnFooterDelegate
    property Component header: !showColumnHeaders ? null : headerComponent
    property Component footer: !showColumnFooters ? null : footerComponent

    property Component headerComponent: WGHeaderRow {
        z:2
        topMargin: root.view.topMargin
        columnCount: root.columnCount()
        backgroundColour: palette.midDarkColor
        columnDelegate: columnHeaderDelegate
        model: root.model
        minimumRowHeight: defaultSpacing.minimumRowHeight
        spacing: root.columnSpacing
        visible: showColumnHeaders
        columnWidths: columnsFrame.columnWidths
        width: root.view.width - root.view.rightMargin - root.view.leftMargin
    }

    property Component footerComponent: WGHeaderRow {
        z:2
        bottomMargin: root.view.bottomMargin
        columnCount: root.columnCount()
        backgroundColour: palette.midDarkColor
        columnDelegate: columnFooterDelegate
        model: root.model
        minimumRowHeight: defaultSpacing.minimumRowHeight
        spacing: root.columnSpacing
        visible: showColumnFooters
        columnWidths: columnsFrame.columnWidths
        width: root.view.width - root.view.rightMargin - root.view.leftMargin
    }

    property Component defaultColumnHeaderDelegate: Item {
        signal dataChanged;

        property var headerTextVariant: getData("headerText");
        property string headerText:
            headerTextVariant !== null && typeof(headerTextVariant) === "string" ? headerTextVariant : ""

        onDataChanged:headerTextVariant = getData("headerText");

        Text {
            id: textBox
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.margins: 4
            verticalAlignment: Text.AlignVCenter
            color: palette.textColor
            text: headerText
        }
    }

    property Component defaultColumnFooterDelegate: Item {
        signal dataChanged;

        property var footerTextVariant: getData("footerText");
        property string footerText:
            footerTextVariant !== null && typeof(footerTextVariant) === "string" ? footerTextVariant : ""

        onDataChanged:footerTextVariant = getData("footerText");

        Text {
            id: textBoxFooter
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.margins: 4
            verticalAlignment: Text.AlignVCenter
            color: palette.textColor
            text: footerText
        }
    }


	property var commonExtensions: [columnExtension, imageExtension]
	extensions: commonExtensions

	function rowCount() {
		return extendedModel.rowCount()
	}

	function columnCount() {
		var count = columnSequence.length
		if (count == 0) {
			count = extendedModel.columnCount(null)
		}
		return count
	}

	ColumnExtension {
		id: columnExtension
	}

	ImageExtension {
		id: imageExtension
	}

	Component.onCompleted: {
		var tmp = columnDelegates
		while (tmp.length < columnCount()) {
			tmp.push(columnDelegate)
		}
		columnDelegates = tmp

		var tmp = columnWidths
		while (tmp.length < columnCount()) {
			tmp.push(Math.max(columnWidth, 1))
		}
		columnWidths = tmp

		root.view.contentWidth = Qt.binding( function() { return columnsFrame.width } )
	}
    

	WGColumnsFrame {
		id: columnsFrame
		x: root.view.contentItem.x + root.view.originX
		y: root.view.contentItem.y + root.view.originY
		height: root.view.contentItem.height
		columnWidths: root.view.columnWidths
		columnSpacing: root.view.columnSpacing
		availableWidth: root.view.width - Math.max(contentItem.x, 0)

		Component.onCompleted: {
			root.view.columnWidths = Qt.binding( function() { return columnsFrame.columnWidths } )
		}
	}

	ItemSelectionModel {
		id: itemSelectionModel
		model: extendedModel
	}
}
