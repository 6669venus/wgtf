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

    spacing: 2
    height: 10
    width: slotIcon.width + slotText.width
    layoutDirection: (isInput) ? Qt.LeftToRight : Qt.RightToLeft

    function getSlotAnchor()
    {
        return mapToItem(graphView, slotIcon.x + slotIcon.width / 2, slotIcon.y + slotIcon.height / 2);
    }

    Image
    {
        id: slotIcon
        height: parent.height
        anchors.verticalCenter: parent.verticalCenter
        fillMode: Image.PreserveAspectFit
        source: (isInput) ? "images/greenSlot.png" : "images/blueSlot.png"

        MouseArea
        {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton

            onPressed:
            {
                mouse.accepted = true;
                startCreatingNewConnection(slotObj);
            }

            onPositionChanged:
            {
                mouse.accepted = true;
                graphView.currentConnection.endPos = mapToItem(graphView, mouse.x, mouse.y);
            }

            onReleased:
            {
                finishCreatingNewConnection(mapToItem(graphView, mouse.x, mouse.y));
            }
        }
    }

    Text
    {
        id: slotText
        text: (isInput) ? "input Slot" : "output Slot"
        color: "white"

        anchors.verticalCenter: parent.verticalCenter
    }
}
