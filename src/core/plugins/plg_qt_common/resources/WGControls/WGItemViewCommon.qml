import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQml.Models 2.2
import WGControls 2.0

/*!
 /brief Adds common C++ functionality to both List and Tree views.
 WGItemView is a C++ class.
 ColumnExtension, ImageExtension and ItemSelectionModel provide extras to data models.
 WGColumnsFrame adds resizable column support.
 */
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

    property var commonExtensions: [columnExtension, imageExtension]
    extensions: commonExtensions

    function rowCount() {
        return extendedModel.rowCount()
    }

    function columnCount() {
        var count = columnSequence.length
        if (count == 0) {
            count = extendedModel.columnCount( null /* parent */ )
        }
        return count
    }

    ColumnExtension {
        id: columnExtension
    }

    ImageExtension {
        id: imageExtension
    }

    // Once component has finished loading, make the column widths match the
    // column count
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

    // Add resizable column handles
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
