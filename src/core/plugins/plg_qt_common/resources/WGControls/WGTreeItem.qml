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
		height: content.height
		color: HasChildren ? (depth % 2 === 0 ? palette.MidLightColor : palette.MidDarkColor) : "transparent"

		Item {
			id: content
			height: childrenRect.height
			anchors.top: parent.top
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
				
				Component {
					id: headerColumnDelegate
					
					Item {
						id: header
						height: headerContent.status === Loader.Ready ? headerContent.height : expandIconArea.height
					
						Rectangle {
							id: expandIconArea
							color: "transparent"
							width:
								expandButton.visible ? expandButton.x + expandButton.width + expandIconMargin
								: expandButton.x + expandButton.width + expandIconMargin - indentation
							height: Math.max(minimumRowHeight, expandIconSize)
							
							Image {
								id: expandButton
								visible: columnIndex === 0 && HasChildren
								source: Expanded ? "qrc:///icons/arrow_down_16x16" : "qrc:///icons/arrow_right_16x16"
								x: expandIconMargin
								width: expandIconSize
								height: expandIconSize
								anchors.verticalCenter: parent.verticalCenter
							}
							
							MouseArea {
								anchors.left: parent.left
								anchors.top: parent.top
								anchors.bottom: parent.bottom
								width: expandButton.visible ? expandButton.x + expandButton.width + expandIconMargin: 0
								hoverEnabled: true

								onPressed: {
									if (HasChildren && typeof Expanded !== "undefined")
									{
										Expanded = !Expanded;
									}
								}
							}
						}
						
						Loader {
							id: headerContent
							anchors.top: parent.top
							x: expandIconArea.x + expandIconArea.width
							width: header.width
							
							sourceComponent:
								columnIndex < treeItem.columnDelegates.length ? treeItem.columnDelegates[columnIndex]
								: treeItem.defaultColumnDelegate

							onLoaded: {
								height = Math.max(expandIconArea.height, item.height);
								rowDelegate.height = height;

								if (typeof item.itemData !== "undefined")
								{
									item.itemData = itemData;
								}
							}
						}
					}
				}
			}
			
			Item {
				id: childItems
				anchors.left: parent.left
				anchors.right: parent.right
				y: rowDelegate.y + rowDelegate.height + childListMargin
				height: Expanded && subTree.status === Loader.Ready ? subTree.height : 0

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
