import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import WGControls 1.0

//TODO This needs to be tested with a tree with multiple root nodes

/*!
    \brief Items that make the rows in a WGTreeView.
    Should only be used within a WGTreeView. Not intended to be used on its own.
    The WGTreeItem is a ListView that loads further treeItems via a delegate loader.

Example:
\code{.js}
    WGTreeItem {
        id: rootItem
        model: treeView.model
        width: treeView.width
        height: treeView.height
    }
\endcode
*/
WGListView {
    id: treeItem
    model: ChildModel
    height: visible ? contentHeight + topMargin + bottomMargin : 0
    leftMargin: 0
    rightMargin: 0
    topMargin: treeView.childListMargin
    bottomMargin: treeView.childListMargin
    columnSpacing: treeView.columnSpacing
    selectionMargin: treeView.selectionMargin
    minimumRowHeight: treeView.minimumRowHeight
    selectionExtension: treeView.selectionExtension
    treeExtension: treeView.treeExtension
    columnDelegates: treeView.columnDelegates
    defaultColumnDelegate: treeView.defaultColumnDelegate
    enableVerticalScrollBar: false

    property int depth: typeof(childItems) === "undefined" ? 0 : childItems.depth
    property int parentListIndex: typeof(index) === "undefined" ? 0 : index
    property real marginedWidth: width - leftMargin - rightMargin -
    (enableVerticalScrollBar ? verticalScrollBar.collapsedWidth : 0)

    // Local properties and methods for handling multiselection during keyboard navigation
    property bool modifiedSelectionExtension: false;
    property bool shiftKeyPressed: false

    function handlePreNavigation() {
        if (selectionExtension == null) {
            return
        }
        if (selectionExtension.multiSelect && !shiftKeyPressed) {
            selectionExtension.multiSelect = false;
            modifiedSelectionExtension = true;
        }
    }

    function handlePostNavigation() {
        if (selectionExtension == null) {
            return
        }
        if (modifiedSelectionExtension == true) {
            selectionExtension.multiSelect = true;
        }
    }

    function setCurrentIndex( modelIndexToSet ) {
        if (treeExtension !== null)
        {
            treeExtension.currentIndex = modelIndexToSet
        }
        // Give the parent active focus, so it can handle keyboard inputs
        if (typeof content !== "undefined")
        {
            content.forceActiveFocus()
        }
        else
        {
            forceActiveFocus()
        }
    }

    //The rectangle for the entire row
    delegate: Item {
        id: itemDelegate

        property real actualIndentation: treeView.indentation * depth
        property real verticalMargins: !HasChildren ? childRowMargin * 2 : Expanded ? 0 : headerRowMargin
        readonly property bool oddDepth: depth % 2 !== 0
        readonly property bool oddIndex: treeItem.parentListIndex % 2 !== 0
        readonly property bool switchRowColours: oddDepth !== oddIndex
		
        x: treeItem.leftMargin
        height: content.height + treeView.footerSpacing + verticalMargins
        width: treeItem.marginedWidth

        Rectangle {
            id: groupBackgroundColour

            x: actualIndentation
            width: treeItem.marginedWidth - x
            height: parent.height
            visible: treeView.backgroundColourMode === treeView.incrementalGroupBackgroundColours
            color: visible ? Qt.lighter(treeView.backgroundColour, 1 + depth % treeView.backgroundColourIncrements / 10) : "transparent"
        }

        Rectangle { // separator line between rows
            id: topSeparator
            width: parent.width
            anchors.top: parent.top
            anchors.horizontalCenterOffset: -(content.height + treeView.footerSpacing)
            height: 1
            color: Qt.darker(palette.MidLightColor,1.2)
            visible: treeView.lineSeparator && depth !== 0
        }

        Item { // All content
            id: content
            objectName: "content"

            property bool hasActiveFocus: false

            height: childrenRect.height
            y: HasChildren ? treeView.headerRowMargin : treeView.childRowMargin
            anchors.left: parent.left
            anchors.right: parent.right
			

            Component.onCompleted: {
                if(treeItem.depth === 0)
                {
                    if (treeView.rootExpanded && HasChildren)
                    {
                        Expanded = true;
                    }
                }
            }

            Keys.onUpPressed: {
                treeExtension.blockSelection = true;

                treeItem.handlePreNavigation();
                treeExtension.moveUp();
                treeItem.handlePostNavigation();
            }

            Keys.onDownPressed: {
                treeExtension.blockSelection = true;

                treeItem.handlePreNavigation();
                treeExtension.moveDown();
                treeItem.handlePostNavigation();
            }

            Keys.onLeftPressed: {
                treeExtension.blockSelection = true;

                treeItem.handlePreNavigation();
                treeExtension.moveLeft();
                treeItem.handlePostNavigation();
            }

            Keys.onRightPressed: {
                treeExtension.blockSelection = true;

                treeItem.handlePreNavigation();
                treeExtension.moveRight();
                treeItem.handlePostNavigation();
            }

            Keys.onReturnPressed: {
                if (treeExtension.blockSelection) {
                    return;
                }

                // Select the current item in tree
                treeExtension.blockSelection = false;
                treeExtension.selectItem();
            }

            Keys.onSpacePressed: {
                if (treeExtension.blockSelection) {
                    return;
                }

                // Select the current item in tree
                treeExtension.blockSelection = false;
                treeExtension.selectItem();
            }

            Keys.onPressed: {
                // Flag the shift key being pressed to allow multiselection via tree navigation
                if (event.key == Qt.Key_Shift) {
                    shiftKeyPressed = true;
                }
                event.accepted = false;
            }

            Keys.onReleased: {
                // Flag the shift key being released to disallow multiselection via tree navigation
                if (event.key == Qt.Key_Shift) {
                    shiftKeyPressed = false;
                }
                event.accepted = false;
            }

            onActiveFocusChanged: {
                if (content.activeFocus)
                {
                    hasActiveFocus = true
                }
                else
                {
                    hasActiveFocus = false
                }
            }

            WGListViewRowDelegate { // The row
                id: rowDelegate

                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right

                handlePosition: treeView.handlePosition
                defaultColumnDelegate: headerColumnDelegate
                hasActiveFocusDelegate: content.hasActiveFocus
                indentation: treeView.indentation * depth
                showBackgroundColour:
                    treeView.backgroundColourMode === treeView.uniformRowBackgroundColours ||
                    treeView.backgroundColourMode === treeView.alternatingRowBackgroundColours
                backgroundColour: itemDelegate.switchRowColours ? treeView.alternateBackgroundColour : treeView.backgroundColour
                alternateBackgroundColour: itemDelegate.switchRowColours ? treeView.backgroundColour : treeView.alternateBackgroundColour

                columnDelegates: []
                selectionExtension: treeView.selectionExtension
                modelIndex: treeView.model.index(rowIndex, 0, ParentIndex)

                onClicked: {
                    if (treeExtension && treeExtension.blockSelection) {
                        return;
                    }

                    var modelIndex = treeView.model.index(rowIndex, 0, ParentIndex);
                    treeView.rowClicked(mouse, modelIndex);
                    currentIndex = rowIndex;

                    // Update the treeExtension's currentIndex
                    if (treeExtension !== null)
                    {
                        treeExtension.currentIndex = modelIndex;
                    }

                    // Give the parent active focus, so it can handle keyboard inputs
                    content.forceActiveFocus()
                }

                onDoubleClicked: {
                    if (treeExtension && treeExtension.blockSelection) {
                        return;
                    }

                    var modelIndex = treeView.model.index(rowIndex, 0, ParentIndex);
                    treeView.rowDoubleClicked(mouse, modelIndex);
                    toggleExpandRow();
                    currentIndex = rowIndex;

                    // Update the treeExtension's currentIndex
                    setCurrentIndex( modelIndex )
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

                Component { // Row contents, icons and columns
                    id: headerColumnDelegate

                    Item {
                        id: header
                        height: headerContent.status === Loader.Ready ? headerContent.height : expandIconArea.height
                        property var parentItemData: itemData
                        property bool showExpandIcon: columnIndex === 0 && HasChildren

                        Rectangle {
                            id: expandIconArea
                            color: "transparent"
                            width: columnIndex > 0 ? 0 : expandButton.x + expandButton.width + expandIconMargin
                            height: Math.max(minimumRowHeight, treeView.expandIconSize)

                            Text {
                                id: expandButton

                                color:
                                    !showExpandIcon ? "transparent" :
                                    expandMouseArea.containsMouse ? palette.HighlightColor :
                                    Expanded ? palette.TextColor :
                                    palette.NeutralTextColor

                                width: columnIndex === 0 ? paintedWidth : 0
                                font.family : "Marlett"
                                font.pixelSize: treeView.expandIconSize
                                renderType: Text.NativeRendering
                                text : Expanded ? "\uF036" : "\uF034"
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
                                width: showExpandIcon ? expandButton.x + expandButton.width + expandIconMargin : 0
                                hoverEnabled: true
                                enabled: showExpandIcon

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

                            sourceComponent: // if a column delegate is defined use it, otherwise use default
                                columnIndex < treeView.columnDelegates.length ? treeView.columnDelegates[columnIndex]
                                : treeView.defaultColumnDelegate

                            onLoaded: {
                                height = Math.max(expandIconArea.height, item.height);
                                rowDelegate.height = height;
                            }
                        }
                    }
                }

                property var selected: typeof Selected != 'undefined' ? Selected : false
                onSelectedChanged: {
                    if (!selected) {
                        return;
                    }
					
                    var listView = treeItem;
                    while (listView != null && 
                    (typeof listView.enableVerticalScrollBar == 'undefined' || listView.enableVerticalScrollBar == false)) {
                        listView = listView.parent;
                    }
                    if (listView == null) {
                        return;
                    }
				
                    var scrollBar = listView.verticalScrollBar.scrollFlickable;
                    var scrollHeight = Math.floor(scrollBar.contentHeight * scrollBar.visibleArea.heightRatio);
				
                    var item = rowDelegate;
                    var itemY = scrollBar.contentY;
                    var itemHeight = item.height;
                    while (item != null && item != listView) {
                        itemY += item.y;
                        item = item.parent;
                    }
                    if (item == null) {
                        return;
                    }
				
                    if (itemY < scrollBar.contentY) {
                        scrollBar.contentY = itemY;
                    }
                    else if (itemY + itemHeight > scrollBar.contentY + scrollHeight) {
                        scrollBar.contentY = itemY + itemHeight - scrollHeight;
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
                property bool ancestorCollapsed: !treeItem.visible || typeof Expanded === "undefined" || !Expanded || subTree.status !== Loader.Ready

                Loader {
                    id: subTree
                    source: "WGTreeItem.qml"
                    width: treeItem.marginedWidth
                }
            }
        }
    }
}
