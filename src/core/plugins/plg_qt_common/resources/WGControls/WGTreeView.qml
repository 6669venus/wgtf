import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

Item {
    id: treeView

    /*! This property determines
        The default value is
    */
    property var model

    /*! This property determines the gap between the left edge of the entire control and the list contents.
        The default value is \c 2
    */
    property real leftMargin: 2

    /*! This property determines the gap between the right edge of the entire control and the list contents.
        The default value of \c 8 is given to allow for enough room for the vertical scrollbar
        The default value is \c 8
    */
    property real rightMargin: 2

    /*! This property determines the gap between the top edge of the entire control and the list contents.
        The default value is \c 2
    */
    property real topMargin: 2

    /*! This property determines the gap between the bottom edge of the entire control and the list contents.
        The default value is \c 2
    */
    property real bottomMargin: 2

    /*! This property adds vertical spacing under each sibling object in the list.
        This will not add spacing between a child and its parent.
        See the childListMargin: for spacing between parents and children.
        The default value is \c 0
    */
    property real spacing: 0

    /*! This property will add space to the right of each column element.
        The default value is \c 1
      */
    property real columnSpacing: 1

    /*! This property determines the margin around the selection highlight.
        A value of \c 0 will cause the highlights to fill the same space as the frame that takes up the entire row.
        The default value is \c 0
    */
    property real selectionMargin: 0

    /*! This property determines the space to the left and right of the expand/contract icon.
        The default value is \c 3
    */
    property real expandIconMargin: 3

    /*! This property adds vertical spacing under each parent object.
        The default value is \c 0
    */
    property real childListMargin: 0

    /*! This property adds spacing beneath each control.
        Increases row height.
        The default value is \c 0
    */
    property real footerSpacing: 0

    /*! This property adds top and bottom margin spacing for branch nodes
        Modifies row height
        The default value is \c 0
    */
    property real headerRowMargin: 0

    /*! This property adds top and bottom margin spacing for leaf nodes
        Modifies row height
        The default value is \c 0
    */
    property real childRowMargin: 0

    /*! This property sets the minimum row height.
        Unlike Row Margin changes this change will alter the space available for controls within each row.
        If controls automatically fill the row height their size will change.
        The default value is \c defaultSpacing.minimumRowHeight
    */
    property real minimumRowHeight: defaultSpacing.minimumRowHeight

    /*! This property determines the indentation of all nodes (child and branch), excluding the root node

        The default value is \c 12
      */
    property int indentation: flatColourisation ? 12 : (depthColourisation !==0 ? 0 : 12)

    /*! This property determines the indentation offset of leaf nodes.
        The default value is \c 0.
      */
    property int leafNodeIndentation: 0

    property var selectionExtension: null
    property var treeExtension: null

    /*! This property holds the list of columns that are displayed within each row
      */
    property var columnDelegates: []

    /*!  This property enables the vertical scrollbar (both flickable and conventional).
        Mouse wheel scrolling is unaffected by this setting.
        The default value is \c true.
      */
    property bool enableVerticalScrollBar: true


    /*! This property adds a horizontal line separator between rows.
        The default value is \c true.
      */
    property bool lineSeparator: true

    /*! This property causes all items of the tree to be coloured the same.
        It ignores the depthColourisation property.
        The default value is \c true */
    property bool flatColourisation: true

    /*! This property causes items of the tree to be coloured based on their depth.
        Items will get progressively lighter for a depth based on this value, then the colouring will loop.
        It is ignored when flatColourisation: is true, and considered false when \c 0
        The default value is \c 0 */
    property int depthColourisation: 0

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

        lineSeparator: treeView.lineSeparator
        flatColourisation: treeView.flatColourisation
        depthColourisation: treeView.depthColourisation
        leafNodeIndentation: treeView.leafNodeIndentation
    }
}
