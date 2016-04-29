import QtQuick 2.3

Canvas
{
    id: connectionNodes
    anchors.fill: parent
    antialiasing: true

    property color curveColor: "green"

    property var inputSlot
    property var outputSlot

    property var inputNode:  (inputSlot)  ? inputSlot.node  : null
    property var outputNode: (outputSlot) ? outputSlot.node : null

    property var inputNodeView
    property var outputNodeView

    property var inputNodePos
    property var outputNodePos

    property var inputSlotView
    property var outputSlotView

    property var endPos

    onInputNodeChanged: initNodeView()
    onOutputNodeChanged: initNodeView()

    onInputNodeViewChanged: { inputSlotView = inputNodeView.getSlotViewBySlotObj(inputSlot)    }
    onOutputNodeViewChanged:{ outputSlotView = outputNodeView.getSlotViewBySlotObj(outputSlot) }

    onInputSlotViewChanged: requestPaint()
    onOutputSlotViewChanged: requestPaint()
    onInputNodePosChanged: requestPaint()
    onOutputNodePosChanged: requestPaint()
    onEndPosChanged: requestPaint()

    function findNodeByPos(view, nodePos)
    {
        var childPos = mapToItem(view, nodePos.x, nodePos.y);
        var child = view.childAt(childPos.x, childPos.y);

        if (!child)
            return null;

        if (child.objectName == "Node")
            return child;

        return findNodeByPos(child, nodePos);
    }

    function initNodeView()
    {
        if (outputNode && !outputNodeView)
        {
            var nodePos = Qt.point(outputNode.nodeCoordX, outputNode.nodeCoordY);
            var node = findNodeByPos(graphView, nodePos);

            outputNodeView = node;
            outputNodePos = Qt.binding(function() { return Qt.point(outputNodeView.x, outputNodeView.y) });
        }

        if (inputNode && !inputNodeView)
        {
            var nodePos = Qt.point(inputNode.nodeCoordX, inputNode.nodeCoordY);
            var node = findNodeByPos(graphView, nodePos);

            inputNodeView = node;
            inputNodePos = Qt.binding(function() { return Qt.point(inputNodeView.x, inputNodeView.y) });
        }
    }

    function tryConnectToSlot(slot)
    {
        if (inputSlot && outputSlot)
            return;

        var slotObj = slot.slotObj

        if (inputSlot)
        {
            inputSlotConnect = inputSlot;
            outputSlotConnect = slotObj;
            createConnection();
        }
        else if (outputSlot)
        {
            inputSlotConnect = slotObj;
            outputSlotConnect = outputSlot;
            createConnection();
        }
    }


    onPaint:
    {
        var ctx = getContext('2d');
        ctx.clearRect(0, 0, width, height);

        var _startPoint = null;
        var _endPoint = null;

        if (inputSlotView)
        {
            _startPoint = inputSlotView.getSlotAnchor();
            _endPoint = (outputSlotView) ? outputSlotView.getSlotAnchor() : endPos;
        }
        else if (outputSlotView)
        {
            _startPoint = outputSlotView.getSlotAnchor();
            _endPoint = (inputSlotView) ? inputSlotView.getSlotAnchor() : endPos;
        }

        if (_startPoint == null || _endPoint == null)
            return;

        ctx.lineWidth = 4.0;
        ctx.lineCap = "round"
        ctx.strokeStyle = curveColor

        ctx.beginPath()
        ctx.moveTo(_startPoint.x, _startPoint.y);
        ctx.lineTo(_endPoint.x, _endPoint.y);
        ctx.stroke();
    }
}
