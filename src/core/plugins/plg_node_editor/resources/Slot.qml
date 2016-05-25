import QtQuick 2.1
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import WGControls 1.0

Row
{
    id: slot
    objectName: "Slot"
    property bool isInput
    property var slotObj

    // TODO: Make this to reflect C++ state?
    property bool connected: false

    // TODO: Make this part of the slot model
    property color slotColor: isInput ? "#7AC943" : "#3FA9F5"

    // TODO: Make this part of the slot model
    property string slotLabel: isInput ? "input Slot" : "output Slot"

    property var style: SlotStyle{
        parentSlot: slot
    }

    spacing: 2
    height: defaultSpacing.minimumRowHeight
    width: slotIcon.width + slotText.width
    layoutDirection: (isInput) ? Qt.LeftToRight : Qt.RightToLeft

    function getSlotAnchor()
    {
        if (nodeContainer.nodeIsExpanded)
        {
            return mapToItem(graphView, slotIcon.x + slotIcon.width / 2, slotIcon.y + slotIcon.height / 2);
        }
        else
        {
            if (isInput)
                return nodeHeader.mapToItem(graphView, nodeHeader.x - defaultSpacing.standardBorderSize, nodeHeader.y + nodeHeader.height / 2);
            else
                return nodeHeader.mapToItem(graphView, nodeHeader.x + nodeHeader.width, nodeHeader.y + nodeHeader.height / 2)
        }
    }

    Loader
    {
        id: slotIcon
        height: parent.height - defaultSpacing.standardMargin
        width: parent.height - defaultSpacing.standardMargin
        anchors.verticalCenter: parent.verticalCenter

        sourceComponent: style.connector
        asynchronous: true

        MouseArea
        {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton

            onPressed:
            {
                mouse.accepted = true;
                canvasContainer.startCreatingNewConnection(slotObj);
            }

            onPositionChanged:
            {
                mouse.accepted = true;
                canvasContainer.currentConnection.endPos = mapToItem(graphView, mouse.x, mouse.y);
            }

            onReleased:
            {
                canvasContainer.finishCreatingNewConnection(mapToItem(graphView, mouse.x, mouse.y));
            }
        }
    }

    Loader
    {
        id: slotText

        sourceComponent: style.label
        asynchronous: true

        anchors.verticalCenter: parent.verticalCenter
    }
}
