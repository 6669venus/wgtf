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
    height: defaultSpacing.minimumRowHeight
    width: slotIcon.width + slotText.width
    layoutDirection: (isInput) ? Qt.LeftToRight : Qt.RightToLeft

    function getSlotAnchor()
    {
        return mapToItem(graphView, slotIcon.x + slotIcon.width / 2, slotIcon.y + slotIcon.height / 2);
    }

    Rectangle
    {
        id: slotIcon
        height: parent.height - defaultSpacing.standardMargin
        width: parent.height - defaultSpacing.standardMargin
        anchors.verticalCenter: parent.verticalCenter

        radius: width / 2

        color: isInput ? "#7AC943" : "#3FA9F5"

        Rectangle {
            anchors.fill: parent
            anchors.margins: 2
            radius: width / 2

            //TODO: Turn this off (or maybe very transparent white) if slot is connected to remove 'hole'
            color: "#66000000"
        }

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

    WGLabel
    {
        id: slotText
        text: (isInput) ? "input Slot" : "output Slot"

        anchors.verticalCenter: parent.verticalCenter
    }
}
