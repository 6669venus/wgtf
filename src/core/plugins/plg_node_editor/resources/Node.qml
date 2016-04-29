import QtQuick 2.1
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import WGControls 1.0

Item
{
    id: nodeContainer
    objectName: "Node"
    property var nodeObj
    property var nodeID
    property var nodeTitle
    property var inputSlotsModel
    property var outputSlotsModel
    property var position: Qt.point(x, y)

    property int margin: 10

    width: nodeSlotsBackground.width
    height: nodeSlotsBackground.height + nodeTitleBackground.height

    onPositionChanged: { nodeObj.setPos(x, y); }


    function getSlotViewBySlotObj(slotObj)
    {
        var isInput = slotObj.isInput;
        var listModel = (isInput) ? inputSlotRepeater : outputSlotRepeater;
        var slotView = null;

        var size = listModel.count;
        for (var i = 0; i < size; ++i)
        {
            slotView = listModel.itemAt(i);
            if (slotView.slotObj == slotObj)
            {
                return slotView;
            }
        }

        return null;
    }

    function getSlotByPos(Pos)
    {
        var slotPos = mapToItem(inputSlotsLayout, Pos.x, Pos.y);
        var slot = inputSlotsLayout.childAt(slotPos.x, slotPos.y);

        if (slot && slot.objectName == "Slot")
        {
            return slot;
        }

        return null;
    }

    Rectangle
    {
        id: nodeSlotsBackground

        width: Math.max(slotsLayout.width + margin, nodeTextTitle.contentWidth + margin)
        height: slotsLayout.height + margin

        anchors.top: nodeTitleBackground.bottom

        color: "black"
        opacity: 0.35
        radius: 3
        border.color: "black"
        border.width: 4
    }

    MouseArea
    {
        anchors.fill: parent
        drag.target: parent
        drag.axis: Drag.XAndYAxis
    }

    Row
    {
        id: slotsLayout

        anchors.top: nodeTitleBackground.bottom
        anchors.horizontalCenter: nodeTitleBackground.horizontalCenter
        anchors.topMargin: margin / 2
        spacing: 50

        Column
        {
            id: inputSlotsLayout
            spacing: 10

            WGListModel
            {
                id: inputSlotsListModel
                source: inputSlotsModel

                ValueExtension {}
            }

            Repeater
            {
                id: inputSlotRepeater
                model: inputSlotsListModel
                delegate: Slot
                {
                    slotObj: Value
                    isInput: Value.isInput
                }
            }
        }

        Column
        {
            id: outputSlotsLayout
            spacing: 10

            WGListModel
            {
                id: outputSlotsListModel
                source: outputSlotsModel

                ValueExtension {}
            }

            Repeater
            {
                id: outputSlotRepeater
                model: outputSlotsListModel
                delegate: Slot
                {
                    slotObj: Value
                    isInput: Value.isInput
                }
            }
        }
    }

    Rectangle
    {
        id: nodeTitleBackground

        height: 20
        width: parent.width
        anchors.top: parent.top
        anchors.leftMargin: margin / 2
        anchors.rightMargin: margin / 2

        radius: 5

        border.color: "black"
        border.width: 0.5


        LinearGradient {
            anchors.fill: parent
            start: Qt.point(0, 0)
            end: Qt.point(70, 0)
            opacity: 0.8
            source: parent
            gradient: Gradient {
                GradientStop { position: 0.0; color: "blue" }
                GradientStop { position: 1.0; color: "black" }
            }
        }

        Text
        {
            id: nodeTextTitle
            text: nodeTitle
            color: "white"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
        }

    }

    NodeContextMenu
    {
    }
}
