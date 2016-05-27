import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls 1.0
import WGControls 2.0
import QtQuick.Dialogs 1.2


Item
{
    id: graphView
    anchors.fill: parent

    property var nodesModel
    property var connectionsModel
    property var nodeClassesModel

    property ListModel groupModel: ListModel{dynamicRoles: true}

    function createGroup(x, y, name, color, height, width) {
        groupModel.append({"xPos": x,
                              "yPos": y,
                              "name": name,
                              "color": color,
                              "width": width,
                              "height": height,
                              "items":[]
                          })
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

    function selectNode(node)
    {
        node.selected = true;
        multiDrag.addDragObj(node);
    }

    function unselectNode(node)
    {
        node.selected = false;
        multiDrag.removeDragObj(node);
    }

    function resetNodesSelection()
    {
        for(var i = 0; i < nodeRepeater.count; ++i)
        {
            var node = nodeRepeater.itemAt(i);
            unselectNode(node);
        }
        for(var i = 0; i < groupRepeater.count; ++i)
        {
            var group = groupRepeater.itemAt(i);
            unselectNode(group);
        }
    }

    function selectNodesInArea(areaRect, isAddMode)
    {
        var selAreaTopLeft = Qt.point(areaRect.x, areaRect.y);
        var selAreaBottomRight = Qt.point(areaRect.x + areaRect.width, areaRect.y + areaRect.height);
        for(var i = 0; i < nodeRepeater.count; ++i)
        {
            var node = nodeRepeater.itemAt(i);
            var nodeTopLeft = Qt.point(node.x, node.y);
            var nodeBottomRight = Qt.point(node.x + node.width, node.y + node.height);

            var isIntersects = Math.max(0, Math.min(nodeBottomRight.x, selAreaBottomRight.x) - Math.max(nodeTopLeft.x, selAreaTopLeft.x)) *
                    Math.max(0, Math.min(nodeBottomRight.y, selAreaBottomRight.y) - Math.max(nodeTopLeft.y, selAreaTopLeft.y)) != 0;

            if (isAddMode && isIntersects)
                selectNode(node);
            else if (!isAddMode)
            {
                var funcNodeState = (isIntersects) ? selectNode : unselectNode;
                funcNodeState(node);
            }
        }
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
        clip: false

        keepAspectRatio: true
        useAxisScaleLimit: xyScaleLimit

        showMouseLine: false
        useGridLabels: false
        useBorders: false

        viewTransform: WGViewTransform{
            container: canvasContainer
            xScale: 1
            yScale: -1
        }

        onCanvasPressed:
        {
            resetNodesSelection();
        }

        onSelectArea:
        {
            selectNodesInArea(canvasContainer.mapToItem(canvasItem, min.x, min.y, Math.abs(max.x - min.x), Math.abs(max.y - min.y)), mouse.modifiers & Qt.ShiftModifier);
        }

        onPreviewSelectArea:
        {
            selectNodesInArea(canvasContainer.mapToItem(canvasItem, min.x, min.y, Math.abs(max.x - min.x), Math.abs(max.y - min.y)), mouse.modifiers & Qt.ShiftModifier);
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

        Item
        {
            id: multiDrag
            property var dragObjects: []

            function addDragObj(obj)
            {
                var index = dragObjects.indexOf(obj);
                if (index != -1)
                    return;

                dragObjects.push(obj);
                obj.dragStateChanged.connect(dragStateChanged);
            }

            function removeDragObj(obj)
            {
                var index = dragObjects.indexOf(obj);
                if (index != -1)
                {
                    dragObjects.splice(index, 1);
                    obj.dragStateChanged.disconnect(dragStateChanged);
                }
            }

            function dragStateChanged(node)
            {
                dragObjects.forEach(function(dragObj){
                    if(dragObj === node)
                        return;

                    if (node.isDragActive)
                    {
                        var oldPos = Qt.point(node.x, node.y);
                        var dragObjPos = Qt.point(dragObj.x, dragObj.y);

                        dragObj.x = Qt.binding(function(){ return dragObjPos.x - (oldPos.x - node.x); })
                        dragObj.y = Qt.binding(function(){ return dragObjPos.y - (oldPos.y - node.y); })
                    }
                    else
                    {
                        //unbind
                        dragObj.x = dragObj.x;
                        dragObj.y = dragObj.y;
                    }
                });
            }
        }

        WGGridCanvasItem
        {
            id: canvasItem
            anchors.fill: parent
            clip: false
            viewTransform: canvasContainer.viewTransform


            Repeater
            {
                id: groupRepeater
                model: groupModel
                delegate: NodeGroupArea
                {
                    groupTitle: model.name
                    groupColor: model.color
                    height: model.height
                    width: model.width
                    x:  mapFromItem(graphView, model.xPos, model.yPos).x
                    y:  mapFromItem(graphView, model.xPos, model.yPos).y

                    z: -1

                    onSetPosition: {
                        groupModel.set(index, {"xPos": xPos,"yPos": yPos})
                    }

                    onDeleteNode: {
                        groupModel.remove(index)
                    }

                    onChangeColor: {
                        beginUndoFrame()
                        colorDialog.open()
                    }

                    onChangeTitle: {
                        groupModel.set(index, {"name": title})
                    }
                }
            }

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
                    x: mapFromItem(graphView, nodeObj.nodeCoordX, nodeObj.nodeCoordY).x
                    y: mapFromItem(graphView, nodeObj.nodeCoordX, nodeObj.nodeCoordY).y
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

    ColorDialog {
        id: colorDialog
        title: "Please choose a color"
        showAlphaChannel: false
        onAccepted: {
            groupModel.set(index, {"color": color})
            endUndoFrame();
        }
        onRejected: {
            console.log("Canceled")
            abortUndoFrame();
        }
    }
}
