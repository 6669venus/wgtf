import QtQuick 2.3
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



    ContextMenu
    {
        id: contextArea
        menuPath: "NodeEditor.group"
    }

    // editable header at the top of the group box
    WGTextBox {
        id: titleBox
        anchors.top: tLeftCornerDrag.top
        anchors.left: tLeftCornerDrag.left
        anchors.right: locked ? lockedText.left : tRightCornerDrag.right
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
            textColor: "#99FFFFFF"
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

    Text {
        id: lockedText
        anchors.top: tRightCornerDrag.top
        anchors.right: tRightCornerDrag.right
        anchors.margins: defaultSpacing.doubleMargin
        height: defaultSpacing.minimumRowHeight + defaultSpacing.doubleMargin
        opacity: 0.5
        horizontalAlignment: Text.AlignRight
        text: "(locked)"
        visible: locked
        color: groupColor
    }

    MouseArea
    {
        id: nodeGroupMouseArea
        anchors.fill: parent
        anchors.margins: defaultSpacing.doubleMargin
        anchors.topMargin: defaultSpacing.minimumRowHeight + defaultSpacing.doubleMargin
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

                canvasContainer.selectArea(canvasContainer.mapFromItem(canvasItem, groupItem.x, groupItem.y),
                                           canvasContainer.mapFromItem(canvasItem, groupItem.x + groupItem.width, groupItem.y + groupItem.height), mouse.modifiers & Qt.ShiftModifier)
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
                canvasContainer.selectArea(canvasContainer.mapFromItem(canvasItem, groupItem.x, groupItem.y),
                                           canvasContainer.mapFromItem(canvasItem, groupItem.x + groupItem.width, groupItem.y + groupItem.height), mouse.modifiers & Qt.ShiftModifier)
            }

        }
        onReleased: {
            graphView.resetNodesSelection();
            graphView.unselectNode(groupItem);
        }
    }

    //visual appearance of the actual group box
    Rectangle {
        id: groupBox
        anchors.left: tLeftCornerDrag.left
        anchors.right: tRightCornerDrag.right
        anchors.bottom: bLeftCornerDrag.bottom
        anchors.top: tLeftCornerDrag.top
        color: selected ? Qt.rgba(groupColor.r, groupColor.g, groupColor.b, 0.3) : Qt.rgba(groupColor.r, groupColor.g, groupColor.b, 0.15)
        border.width: defaultSpacing.doubleBorderSize
        border.color: selected ? groupColor : Qt.rgba(groupColor.r, groupColor.g, groupColor.b, 0.5)
        radius: defaultSpacing.standardRadius
        z: -1
    }

    WGAction
    {
        active: true
        actionId: qsTr("NodeEditor.group|.Delete Group Box")
        onTriggered: {
            groupItem.deleteNode();
        }
    }
    // bug with WGContextArea means the checked state of the menu won't update
    WGAction
    {
        active: true
        actionId: qsTr("NodeEditor.group|.Lock Group Box")
        checkable: true
        checked: locked
        onTriggered: {
            locked = !locked
        }
    }
    WGAction
    {
        active: true
        actionId: qsTr("NodeEditor.group|.Change Color")
        onTriggered: {
            groupItem.changeColor()
        }
    }

    MouseArea {
        id: tLeftCornerDrag
        anchors.left: parent.left
        anchors.top: parent.top

        drag.target: tLeftCornerDrag
        drag.axis: Drag.XAndYAxis
        drag.maximumX: tRightCornerDrag.x - 50
        drag.maximumY: bLeftCornerDrag.y - 50

        enabled: !locked

        cursorShape: !locked ? Qt.SizeFDiagCursor : Qt.ArrowCursor

        onPressed: {
            initialX = x
            initialY = y
            initialWidth = groupItem.width
            initialHeight = groupItem.height
            anchors.left = undefined
            anchors.top = undefined
        }

        onReleased: {
            groupItem.width = initialWidth - (tLeftCornerDrag.x - initialX)
            groupItem.height = initialHeight - (tLeftCornerDrag.y - initialY)
            groupItem.x = groupItem.x + (tLeftCornerDrag.x - initialX)
            groupItem.y = groupItem.y + (tLeftCornerDrag.y - initialY)
            anchors.left = parent.left
            anchors.top = parent.top
        }

        width: defaultSpacing.doubleMargin
        height: defaultSpacing.doubleMargin
    }

    MouseArea {
        id: tRightCornerDrag
        anchors.right: parent.right
        anchors.top: parent.top

        drag.target: tRightCornerDrag
        drag.axis: Drag.XAndYAxis
        drag.minimumX: tLeftCornerDrag.x + tLeftCornerDrag.width + 50
        drag.maximumY: bRightCornerDrag.y - 50

        enabled: !locked

        cursorShape: !locked ? Qt.SizeBDiagCursor : Qt.ArrowCursor

        onPressed: {
            initialX = x
            initialY = y
            initialWidth = groupItem.width
            initialHeight = groupItem.height
            anchors.right = undefined
            anchors.top = undefined
            groupBox.anchors.top = tRightCornerDrag.top
            titleBox.anchors.top = tRightCornerDrag.top
        }

        onReleased: {
            groupItem.width = initialWidth + (tRightCornerDrag.x - initialX)
            groupItem.height = initialHeight - (tRightCornerDrag.y - initialY)
            groupItem.y = groupItem.y + (tRightCornerDrag.y - initialY)
            anchors.right = parent.right
            anchors.top = parent.top
            groupBox.anchors.top = tLeftCornerDrag.top
            titleBox.anchors.top = tLeftCornerDrag.top
        }

        width: defaultSpacing.doubleMargin
        height: defaultSpacing.doubleMargin
    }

    MouseArea {
        id: bLeftCornerDrag
        anchors.left: parent.left
        anchors.bottom: parent.bottom

        drag.target: bLeftCornerDrag
        drag.axis: Drag.XAndYAxis
        drag.maximumX: bRightCornerDrag.x - 50
        drag.minimumY: titleBox.y + titleBox.height + 50

        enabled: !locked

        cursorShape: !locked ? Qt.SizeBDiagCursor : Qt.ArrowCursor


        onPressed: {
            initialX = x
            initialY = y
            initialWidth = groupItem.width
            initialHeight = groupItem.height
            anchors.left = undefined
            anchors.bottom = undefined
            titleBox.anchors.left = bLeftCornerDrag.left
            groupBox.anchors.left = bLeftCornerDrag.left
        }

        onReleased: {
            groupItem.width = initialWidth - (bLeftCornerDrag.x - initialX)
            groupItem.height = initialHeight + (bLeftCornerDrag.y - initialY)
            groupItem.x = groupItem.x + (bLeftCornerDrag.x - initialX)
            anchors.left = parent.left
            anchors.bottom = parent.bottom
            titleBox.anchors.left = tLeftCornerDrag.left
            groupBox.anchors.left = tLeftCornerDrag.left
        }

        width: defaultSpacing.doubleMargin
        height: defaultSpacing.doubleMargin
    }

    MouseArea {
        id: bRightCornerDrag
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        drag.target: bRightCornerDrag
        drag.axis: Drag.XAndYAxis
        drag.minimumX: bLeftCornerDrag.x + 50
        drag.minimumY: titleBox.y + titleBox.height + 50

        enabled: !locked

        cursorShape: !locked ? Qt.SizeFDiagCursor : Qt.ArrowCursor

        onPressed: {
            initialX = x
            initialY = y
            initialWidth = groupItem.width
            initialHeight = groupItem.height
            anchors.right = undefined
            anchors.bottom = undefined
            groupBox.anchors.right = bRightCornerDrag.right
            groupBox.anchors.bottom = bRightCornerDrag.bottom
        }

        onReleased: {
            groupItem.width = initialWidth + (bRightCornerDrag.x - initialX)
            groupItem.height = initialHeight + (bRightCornerDrag.y - initialY)
            anchors.right = parent.right
            anchors.bottom = parent.bottom
            groupBox.anchors.right = tRightCornerDrag.right
            groupBox.anchors.bottom = bLeftCornerDrag.bottom
        }

        width: defaultSpacing.doubleMargin
        height: defaultSpacing.doubleMargin
    }


    MouseArea {
        id: leftDrag
        anchors.top: tLeftCornerDrag.bottom
        anchors.left: parent.left
        anchors.bottom: bLeftCornerDrag.top

        enabled: !locked

        drag.target: leftDrag
        drag.axis: Drag.XAxis
        drag.maximumX: rightDrag.x - 50

        cursorShape: !locked ? Qt.SizeHorCursor : Qt.ArrowCursor

        width: defaultSpacing.doubleMargin

        onPressed: {
            initialX = x
            initialWidth = groupItem.width
            anchors.left = undefined
            groupBox.anchors.left = leftDrag.left
            titleBox.anchors.left = leftDrag.left
        }

        onReleased: {
            groupItem.width = initialWidth - (leftDrag.x - initialX)
            groupItem.x = groupItem.x + (leftDrag.x - initialX)
            anchors.left = parent.left
            groupBox.anchors.left = tLeftCornerDrag.left
            titleBox.anchors.left = tLeftCornerDrag.left
        }
    }

    MouseArea {
        id: rightDrag
        anchors.top: tRightCornerDrag.bottom
        anchors.right:  parent.right
        anchors.bottom: bRightCornerDrag.top

        drag.target: rightDrag
        drag.axis: Drag.XAxis
        drag.minimumX: leftDrag.x + 50

        enabled: !locked

        cursorShape: !locked ? Qt.SizeHorCursor : Qt.ArrowCursor

        width: defaultSpacing.doubleMargin

        onPressed: {
            initialX = x
            initialWidth = groupItem.width
            anchors.right = undefined
            groupBox.anchors.right = rightDrag.right
            titleBox.anchors.right = rightDrag.right
        }

        onReleased: {
            groupItem.width = initialWidth + (rightDrag.x - initialX)
            anchors.right = parent.right
            groupBox.anchors.right = tRightCornerDrag.right
            titleBox.anchors.right = tRightCornerDrag.right
        }
    }

    MouseArea {
        id: topDrag
        anchors.left: tLeftCornerDrag.right
        anchors.right: tRightCornerDrag.left
        anchors.top: parent.top

        drag.target: topDrag
        drag.axis: Drag.YAxis
        drag.maximumY: bottomDrag.y -50

        enabled: !locked

        cursorShape: !locked ? Qt.SizeVerCursor : Qt.ArrowCursor

        height: defaultSpacing.doubleMargin

        onPressed: {
            initialY = y
            initialHeight = groupItem.height
            anchors.top = undefined
            groupBox.anchors.top = topDrag.top
            titleBox.anchors.top = topDrag.top
        }

        onReleased: {
            groupItem.height = initialHeight - (topDrag.y - initialY)
            groupItem.y = groupItem.y + (topDrag.y - initialY)
            anchors.top = parent.top
            groupBox.anchors.top = tLeftCornerDrag.top
            titleBox.anchors.top = tLeftCornerDrag.top
        }
    }

    MouseArea {
        id: bottomDrag
        anchors.left: bLeftCornerDrag.right
        anchors.right: bRightCornerDrag.left
        anchors.bottom: parent.bottom

        drag.target: bottomDrag
        drag.axis: Drag.YAxis
        drag.minimumY: titleBox.y + titleBox.height + 50

        enabled: !locked

        cursorShape: !locked ? Qt.SizeVerCursor : Qt.ArrowCursor

        height: defaultSpacing.doubleMargin

        onPressed: {
            initialY = y
            initialHeight = groupItem.height
            anchors.bottom = undefined
        }

        onReleased: {
            groupItem.height = initialHeight + (bottomDrag.y - initialY)
            anchors.bottom = parent.bottom
        }
    }
}
