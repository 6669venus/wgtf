import QtQuick 2.1
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import QtQuick.Controls 1.2
import WGControls 1.0

Item
{
    id: groupItem
    objectName: "groupItem"


    property var globalMinPosition: mapToItem(graphView, x, y)
    property var globalMaxPosition: mapToItem(graphView, x + width, y + height)

    property bool isDragActive: nodeGroupMouseArea.drag.active

    property string groupTitle

    property color groupColor

    property bool selected: false

    property bool locked: false

    property int initialX: 0
    property int initialY: 0
    property int initialWidth: 0
    property int initialHeight: 0

    signal setPosition(var xPos, var yPos)

    signal deleteNode()

    signal changeColor()

    signal changeTitle(var title)

    onIsDragActiveChanged:
    {
        dragStateChanged(this)
    }

    onGlobalMinPositionChanged: {
        if (!isDragActive)
        {
            setPosition(globalMinPosition.x,globalMinPosition.y)
        }
    }

    signal dragStateChanged(var group);

    // editable header at the top of the group box
    WGTextBox {
        id: titleBox
        anchors.top: parent.top
        anchors.left: leftDrag.left
        anchors.right: parent.right
        anchors.margins: defaultSpacing.standardMargin
        height: defaultSpacing.minimumRowHeight + defaultSpacing.doubleMargin

        enabled: !locked

        text: groupTitle

        font.bold: true
        font.pointSize: 12

        onEditingFinished: {
            changeTitle(text)
        }

        style: WGTextBoxStyle {
            background: Item {}
            textColor: groupColor
        }

        // focus is not dropping because nothing else in node editor has focus items. Bit of a hack to stop editing state.
        Connections {
            target: canvasContainer
            onCanvasPressed: {
                titleBox.focus = false
            }
        }
        Connections {
            target: groupItem
            onIsDragActiveChanged: {
                titleBox.focus = false
            }
            onSelectedChanged: {
                titleBox.focus = false
            }
        }
    }

    MouseArea
    {
        id: nodeGroupMouseArea
        anchors.fill: parent
        anchors.margins: defaultSpacing.doubleMargin
        anchors.topMargin: defaultSpacing.minimumRowHeight
        drag.target: groupItem
        drag.axis: Drag.XAndYAxis
        acceptedButtons: Qt.LeftButton
        preventStealing: true

        enabled: !locked

        onPositionChanged:
        {
            if (drag.active && !groupItem.selected)
            {
                graphView.resetNodesSelection();
                graphView.selectNode(groupItem);
            }
        }

        onPressed: {
            if (!(mouse.modifiers & Qt.ControlModifier))
            {
                graphView.resetNodesSelection();
                graphView.selectNode(groupItem);

                // select all nodes inside the group box. Currently not working properly
                canvasContainer.selectArea(globalMinPosition, globalMaxPosition, mouse)
                return;
            }

            if (selected)
            {
                graphView.unselectNode(groupItem);
            }
            else
            {
                graphView.selectNode(groupItem);
                // select all nodes inside the group box. Currently not working properly
                canvasContainer.selectArea(globalMinPosition, globalMaxPosition, mouse)
            }

        }
        onReleased: {
            graphView.resetNodesSelection();
            graphView.unselectNode(groupItem);
        }
    }

    WGContextArea
    {
        id: contextArea
        contextMenu: WGMenu
        {
            //would be better to have this in an actions.xml file?
            MenuItem
            {
                text: qsTr("Create Node")
                onTriggered: {
                    createNode(mapToItem(graphView,contextArea.popupPoint.x, contextArea.popupPoint.y).x,
                               mapToItem(graphView,contextArea.popupPoint.x, contextArea.popupPoint.y).y,
                               "Test Node From QML");
                }
            }
            MenuItem
            {
                text: qsTr("Delete Group Box")
                onTriggered: {
                    groupItem.deleteNode();
                }
            }
            // bug with WGContextArea means the checked state of the menu won't update
            MenuItem
            {
                text: qsTr("Lock Group Box")
                checkable: true
                checked: locked
                onTriggered: {
                    locked = !locked
                }
            }
            MenuItem
            {
                text: qsTr("Change Color")
                onTriggered: {
                    groupItem.changeColor()
                }
            }
        }
    }

    //visual appearance of the actual group box
    Rectangle {
        id: groupBox
        anchors.left: leftDrag.left
        anchors.right: rightDrag.right
        anchors.bottom: bottomDrag.bottom
        anchors.top: titleBox.top
        color: selected ? Qt.rgba(groupColor.r, groupColor.g, groupColor.b, 0.4) : Qt.rgba(groupColor.r, groupColor.g, groupColor.b, 0.2)
        border.width: defaultSpacing.doubleBorderSize
        border.color: selected ? groupColor : Qt.rgba(groupColor.r, groupColor.g, groupColor.b, 0.6)

        z: -1
    }

    MouseArea {
        id: leftDrag
        anchors.top: titleBox.bottom
        anchors.left: parent.left
        anchors.bottom: leftCornerDrag.top

        enabled: !locked

        drag.target: leftDrag
        drag.axis: Drag.XAxis
        drag.maximumX: rightDrag.x - 50

        cursorShape: Qt.SizeHorCursor

        onPressed: {
            initialX = x
            initialWidth = groupItem.width
            anchors.left = undefined
        }

        onReleased: {
            groupItem.width = initialWidth - (leftDrag.x - initialX)
            groupItem.x = groupItem.x + (leftDrag.x - initialX)
            anchors.left = parent.left
        }

        width: defaultSpacing.doubleMargin
    }

    MouseArea {
        id: rightDrag
        anchors.top: titleBox.bottom
        anchors.right:  parent.right
        anchors.bottom: rightCornerDrag.top

        drag.target: rightDrag
        drag.axis: Drag.XAxis
        drag.minimumX: leftDrag.x + 50

        enabled: !locked

        cursorShape: Qt.SizeHorCursor

        onPressed: {
            initialX = x
            initialWidth = groupItem.width
            anchors.right = undefined
        }

        onReleased: {
            groupItem.width = initialWidth + (rightDrag.x - initialX)
            anchors.right = parent.right
        }

        width: defaultSpacing.doubleMargin
    }

    MouseArea {
        id: leftCornerDrag
        anchors.left: parent.left
        anchors.bottom: parent.bottom

        drag.target: leftCornerDrag
        drag.axis: Drag.XAndYAxis
        drag.maximumX: rightDrag.x - 50
        drag.minimumY: titleBox.y + titleBox.height + 50

        enabled: !locked

        cursorShape: Qt.SizeBDiagCursor

        onPressed: {
            initialX = x
            initialY = y
            initialWidth = groupItem.width
            initialHeight = groupItem.height
            anchors.left = undefined
            anchors.bottom = undefined
            titleBox.anchors.left = leftCornerDrag.left
            groupBox.anchors.left = leftCornerDrag.left
            groupBox.anchors.bottom = leftCornerDrag.bottom
        }

        onReleased: {
            groupItem.width = initialWidth - (leftCornerDrag.x - initialX)
            groupItem.height = initialHeight + (leftCornerDrag.y - initialY)
            groupItem.x = groupItem.x + (leftCornerDrag.x - initialX)
            anchors.left = parent.left
            anchors.bottom = parent.bottom
            titleBox.anchors.left = leftDrag.left
            groupBox.anchors.left = leftDrag.left
            groupBox.anchors.bottom = bottomDrag.bottom
        }

        width: defaultSpacing.doubleMargin
        height: defaultSpacing.doubleMargin
    }

    MouseArea {
        id: rightCornerDrag
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        drag.target: rightCornerDrag
        drag.axis: Drag.XAndYAxis
        drag.minimumX: leftDrag.x + 50
        drag.minimumY: titleBox.y + titleBox.height + 50

        enabled: !locked

        cursorShape: Qt.SizeFDiagCursor

        onPressed: {
            initialX = x
            initialY = y
            initialWidth = groupItem.width
            initialHeight = groupItem.height
            anchors.right = undefined
            anchors.bottom = undefined
            groupBox.anchors.right = rightCornerDrag.right
            groupBox.anchors.bottom = rightCornerDrag.bottom
        }

        onReleased: {
            groupItem.width = initialWidth + (rightCornerDrag.x - initialX)
            groupItem.height = initialHeight + (rightCornerDrag.y - initialY)
            anchors.right = parent.right
            anchors.bottom = parent.bottom
            groupBox.anchors.right = rightDrag.right
            groupBox.anchors.bottom = bottomDrag.bottom
        }

        width: defaultSpacing.doubleMargin
        height: defaultSpacing.doubleMargin
    }

    MouseArea {
        id: bottomDrag
        anchors.left: leftCornerDrag.right
        anchors.right: rightCornerDrag.left
        anchors.bottom: parent.bottom

        drag.target: bottomDrag
        drag.axis: Drag.YAxis
        drag.minimumY: titleBox.y + titleBox.height + 50

        enabled: !locked

        cursorShape: Qt.SizeVerCursor

        onPressed: {
            initialY = y
            initialHeight = groupItem.height
            anchors.bottom = undefined
        }

        onReleased: {
            groupItem.height = initialHeight + (bottomDrag.y - initialY)
            anchors.bottom = parent.bottom
        }

        height: defaultSpacing.doubleMargin
    }
}
