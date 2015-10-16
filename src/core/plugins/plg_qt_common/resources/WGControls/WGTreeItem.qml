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

    /*! This property causes all items of the tree to be coloured the same.
        When false, items will alternate between two colours based on their parent colour.
        The default value is \c true */
    property bool flatColourisation: true

    /*! This property causes items of the tree to be coloured based on their depth.
        Items will get progressively lighter for a depth based on this value, then the colouring will loop.
        It is ignored when flatColourisation: is true, and considered false when \c 0
        The default value is \c 0 */
    property int depthColourisation: 0

    /*! This property determines the indentation offset of leaf nodes.
        The default value is 0. */
    property int leafNodeIndentation: 0

    /*! This property toggles the visilbilty of a single line separator at the bottom of each listItem
        The default value is \c false */
    property bool lineSeparator: false

    /*! This property determines the font height of the expand icon (triangle).
        The default value is \c 16 */
    property int expandIconSize: 16

    /*! This property determines the indentation of child objects from their parent.
        The default value is \c 0 */
    property int indentation: 0

    /*! This property holds the depth of an item.
        The root node object will have a depth of 0.
        The default value is \c 0 */
    property int depth: typeof childItems !== "undefined" ? childItems.depth : 0

    property real childListMargin: typeof childItems !== "undefined" ? childItems.childListMargin : 1
	
	// Local properties and methods for handling multiselection during keyboard navigation
	property bool modifiedSelection: false;
	property bool shiftKeyPressed: false

	function handlePreNavigation() {
		if (selectionExtension.multiSelect == true && shiftKeyPressed == false) {
			selectionExtension.multiSelect = false;
			modifiedSelection = true;
		}
	}

	function handlePostNavigation() {
		if (modifiedSelection == true) {
			selectionExtension.multiSelect = true;
		}
	}

	function postSelectionCheck() {
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
    delegate: Rectangle {
        id: itemDelegate

        // required to pass value to child
        property int colorIndex: typeof parentColorIndex !== "undefined" ? parentColorIndex + index + 1 : index

        x: {
            if (depthColourisation !== 0) // offset entire row //(flatColourisation == false && depthColourisation !==0)
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

        //TODO investigate magic number 1's
        width: {
            if (depthColourisation !== 0)
            {
                treeItem.width - treeItem.leftMargin - treeItem.rightMargin - 1 - (treeView.indentation * (depth))
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
                    Qt.lighter(palette.MidDarkColor, (1 + (depth % depthColourisation / 10)))
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
            visible: lineSeparator && depth !== 0
        }

        Item { // All content
            id: content

            property bool hasActiveFocus: false

            height: childrenRect.height
            y: HasChildren ? headerRowMargin : childRowMargin
            anchors.left: parent.left
            anchors.right: parent.right

            Keys.onUpPressed: {
                treeExtension.blockSelection = true;

				handlePreNavigation();
                treeExtension.moveUp();
				handlePostNavigation();
            }

            Keys.onDownPressed: {
                treeExtension.blockSelection = true;

				handlePreNavigation();
                treeExtension.moveDown();
				handlePostNavigation();
            }

            Keys.onLeftPressed: {
                treeExtension.blockSelection = true;

				handlePreNavigation();
                treeExtension.moveLeft();
				handlePostNavigation();
            }

            Keys.onRightPressed: {
                treeExtension.blockSelection = true;

				handlePreNavigation();
                treeExtension.moveRight();
				handlePostNavigation();
            }

            Keys.onReturnPressed: {
                // Select the current item in tree
                treeExtension.blockSelection = false;
                treeExtension.selectItem();
            }

            Keys.onSpacePressed: {
                // Select the current item in tree
                treeExtension.blockSelection = false;
                treeExtension.selectItem();
            }

			Keys.onPressed: {
				// Flag the shift key being pressed to allow multiselection via tree navigation
				if (event.key == Qt.Key_Shift) {
					shiftKeyPressed = true;
				}
			}

			Keys.onReleased: {
				// Flag the shift key being released to disallow multiselection via tree navigation
				if (event.key == Qt.Key_Shift) {
					shiftKeyPressed = false;
				}
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

                defaultColumnDelegate: headerColumnDelegate

                hasActiveFocusDelegate: content.hasActiveFocus

                /* This property passes the WGTreeView colourisation style information to the columnDelegates  */
                depthColourisation: treeItem.depthColourisation

                /*  If depthColourisation is used, indentation will offset the row.
                    If depthColourisation is not used the offset is within the first column*/
                indentation: depthColourisation === 0 ? treeItem.indentation * depth : treeItem.indentation

                columnDelegates: []
                selectionExtension: treeItem.selectionExtension

                onClicked: {
                    var modelIndex = treeView.model.index(rowIndex, 0, ParentIndex);
                    treeView.rowClicked(mouse, modelIndex);
                    currentIndex = rowIndex;

                    // Update the treeExtension's currentIndex
                    //setCurrentIndex( modelIndex )
                    if (treeExtension !== null)
                    {
                        treeExtension.currentIndex = modelIndex;
                    }
                    // Give the parent active focus, so it can handle keyboard inputs
                    content.forceActiveFocus()
                }



                onDoubleClicked: {
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

                        Rectangle {
                            id: expandIconArea
                            color: "transparent"
                            width: {//TODO test with expandButton.visible false
                                if (columnIndex == 0)
                                {
                                    //TODO: leafNodeIndentation needs testing
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
                                text : Expanded ? "\uF036" : "\uF034"
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

                            sourceComponent: // if a column delegate is defined use it, otherwise use default
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
                    property int parentColorIndex: colorIndex // Alternating coloured treeItems need to know the colour of their parent.

                    onLoaded :{
                        item.leafNodeIndentation = Qt.binding( function() { return treeItem.leafNodeIndentation } )
                        item.flatColourisation = Qt.binding( function() { return treeItem.flatColourisation } )
                        item.depthColourisation = Qt.binding( function() { return treeItem.depthColourisation } )
                        item.indentation = Qt.binding( function() { return treeItem.indentation } )
                        item.lineSeparator = Qt.binding( function() { return treeItem.lineSeparator } )
                    }
                }
            }
        }
    }
}
