import QtQuick 2.0

Rectangle {
    id: handle
    // Bezier Point Data has 3 points, the position and two control points relative to the position
    // point{
    //      point pos;
    //      point cp1;
    //      point cp2;
    // }
    function updateHandlePositions(){
        leftHandle.x = handle.point.cp1.x * viewTransform.xScale;
        leftHandle.y = handle.point.cp1.y * viewTransform.yScale;
        rightHandle.x = handle.point.cp2.x * viewTransform.xScale;
        rightHandle.y = handle.point.cp2.y * viewTransform.yScale;
    }

	function constrainHandles()
	{
		if(prevPoint){
			var constrainedX = Math.max(prevPoint.pos.x - point.pos.x, handle.point.cp1.x)
            handle.point.cp1.y = constrainedX ? handle.point.cp1.y * constrainedX / handle.point.cp1.x : 0;
            handle.point.cp1.x = constrainedX;
            leftHandle.x = handle.point.cp1.x * viewTransform.xScale;
            leftHandle.y = handle.point.cp1.y * viewTransform.yScale;
		}
        if (nextPoint) {
			var constrainedX = Math.min(nextPoint.pos.x - point.pos.x, handle.point.cp2.x)
            handle.point.cp2.y = constrainedX ? handle.point.cp2.y * constrainedX / handle.point.cp2.x : 0;
            handle.point.cp2.x = constrainedX;
            rightHandle.x = handle.point.cp2.x * viewTransform.xScale;
            rightHandle.y = handle.point.cp2.y * viewTransform.yScale;
        }
	}

    property var _scaleX: viewTransform.xScale;
    property var _scaleY: viewTransform.yScale;
    property var _originX: viewTransform.origin.x;
    property var _originY: viewTransform.origin.y;
	property var _prevX;
	property var _prevY;
	property var _nextX;
	property var _nextY;
	property var parentCurve;
    property var prevPoint;
    property var nextPoint;
    property var point;
    property bool enabled;
    property color baseColor;
    property string prevState;
    property bool selected: false;
    property bool cp1Enabled: prevPoint !== null;
    property bool cp2Enabled: nextPoint !== null;
    property var viewTransform: ViewTransform{
        container: parent
    }

    visible: enabled;
    width: 5;
    height: width
    color: baseColor
    state: "unselected"

    x: viewTransform.transformX(handle.point.pos.x);
    y: viewTransform.transformY(handle.point.pos.y);

    on_ScaleXChanged: { updateHandlePositions() }
    on_ScaleYChanged: { updateHandlePositions() }
    on_OriginXChanged: { updateHandlePositions() }
    on_OriginYChanged: { updateHandlePositions() }

    onSelectedChanged:{
        state = selected ? 'selected' : 'unselected';
    }

    transform: Translate{
        x: -width / 2;
        y: -height / 2;
    }

    states: [
        State {
            name: "selected";
            PropertyChanges { target: handle; color: Qt.lighter(handle.baseColor); }
            PropertyChanges { target: handle; width: 8; }
            PropertyChanges { target: leftHandle; visible: handle.cp1Enabled; }
            PropertyChanges { target: rightHandle; visible: handle.cp2Enabled; }
        },
        State {
            name: "unselected";
            PropertyChanges { target: handle; color: handle.baseColor; }
            PropertyChanges { target: handle; width: 5; }
            PropertyChanges { target: leftHandle; visible: false; }
            PropertyChanges { target: rightHandle; visible: false; }
        },
        State {
            name: "hovered"
            PropertyChanges { target: handle; color: Qt.lighter(handle.baseColor); }
            PropertyChanges { target: handle; width: 8; }
        }
    ]

    MouseArea {
		property bool dragging: false
        anchors.fill: parent
        drag.target: handle
        drag.minimumX: prevPoint ? viewTransform.transformX(prevPoint.pos.x) : viewTransform.transformX(0)
        drag.maximumX: nextPoint ? viewTransform.transformX(nextPoint.pos.x) : viewTransform.transformX(1)
        drag.maximumY: viewTransform.transformY(0)
        drag.filterChildren: true
        drag.threshold: 0
        hoverEnabled: true
        onPositionChanged: {
            if (drag.active) {
				if(!dragging)
				{
					dragging = true
					beginUndoFrame();
				}
                var pos = viewTransform.inverseTransform(Qt.point(handle.x, handle.y))
                handle.point.pos.x = pos.x;
                handle.point.pos.y = pos.y;
				parentCurve.constrainHandles();
                handle.parent.requestPaint();
            }
        }
        onEntered: {
            if (handle.state === "unselected") {
                prevState = handle.state;
                handle.state = "hovered"
            }
        }
        onExited: {
            if (handle.state === "hovered") {
                handle.state = prevState
            }
        }
        onClicked: {
            handle.selected = !handle.selected;
        }
		onReleased: {
			if(dragging)
			{
				endUndoFrame();
				dragging = false
			}
		}
    }

	Rectangle {
		id: leftTangent
		color: handle.color
		x: handle.width/2
		y: handle.height/2
		visible: leftHandle.visible
		width: Qt.vector2d(leftHandle.x, leftHandle.y).length()
		height: 1
		transform: Rotation {
			angle: Math.acos(Qt.vector2d(leftHandle.x, leftHandle.y).normalized().dotProduct(Qt.vector2d(1,0))) * 180/Math.PI * ((leftHandle.y > 0) ? 1 : -1)
		}
	}

    Rectangle {
        id: leftHandle
        width: 20;
        height: width
        color: "transparent";
        visible: false;
        border.color: Qt.rgba(handle.color.r, handle.color.g, handle.color.b, .1)
        x: handle.point.cp1.x * viewTransform.xScale;
        y: handle.point.cp1.y * viewTransform.yScale;
        transform: Translate{
            x: -width/2 - 2; // Additional 2 pixels due to snapping
            y: -height/2 - 2;
        }

        Rectangle {
            anchors.fill: parent;
            anchors.margins: 8;
            color: handle.color;
        }

        MouseArea {
			property bool dragging: false;
            anchors.fill: parent
            drag.target: leftHandle
            drag.threshold: 0
            drag.minimumX: prevPoint ?
                viewTransform.xScale*(prevPoint.pos.x - point.pos.x) : -Infinity
            drag.maximumX: 0
            onPositionChanged: {
                if (drag.active) {
					if(!dragging)
					{
						dragging = true;
						beginUndoFrame();
					}
                    // -- Move the tangent
                    if (!rightHandle.children[1].drag.active) {
                        handle.point.cp1.x = leftHandle.x / viewTransform.xScale;
                        handle.point.cp1.y = leftHandle.y / viewTransform.yScale;
                    }

                    // -- Force the opposite tangent to the inverse position
                    if (!(mouse.modifiers & Qt.ControlModifier) && nextPoint) {
						handle.point.cp2.x = -handle.point.cp1.x;
                        handle.point.cp2.y = -handle.point.cp1.y;
						constrainHandles();
                    }

                    handle.parent.requestPaint();
                }
            }
			onReleased: {
				if(dragging)
				{
					endUndoFrame();
					dragging = false;
				}
			}
        }
    }

	Rectangle {
		id: rightTangent
		color: handle.color
		x: handle.width/2
		y: handle.height/2
		visible: rightHandle.visible
		width: Qt.vector2d(rightHandle.x, rightHandle.y).length()
		height: 1
		transform: Rotation {
			angle: Math.acos(Qt.vector2d(rightHandle.x, rightHandle.y).normalized().dotProduct(Qt.vector2d(1,0))) * 180/Math.PI * ((rightHandle.y > 0) ? 1 : -1)
		}
	}

    Rectangle {
        id: rightHandle
        width: 20;
        height: width
        color: "transparent";
        visible: false;
        border.color: Qt.rgba(handle.color.r, handle.color.g, handle.color.b, .1)
        x: handle.point.cp2.x * viewTransform.xScale;
        y: handle.point.cp2.y * viewTransform.yScale;
        transform: Translate{
            x: -width/2 - 2; // Additional 2 pixels due to snapping
            y: -height/2 - 2;
        }

        Rectangle {
            anchors.fill: parent;
            anchors.margins: 8;
            color: handle.color;
        }

        MouseArea {
			property bool dragging: false;
            anchors.fill: parent
            drag.target: rightHandle
            drag.threshold: 0
            drag.minimumX: 0
            drag.maximumX: nextPoint ?
                viewTransform.xScale*(nextPoint.pos.x - point.pos.x) : Infinity
            onPositionChanged: {
                if (drag.active) {
					if(!dragging)
					{
						dragging = true;
						beginUndoFrame();
					}
                    // -- Move the tangent
                    if (!leftHandle.children[1].drag.active) {
                        handle.point.cp2.x = rightHandle.x / viewTransform.xScale;
                        handle.point.cp2.y = rightHandle.y / viewTransform.yScale;
                    }

                    // -- Force the opposite tangent to the inverse position
                    if (!(mouse.modifiers & Qt.ControlModifier) && prevPoint) {
                        handle.point.cp1.x = -handle.point.cp2.x;
                        handle.point.cp1.y = -handle.point.cp2.y;
						constrainHandles();
                    }

                    handle.parent.requestPaint();
                }
            }
			onReleased: {
				if(dragging)
				{
					endUndoFrame();
					dragging = false;
				}
			}
        }
    }
}

