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
        currentConnection = connectionComponent.createObject(nodeEditorView, {"firstSlot": fromSlotObj,
                                                                              "secondSlot": null });
    }

    function finishCreatingNewConnection(endPos)
    {

        var nodePos = mapToItem(nodeLayout, endPos.x, endPos.y);
        var node = nodeLayout.childAt(nodePos.x, nodePos.y);

        if (node)
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

    function getNodeViewById(nodeId)
    {
        var nodeView = null;
        var size = nodeRepeater.count;
        for (var i = 0; i < size; ++i)
        {
            nodeView = nodeRepeater.itemAt(i);
            if (nodeView.nodeObj.id == nodeId)
            {
                return nodeView;
            }
        }

        return null;
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

    Item
    {
        id: nodeLayout
        anchors.fill: parent

        Repeater
        {
            id: nodeRepeater
            model: nodesListModel
            delegate: Node
            {
                nodeObj: Value
                nodeID: Value.id
                nodeTitle: Value.nodeTitle
                inputSlotsModel: Value.inputSlotsModel
                outputSlotsModel: Value.outputSlotsModel
                x: Value.nodeCoordX
                y: Value.nodeCoordY
            }
        }
    }

    Repeater
    {
        id: connectionRepeater
        model: connectionsListModel
        delegate: ConnectionNodes
        {
            connectionID: Value.id
            firstSlot: Value.input
            secondSlot: Value.output
        }
    }
}
