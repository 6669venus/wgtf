import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls 1.0

Rectangle
{
    id: graphView
    anchors.fill: parent

    property var graph
	property var nodesModel
    property var connectionsModel

    property var connectionComponent: Qt.createComponent("ConnectionNodes.qml")
    property var currentConnection: null

    color: palette.lightPanelColor


    function startCreatingNewConnection(fromSlotObj)
    {
        currentConnection = connectionComponent.createObject(nodeEditorView, {"inputSlot": (fromSlotObj.isInput) ? fromSlotObj : null,
                                                                              "outputSlot": (fromSlotObj.isInput) ? null : fromSlotObj});
    }

    function finishCreatingNewConnection(endPos)
    {
        var node = childAt(endPos.x, endPos.y);
        if (node && node.objectName == "Node")
        {
            var endSlot = node.getSlotByPos(mapToItem(node, endPos.x, endPos.y))
            if (endSlot && endSlot.objectName == "Slot")
            {
                currentConnection.tryConnectToSlot(endSlot);
            }
        }

        currentConnection.destroy();
        currentConnection = null;
    }

    Image
    {
        anchors.fill: parent
        source: "images/grid_line.png"
        fillMode: Image.Tile
    }

	WGListModel
	{
		id: nodesListModel
		source: nodesModel

		ValueExtension {}
	}

    WGListModel
    {
        id: connectionsListModel
        source: connectionsModel

        ValueExtension {}
    }

    ContextMenu
    {
    }

    Repeater
    {
        id: connectionRepeater
        model: connectionsListModel
        delegate: ConnectionNodes
        {
            inputSlot: Value.input
            outputSlot: Value.output
        }
    }

	Repeater
	{
		id: nodeRepeater
		model: nodesListModel
		delegate: Node
		{
            nodeObj: Value
            nodeID: Value.nodeID
            nodeTitle: Value.nodeTitle
            inputSlotsModel: Value.inputSlotsModel
            outputSlotsModel: Value.outputSlotsModel
            x: Value.nodeCoordX
            y: Value.nodeCoordY            
		}
	}
}
