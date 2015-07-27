import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import WGControls 1.0

WGListView {
	id: treeItem
	model: ChildModel
	height: expanded ? contentHeight + topMargin + bottomMargin : 0
	spacing: treeView.spacing
	leftMargin: 0
	rightMargin: 0
	topMargin: childListMargin
	bottomMargin: childListMargin
	columnSpacing: treeView.columnSpacing
	selectionMargin: treeView.selectionMargin
	minimumRowHeight: treeView.minimumRowHeight
	selectionExtension: treeView.selectionExtension
	columnDelegates: treeView.columnDelegates
	defaultColumnDelegate: treeView.defaultColumnDelegate
	verticalScrollBar: false

	property int expandIconSize: 16
	property int depth: typeof childItems !== "undefined" ? childItems.depth : 0
	property real childListMargin: typeof childItems !== "undefined" ? childItems.childListMargin : 1
	property bool expanded: typeof Expanded === "undefined" ? false : Expanded

	delegate: Rectangle {
		id: itemDelegate
		x: treeItem.x
		width: treeItem.width - treeItem.leftMargin - treeItem.rightMargin - 1
		height: content.height + treeView.footerSpacing + (!HasChildren ? childRowMargin * 2 : Expanded ? 0 : headerRowMargin)
		color: HasChildren ? (depth % 2 === 0 ? palette.MidLightColor : palette.MidDarkColor) : "transparent"

		Item {
			id: content
			height: childrenRect.height
			y: HasChildren ? headerRowMargin : childRowMargin
			anchors.left: parent.left
			anchors.right: parent.right

			WGListViewRowDelegate {
				id: rowDelegate
				anchors.top: parent.top
				anchors.left: parent.left
				anchors.right: parent.right
				indentation: treeView.indentation * depth
				defaultColumnDelegate: headerColumnDelegate
				columnDelegates: []
				selectionExtension: treeItem.selectionExtension

				onClicked: {
					var modelIndex = treeView.model.index(rowIndex, 0, treeView.model.parent);
					treeView.rowClicked(mouse, modelIndex);
				}
				
				onDoubleClicked: {
					var modelIndex = treeView.model.index(rowIndex, 0, treeView.model.parent);
					treeView.rowDoubleClicked(mouse, modelIndex);
					expandRow();
				}
				
				function expandRow()
				{
					if (HasChildren && typeof Expanded !== "undefined")
					{
						Expanded = !Expanded;
					}
				}
				
				Component {
					id: headerColumnDelegate
					
					Item {
						id: header
						height: headerContent.status === Loader.Ready ? headerContent.height : expandIconArea.height
						property var parentItemData: itemData

						Rectangle {
							id: expandIconArea
							color: "transparent"
							width:
								expandButton.visible ? expandButton.x + expandButton.width + expandIconMargin
								: expandButton.x + expandButton.width + expandIconMargin - indentation
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
								width: expandButton.visible ? expandButton.x + expandButton.width + expandIconMargin: 0
								hoverEnabled: true

								onPressed: {
									rowDelegate.expandRow()
								}
							}
						}
						
						Loader {
							id: headerContent
							anchors.top: parent.top
							anchors.left: expandIconArea.right
							anchors.right: header.right
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
				anchors.left: parent.left
				anchors.right: parent.right
				y: rowDelegate.y + rowDelegate.height + (HasChildren ? treeView.headerRowMargin : 0) + (Expanded ? childListMargin : 0)
				height: (Expanded && subTree.status === Loader.Ready) ? subTree.height : 0

				property int depth: treeItem.depth + 1
				property real childListMargin: treeItem.childListMargin

				Loader {
					id: subTree
					source: "WGTreeItem.qml"
					width: treeView.width - treeView.leftMargin - treeView.rightMargin
				}
			}
		}
	}
}
