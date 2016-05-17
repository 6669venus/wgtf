import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls 1.0
import WGControls 2.0


Item
{
    id: graphView
    anchors.fill: parent

	property var nodesModel
    property var connectionsModel

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

    WGGridCanvas
    {
        id: canvasContainer
        anchors.fill: parent
        showMouseLine: false
        clip: false

        viewTransform: WGViewTransform{
            container: canvasContainer
        }

        property var connectionComponent: Qt.createComponent("ConnectionNodes.qml")
        property var currentConnection: null

        function startCreatingNewConnection(fromSlotObj)
        {
            currentConnection = connectionComponent.createObject(nodeEditorView, {"firstSlot": fromSlotObj,
                                                                                  "secondSlot": null,
                                                                                  "viewTransform": viewTransform});
        }

        function finishCreatingNewConnection(endPos)
        {
            var nodePos = graphView.mapToItem(canvasItem, endPos.x, endPos.y);
            var node = canvasItem.childAt(nodePos.x, nodePos.y);
            if (node)
            {
                var endSlot = node.getSlotByPos(canvasItem.mapToItem(node, nodePos.x, nodePos.y))
                if (endSlot && endSlot.objectName == "Slot")
                {
                    currentConnection.tryConnectToSlot(endSlot);
                }
            }

            currentConnection.destroy();
            currentConnection = null;
        }

        WGGridCanvasItem
        {
            id: canvasItem
            anchors.fill: parent
            clip: false
            viewTransform: canvasContainer.viewTransform

            Repeater
            {
                id: nodeRepeater
                model: nodesListModel
                delegate: Node
                {
                    nodeObj: Value
                    nodeID: nodeObj.id
                    nodeTitle: nodeObj.nodeTitle
                    inputSlotsModel: nodeObj.inputSlotsModel
                    outputSlotsModel: nodeObj.outputSlotsModel
                    localPosition: mapFromItem(graphView, nodeObj.nodeCoordX, nodeObj.nodeCoordY)
                }
            }
        }

        Repeater
        {
            id: connectionRepeater
            model: connectionsListModel
            delegate: ConnectionNodes
            {
                connectionObj: Value
                connectionID: connectionObj.id
                firstSlot: connectionObj.output
                secondSlot: connectionObj.input
                viewTransform: canvasContainer.viewTransform
            }
        }
    }
}
