import QtQuick 2.3
import QtQuick.Controls 1.2
import WGControls 1.0


WGContextArea
{
    id: nodeContextArea

    anchors.fill: parent
    propagateComposedEvents: false

    contextMenu: WGMenu
    {
        MenuItem
        {
            text: qsTr("Delete Node")
            onTriggered: {
                deleteNode(nodeID);
            }
        }
    }
}
