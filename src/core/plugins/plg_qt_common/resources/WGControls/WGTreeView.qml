import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

//TODO: Test with various configurations of depthColourisation and flatColourisation
//TODO: Requires extensive testing with indentation and leafNodeIndentation

/*!
 \brief
    Creates a TreeView of data with branches and leaf nodes.
    The TreeView loads WGTreeItems and passes it a columnDelegates list of contents for each row.
    If a columnDelegate is not defined the defaultColumnDelegate will be used.

Example:
\code{.js}
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
        depthColourisation: 5

        property Component propertyDelegate: Loader {
            clip: true
            sourceComponent: itemData != null ? itemData.Component : null
        }
    }
\endcode
*/

Item {
    id: treeView

    /*! This property holds the dataModel information that will be displayed in the tree view
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
        When depthColourisation is used, indentation is set to \c 0 by default as the entire row is indented instead.
        The default value is \c 12
      */
    property int indentation: 12

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
        When false, items will alternate between two colours based on their parent colour.
        The default value is \c true */
    property bool flatColourisation: true

    /*! This property causes items of the tree to be coloured based on their depth.
        Items will get progressively lighter for a depth based on this value, then the colouring will loop.
        It is ignored when flatColourisation: is true, and considered false when \c 0
        The default value is \c 0 */
    property int depthColourisation: 0

    property bool _columnHandle: columnDelegates.length > 1 ? true : false

    /*! This signal is emitted when the row is clicked.
      */
    signal rowClicked(var mouse, var modelIndex)

    /*! This signal is emitted when the row is double clicked.
      */
    signal rowDoubleClicked(var mouse, var modelIndex)

    /*! This Component is used by the property columnDelegate if no other column delegate is defined
      */
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
        indentation: treeView.indentation

        //TODO need to know which handle being dragged.
        //will need more data
    }

    Repeater {
        model: columnDelegates.length > 0 ? columnDelegates.length - 1 : 0
        Component {
            id: handle

            Rectangle {
                id: columnHandleFrame
                visible: columnHandle
                color: palette.DarkColor
                width: defaultSpacing.separatorWidth //standardMargin
                x: 100 // TODO make this smarter, look at column 1 text width
                height: treeView.height

                MouseArea{
                    id: columnHandleMouseArea
                    width: parent.width
                    height: parent.height
                    cursorShape: Qt.SplitHCursor

                    drag.target: columnHandleFrame
                    drag.axis: Drag.XAxis
                    drag.minimumX: 0
                    drag.maximumX: treeView.width

                    //needed for initial column spacing
                    Component.onCompleted: {
                        rootItem.handlePosition = columnHandleFrame.x
                    }

                    onPositionChanged: {
                        rootItem.handlePosition = columnHandleFrame.x
                    }

                    onDoubleClicked: {
                        //double click to revert to auto sizing? based on max size of first column?
                        console.log("dclick todo autosizing")
                    }
                }

                Rectangle {
                    id: innerShade
                    width: 1
                    color: palette.MidLightColor
                    height: parent.height
                    anchors.right: parent.right
                }

                Rectangle {
                    id: snapContainer
                    anchors.centerIn: parent
                    width: arrowRight.width / 2
                    color: "transparent"
                    height: 24

                    Text {
                        id: arrowRight
                        anchors.centerIn: parent
                        anchors.verticalCenterOffset: - arrowRight.paintedHeight / 6
                        color: palette.LightPanelColor
                        font.family : "Marlett"
                        font.pixelSize: parent.height

                        renderType: Text.QtRendering
                        text : "\uF038"
                    }

                    Text {
                        id: arrowLeft
                        anchors.centerIn: parent
                        anchors.verticalCenterOffset: arrowLeft.paintedHeight / 6
                        color: palette.LightPanelColor
                        font.family : "Marlett"
                        font.pixelSize: parent.height

                        renderType: Text.QtRendering
                        text : "\uF077"
                    }
                }
            }
        }
    }
}
