import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

Item {
    id: treeView

    property var model
    property real leftMargin: 2
    property real rightMargin: 2 //margin to the right of the tree
    property real topMargin: 2
    property real bottomMargin: 2
    property real spacing: 0 // Spacing between columns. Will retain row height and add space between.
    property real columnSpacing: 1 //Margin to the right of the controls
    property real selectionMargin: 0 //All margins for the selection highlight
    property real expandIconMargin: 3 //Space to the left and right of the expand/contract arrow
    property real childListMargin: 0 //Margin beneath the heading of each folder
    property real footerSpacing: 0 //Spacing beneath each control. Increases row height
    property real headerRowMargin: 0 //Top and bottom margin spacing for folder nodes
    property real childRowMargin: 0 // Spacing between control and each row. Modifies row height
    property real minimumRowHeight: defaultSpacing.minimumRowHeight

    /*  This property determines the indentation of child nodes (branches and leaves).
        The default value is 12
      */
    property int indentation: 12

    /*  This property determines the indentation offset of leaf nodes.
        The default value is 0.
      */
    property int leafNodeIndentation: 0

    property var selectionExtension: null
    property var treeExtension: null
    property var columnDelegates: []
    property bool enableVerticalScrollBar: true

    property bool flatColourisation: true
    property bool depthColourisation: false
    property bool leafNodeColourGrouping: false

    signal rowClicked(var mouse, var modelIndex)
    signal rowDoubleClicked(var mouse, var modelIndex)

    property Component defaultColumnDelegate: Text {
        color: palette.TextColor
        clip: itemData != null && itemData.Component != null
        text: itemData != null ? itemData.display : ""
        font.bold: itemData != null && itemData.HasChildren
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    WGTreeItem {
        id: rootItem
        leftMargin: treeView.leftMargin
        rightMargin: treeView.rightMargin
        topMargin: treeView.topMargin
        bottomMargin: treeView.bottomMargin
        spacing: treeView.spacing
        childListMargin: treeView.childListMargin
        model: treeView.model
        enableVerticalScrollBar: treeView.enableVerticalScrollBar
        width: treeView.width
        height: treeView.height

        flatColourisation: treeView.flatColourisation
        leafNodeColourGrouping: treeView.leafNodeColourGrouping
        depthColourisation: treeView.depthColourisation
        leafNodeIndentation: treeView.leafNodeIndentation
    }
}
