import QtQuick 2.3

Canvas
{
    id: connectionNodes
    anchors.fill: parent
    antialiasing: true

    property var connectionObj
    property color curveColor: "green"

    property var firstSlot
    property var secondSlot

    property var firstSlotView
    property var secondSlotView

    property var firstNode: (firstSlot) ? firstSlot.node : null
    property var secondNode: (secondSlot) ? secondSlot.node : null

    property var firstNodeView
    property var secondNodeView

    property var firstNodePos
    property var secondNodePos

    property var endPos

    onFirstNodeChanged: initNodeView()
    onSecondNodeChanged: initNodeView()

    onFirstNodeViewChanged: { firstSlotView = firstNodeView.getSlotViewBySlotObj(firstSlot)    }
    onSecondNodeViewChanged:{ secondSlotView = secondNodeView.getSlotViewBySlotObj(secondSlot) }

    onFirstSlotViewChanged: requestPaint()
    onSecondSlotViewChanged: requestPaint()
    onFirstNodePosChanged: requestPaint()
    onSecondNodePosChanged: requestPaint()
    onEndPosChanged: requestPaint()
    onCurveColorChanged: requestPaint()

    function initNodeView()
    {
        if (firstNode && !firstNodeView)
        {
            var node = graphView.getNodeViewById(firstNode.id);

            firstNodeView = node;
            firstNodePos = Qt.binding(function() { return Qt.point(firstNodeView.x, firstNodeView.y) });
        }

        if (secondNode && !secondNodeView)
        {
            var node = graphView.getNodeViewById(secondNode.id);

            secondNodeView = node;
            secondNodePos = Qt.binding(function() { return Qt.point(secondNodeView.x, secondNodeView.y) });
        }
    }

    function tryConnectToSlot(slot)
    {
        if (firstSlot && secondSlot)
            return;

        if (firstSlot)
        {
            var slotObj = slot.slotObj
            createConnection(firstNode.id, firstSlot.id, slotObj.node.id, slotObj.id)
        }
    }

    onPaint:
    {
        var ctx = getContext('2d');
        ctx.clearRect(0, 0, width, height);

        var _startPoint = null;
        var _endPoint = null;

        if (firstSlotView)
        {
            _startPoint = firstSlotView.getSlotAnchor();
            _endPoint = (secondSlotView) ? secondSlotView.getSlotAnchor() : endPos;
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
