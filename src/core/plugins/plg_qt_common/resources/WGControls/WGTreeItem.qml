import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import WGControls 1.0

/*!
    \brief Items that make the rows in a WGTreeView.
    For exmample: the folders in a tree view are made up of treeItems.

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
    flatColourisation: false
    leafNodeColourGrouping: true

    property Component propertyDelegate: Loader {
        clip: true
        sourceComponent: itemData != null ? itemData.Component : null
    }
}
\endcode
*/

WGListView {
    id: treeItem
    model: ChildModel
    height: visible ? contentHeight + topMargin + bottomMargin : 0
    spacing: treeView.spacing
    leftMargin: 0
    rightMargin: 0
    topMargin: childListMargin
    bottomMargin: childListMargin
    columnSpacing: treeView.columnSpacing
    selectionMargin: treeView.selectionMargin
    minimumRowHeight: treeView.minimumRowHeight
    selectionExtension: treeView.selectionExtension
    treeExtension: treeView.treeExtension
    columnDelegates: treeView.columnDelegates
    defaultColumnDelegate: treeView.defaultColumnDelegate
    enableVerticalScrollBar: false


    //TODO: These colourisation states should probably should be done another way
    /*! This property causes all items of the tree to be coloured the same.
        It ignores the depthColourisation property.
        It can work in conjuction with leafNodeColourGrouping.
        The default value is \c true */
    property bool flatColourisation: true

    /*! This property causes items of the tree to be coloured based on their depth.
        Items will get progressively lighter for a depth based on this value, then the colouring will loop.
        It is ignored when flatColourisation: is true, and considered false when set to \c 0
        It can work in conjuction with leafNodeColourGrouping.
        The default value is \c 0 */
    property int depthColourisation: 0

    /*! This property determines the indentation offset of leaf nodes.
        The default value is 0. */
    property int leafNodeIndentation: 0

    /*! This property toggles the visilbilty of a single line separator at the bottom of each listItem
        The default value is \c false */
    property bool lineSeparator: false

    /*! This property determines the font height of the expand icon (triangle).
        The default value is \c 12 */
    property int expandIconSize: 16

    //TODO
    property int indentation: 0

    property int depth: typeof childItems !== "undefined" ? childItems.depth : 0
    // This appears to give all leaf nodes a depth of 0
    // property int depth: childItems.depth
    property real childListMargin: typeof childItems !== "undefined" ? childItems.childListMargin : 1

    //The rectangle for the entire row
    delegate: Rectangle {
        id: itemDelegate

        property int colorIndex: typeof parentColorIndex !== "undefined" ? parentColorIndex + index + 1 : index

        x: {
            if (depthColourisation !==0) // offset entire row //(flatColourisation == false && depthColourisation !==0)
            {
                if (depth == 0) // first item doesn't need offset
                {
                    treeItem.x
                }
                else
                {
                    treeItem.x + treeView.indentation // add indentation to the next row
                }
            }
            else
            {
                treeItem.x // flat and alternating coloured rows do not offset row position, only columns
            }
        }
        width: {
            if (depthColourisation !==0)
            {
                treeItem.width - treeItem.leftMargin - treeItem.rightMargin - 1 - (treeView.indentation*(depth))
            }
            else
            {
                treeItem.width - treeItem.leftMargin - treeItem.rightMargin - 1
            }
        }
        height: content.height + treeView.footerSpacing + (!HasChildren ? childRowMargin * 2 : Expanded ? 0 : headerRowMargin)

        color: {
            if (flatColourisation == true)
            {
                palette.MidDarkColor//palette.MidLightColor
            }
            else // not a flat colour
            {
                if (depthColourisation !== 0) // Colourise by depth
                {
                    //HasChildren ? (depth % 2 === 0 ? palette.MidLightColor : palette.MidDarkColor) : "transparent"
                    if (depth % 5 == 0)
                    {
                        palette.MidDarkColor
                    }
                    else
                    {
                        Qt.lighter(palette.MidDarkColor, (1 + (depth % depthColourisation/10)))
                    }
                }
                else // not flat, not by depth simply alternate the colour of each WGTreeItem
                {
                    if (colorIndex % 2 == 0)
                    {
                        Qt.darker(palette.MidLightColor,1.2)
                    }
                    else
                    {
                        palette.MidDarkColor
                    }
                }
            }
        }

        Rectangle { // separator line between rows
            id: topSeparator
            width: treeItem.width - treeItem.leftMargin - treeItem.rightMargin - 1
            anchors.top: parent.top
            anchors.horizontalCenterOffset : -(content.height + treeView.footerSpacing)
            height: 1
            color: Qt.darker(palette.MidLightColor,1.2)
            visible: lineSeparator ? (depth == 0 ? false : true) : false //only between rows
        }

        Item { // All content
            id: content
            height: childrenRect.height
            y: HasChildren ? headerRowMargin : childRowMargin
            anchors.left: parent.left
            anchors.right: parent.right

            Keys.onUpPressed: {
                treeExtension.blockSelection = true;
                treeExtension.moveUp();
            }

            Keys.onDownPressed: {
                treeExtension.blockSelection = true;
                treeExtension.moveDown();
            }

            Keys.onLeftPressed: {
                treeExtension.blockSelection = true;
                treeExtension.collapse();
            }

            Keys.onRightPressed: {
                treeExtension.blockSelection = true;
                treeExtension.expand();
            }

            Keys.onReturnPressed: {
                treeExtension.blockSelection = false;
                treeExtension.selectItem();
            }

            WGListViewRowDelegate {
                id: rowDelegate

                Rectangle {
                    anchors.fill:parent
                    color:"purple"
                }

                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right

                defaultColumnDelegate: headerColumnDelegate

                /* This property passes the WGTreeView colourisation style information to the columnDelegates  */
                depthColourisation: treeItem.depthColourisation

                //indentation: treeItem.indentation // DOES THIS NEED x DEPTH?
                indentation: {
                    if (depthColourisation !==0)
                    {
                        treeItem.indentation
                    }
                    else
                    { //TODO NEED TO CHECK IF ROOT AND USE
                        if (depth == 0)
                        {
                            0
                        }
                        else
                        {
                            treeItem.indentation * depth
                        }
                    }
                }

                columnDelegates: []
                selectionExtension: treeItem.selectionExtension

                onClicked: {
                    var modelIndex = treeView.model.index(rowIndex, 0, ParentIndex);
                    treeView.rowClicked(mouse, modelIndex);
                    currentIndex = rowIndex;

                    console.log ("leafNodeIndentation = " + leafNodeIndentation)
                    console.log ("flatColourisation = " + flatColourisation)
                    console.log ("depthColourisation = " + depthColourisation)
                    console.log ("indentation = " + indentation)
                    console.log ("lineSeparator = " + lineSeparator)
                    console.log ("itemDelegate.width is " + itemDelegate.width)
                    console.log ("------------------")
                    console.log ("rowIndex is " + rowIndex)
                    console.log ("depth is " + depth)
                    console.log ("------------------")
                }

                onDoubleClicked: {
                    var modelIndex = treeView.model.index(rowIndex, 0, ParentIndex);
                    treeView.rowDoubleClicked(mouse, modelIndex);
                    toggleExpandRow();
                    currentIndex = rowIndex;
                }

                function isExpandable()
                {
                    return (HasChildren && typeof Expanded !== "undefined");
                }

                function toggleExpandRow()
                {
                    if (isExpandable())
                    {
                        Expanded = !Expanded;
                    }
                }

                // return - true - if child tree is expanded
                function expandRow()
                {
                    if (isExpandable() && !Expanded)
                    {
                        Expanded = true;

                        // handled
                        return true;
                    }

                    // No children, non expandable, or already expanded
                    return false;
                }

                // return - true - if child tree is collapsed
                function collapseRow()
                {
                    if (isExpandable() && Expanded)
                    {
                        Expanded = false;

                        // handled
                        return true;
                    }

                    // No children, non expandable, or collapsed
                    return false;
                }

                Component {
                    id: headerColumnDelegate

                    Item {
                        id: header
                        height: headerContent.status === Loader.Ready ? headerContent.height : expandIconArea.height
                        property var parentItemData: itemData

                        //DEBUG COLUMNS
                        Rectangle {
                            anchors.fill: parent
                            color: columnIndex == 0 ? "red" : "blue"
                        }

                        Rectangle {
                            id: expandIconArea
                            color: "green"//"transparent"
                            width: {//TODO test with expandButton.visible false
                                if (columnIndex == 0)
                                {
                                    //TODO: this looks wrong
                                    expandButton.visible ? expandButton.x + expandButton.width + expandIconMargin
                                    : expandButton.x + expandButton.width + expandIconMargin + leafNodeIndentation
                                }
                                else // second column, controls
                                {
                                    0
                                }
                            }
                            height: Math.max(minimumRowHeight, expandIconSize)

                            Text {
                                id: expandButton

                                color : {
                                    if (expandMouseArea.containsMouse)
                                    {
                                        return palette.HighlightColor
                                    }
                                    else if (Expanded)
                                    {
                                        return palette.TextColor
                                    }
                                    else
                                    {
                                        return palette.NeutralTextColor
                                    }
                                }

                                width: columnIndex === 0 ? paintedWidth : 0
                                font.family : "Marlett"
                                font.pixelSize: expandIconSize
                                renderType: Text.NativeRendering
                                text : Expanded ? "6" : "4"
                                visible: columnIndex === 0 && HasChildren
                                x: expandIconMargin
                                anchors.verticalCenter: parent.verticalCenter
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignHCenter
                            }

                            MouseArea {
                                id: expandMouseArea
                                anchors.left: parent.left
                                anchors.top: parent.top
                                anchors.bottom: parent.bottom
                                width: expandButton.visible ? expandButton.x + expandButton.width + expandIconMargin : 0
                                hoverEnabled: true



                                onPressed: {
                                    rowDelegate.toggleExpandRow()
                                }
                            }
                        }

                        Loader {
                            id: headerContent
                            anchors.top: parent.top
                            anchors.left: expandIconArea.right
                            anchors.right: header.right
                            anchors.leftMargin: expandIconMargin
                            property var itemData: parentItemData

                            sourceComponent:
                                columnIndex < treeItem.columnDelegates.length ? treeItem.columnDelegates[columnIndex]
                                : treeItem.defaultColumnDelegate

                            onLoaded: {
                                height = Math.max(expandIconArea.height, item.height);
                                rowDelegate.height = height;
                            }
                        }
                    }
                }
            }

            Item {
                id: childItems
                anchors.right: parent.right
                anchors.left: parent.left
                y: rowDelegate.y + rowDelegate.height + (HasChildren ? treeView.headerRowMargin : 0) + (Expanded ? childListMargin : 0)
                height: visible ? subTree.height : 0
                visible: !ancestorCollapsed

                property int depth: treeItem.depth + 1
                property real childListMargin: treeItem.childListMargin
                property bool ancestorCollapsed: !treeItem.visible || typeof Expanded === "undefined" || !Expanded || subTree.status !== Loader.Ready


                Loader {
                    id: subTree
                    source: "WGTreeItem.qml"
                    width: treeView.width - treeView.leftMargin - treeView.rightMargin
                    // Uses delegate itemDelegate rectangle as context. Cannot inherit from treeItem
                    // All properties have to be passsed here
                    property int parentColorIndex: colorIndex

                    /*property int parentLeafNodeIndentation: itemDelegate.leafNodeIndentation
                    property bool parentFlatColourisation: itemDelegate.flatColourisation
                    property int parentDepthColourisation: itemDelegate.depthColourisation
                    property int parentIndentation: itemDelegate.indentation
                    property int parentLineSeparator: itemDelegate.lineSeparator*/

                    onLoaded :{
                        //TODO This needs to be tested with a tree with multiple root nodes
                        item.leafNodeIndentation = treeItem.leafNodeIndentation
                        item.flatColourisation = treeItem.flatColourisation
                        item.depthColourisation = treeItem.depthColourisation
                        item.indentation = treeItem.indentation
                        item.lineSeparator = treeItem.lineSeparator
                    }
                }
            }
        }
    }
}
