import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import BWControls 1.0
import WGControls 1.0

Rectangle {
    color: palette.MainWindowColor
    property var title: "Tree Test"
    property var layoutHints: { 'test': 0.1 }
    property var sourceModel: source


    Label {
        id: searchBoxLabel
        x: testTreeView.leftMargin
        y: 2
        text: "Search:"
    }

    BWTextField {
        id: searchBox
        y: 2
        anchors.left: searchBoxLabel.right
        anchors.right: parent.right
    }

    WGTreeFilter {
        id: filter
        source: sourceModel
        filter: searchBox.text
    }

    WGTreeModel {
        id: testModel
        source: filter.filteredSource

        ValueExtension {}
        ColumnExtension {}
        ComponentExtension {}
        TreeExtension {}
        ThumbnailExtension {}
        SelectionExtension {
            id: treeModelSelection
            multiSelect: true
        }
    }

    WGTreeView {
        id: testTreeView
        anchors.top: searchBox.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        model: testModel
        rightMargin: 8 // leaves just enought space for conventional slider
        columnDelegates: [defaultColumnDelegate, propertyDelegate]
        selectionExtension: treeModelSelection
        childRowMargin: 2
        columnSpacing: 4
        lineSeparator: false

        flatColourisation: false
        depthColourisation: 0

        property Component propertyDelegate: Loader {
            clip: true
            sourceComponent: itemData != null ? itemData.Component : null
        }
    }
}
