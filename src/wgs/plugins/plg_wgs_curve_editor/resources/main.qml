import QtQuick 2.4
import QtQuick.Window 2.2
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import BWControls 1.0
import WGControls 1.0

Rectangle {
    id: curveEditor
    property string title: "Curve Editor"
    property var layoutHints: { 'curveeditor': 1.0, 'bottom': 0.5 }
    property var mouseDragStart;
    property QtObject panelProps: WGPanelProperties{}

    property color majorLineColor: palette.MidLightColor
    property color minorLineColor: palette.MidDarkColor
    property color backgroundColor: palette.DarkColor
    property color zeroAxisColor: Qt.tint(majorLineColor, '#007f00');
    property color oneAxisColor: Qt.tint(majorLineColor, '#7f6600')

    Layout.fillHeight: true
    color: palette.MainWindowColor

    function toggleCurve(index) {
		var curve = curveRepeater.itemAt(index)
        if(curve) {
            curve.enabled = !curve.enabled;
			curve.requestPaint()
        }
    }

	function updatePoints(curve)
	{
        // Update the point handle positions and prev and next points
        var prevPoint = null;
        var prevBezierPoint = null;
        for(var i = 0; i < curve.pointRepeater.count; ++i){
            var point = curve.pointRepeater.itemAt(i);
            point.prevPoint = prevBezierPoint;
            prevBezierPoint = point.point;
            point.nextPoint = null;
            if(prevPoint)
                prevPoint.nextPoint = point.point;
            prevPoint = point
            point.updateHandlePositions();
        }
        curve.requestPaint()
	}

    function addPointsToActiveCurves(mouse)
    {
        var pos = timeline.viewTransform.inverseTransform(Qt.point(mouse.x, mouse.y))
        if(pos.x > 0 && pos.x < 1 ){
            var curveIt = iterator(curves)
            var index = 0;
            while(curveIt.moveNext()){
                var currentCurve = curveRepeater.itemAt(index);
                if(currentCurve.enabled){
                    curveIt.current.add = pos.x
					updatePoints(currentCurve);
                }
                ++index;
            }
        }
    }

    function clearSelection()
    {
        var curveIt = iterator(curves)
        var index = 0;
        while(curveIt.moveNext()){
            var currentCurve = curveRepeater.itemAt(index++);
            for(var i = 0; i < currentCurve.pointRepeater.count; ++i){
                currentCurve.pointRepeater.itemAt(i).selected = false;
            }
        }
    }

	function deleteSelected()
	{
        var curveIt = iterator(curves)
        var index = 0;
        while(curveIt.moveNext()){
            var currentCurve = curveRepeater.itemAt(index++);
            for(var i = 0; i < currentCurve.pointRepeater.count; ++i){
				var point = currentCurve.pointRepeater.itemAt(i);
                if(point.selected){
                    curveIt.current.remove = point.point.pos.x
                    updatePoints(currentCurve);
				}
            }
        }
	}

//    Selection {
//        id: selection
//    }

    ColumnLayout{
        id: contents
        spacing: 2
        anchors.fill: parent

        // Toolbar
        RowLayout {
            id: toolbar
            WGToolButton {
                iconSource: "qrc:///icons/resources/images/x.png"
                onClicked: {
                    toggleCurve(0)
                }
            }
            WGToolButton {
                iconSource: "qrc:///icons/resources/images/y.png"
                onClicked: {
                    toggleCurve(1)
                }
            }
            WGToolButton {
                iconSource: "qrc:///icons/resources/images/z.png"
                onClicked: {
                    toggleCurve(2)
                }
            }
        }


        // Timeline View
        Canvas {
            id: timeline;
            contextType: "2d";
            Layout.fillHeight: true
            Layout.fillWidth: true
            focus: true

            Keys.onPressed:{
                if(event.key === Qt.Key_E && event.modifiers & Qt.ControlModifier){
                    zoomExtents();
                }
				else if(event.key === Qt.Key_Delete)
				{
					deleteSelected();
				}
            }

            function zoomExtents()
            {
                // TODO: use curve points to zoom
                timeline.viewTransform.origin.x = timeline.width * .1
                timeline.viewTransform.origin.y = timeline.height * .9
                timeline.viewTransform.xScale = timeline.width * .8
                timeline.viewTransform.yScale = -timeline.height * .8
                timeline.requestPaint()
            }

            property var viewTransform: ViewTransform{
				container: timeline
			}

            function nearestQuarter(val)
            {
                if( val < 1 )
                {
                    // Find nearest multiple of 1/4
                    var multiplier = 4;
                    while(val * multiplier < 1)
                    {
                        multiplier *= 4;
                    }
                    val = 1/multiplier;
                }
                else if(val < 4)
                    val = 1;
                else
                {
                    // Find nearest multiple of 4
                    val = Math.floor(val) - (Math.floor(val) % 4);
                }
                return val
            }

            function isMajor(val, lineHeight)
            {
                var mod4 = (Math.abs(val/lineHeight) % 4)
                return mod4 < 0.000001;
            }


            onPaint: {
                var height = timeline.height;
                var ctx = timeline.getContext('2d');
                ctx.fillStyle = backgroundColor
                ctx.fillRect(0, 0, timeline.width, timeline.height);

                var startY = timeline.viewTransform.inverseTransform(Qt.point(0,height)).y
                var endY = timeline.viewTransform.inverseTransform(Qt.point(0,0)).y
                // The maximum number of pixels between lines
                var pixelGap = 20
                var numlines = (height / pixelGap)
                var lineHeight = nearestQuarter((endY - startY) / numlines);
                var nearStartWhole = Math.floor(startY) - Math.floor(startY) % lineHeight
                var countFromWhole = Math.floor((startY - nearStartWhole) / lineHeight)
                startY = nearStartWhole + countFromWhole * lineHeight;

                // -- Dark lines
                ctx.beginPath();
                ctx.strokeStyle = minorLineColor;
                for (var i=startY;i<endY;i+=lineHeight) {
                    if(!isMajor(i, lineHeight)){
                        var y = viewTransform.transformY(i);
                        ctx.moveTo(0, Math.floor(y) + 0.5);
                        ctx.lineTo(timeline.width, Math.floor(y) + 0.5);
                    }
                }
                ctx.stroke();

                // -- Darker lines
                ctx.beginPath();
                ctx.strokeStyle = majorLineColor;
                for (var i=startY;i<endY;i+=lineHeight) {
                    if(isMajor(i, lineHeight)) {
                        var y = viewTransform.transformY(i);
                        ctx.moveTo(0, Math.floor(y) + 0.5);
                        ctx.lineTo(timeline.width, Math.floor(y) + 0.5);
                    }
                }
                ctx.stroke();

                // -- Text
                ctx.font = '12px Courier New';
                ctx.strokeStyle = majorLineColor;
                for (var i=startY;i<endY;i+=lineHeight) {
                    if(isMajor(i, lineHeight)) {
                        var y = viewTransform.transformY(i) - 1;
                        ctx.strokeText(i.toPrecision(3), 20, y);
                    }
                }

                // -- Green lines
                ctx.beginPath();
                ctx.strokeStyle = zeroAxisColor
                var pos = viewTransform.transform(Qt.point(0,0));
				ctx.moveTo(0, pos.y)
                ctx.lineTo(parent.width, pos.y)
                ctx.moveTo(pos.x, 0)
                ctx.lineTo(pos.x, parent.height)
                ctx.stroke();

                // -- Yellow lines
                ctx.beginPath();
                ctx.strokeStyle = oneAxisColor
                var pos = viewTransform.transform(Qt.point(1,1));
                ctx.moveTo(pos.x, 0)
                ctx.lineTo(pos.x, parent.height)
                ctx.moveTo(0, pos.y)
                ctx.lineTo(parent.width, pos.y)
                ctx.stroke();
            }

            Rectangle {
                id: mouseLine;
                height: parent.height
                width: 1
                color: majorLineColor
			}

            MouseArea
            {
                anchors.fill: parent;
                acceptedButtons: Qt.AllButtons
                onWheel: {
                    var delta = 1 + wheel.angleDelta.y/120.0 * .1;
                    if(Qt.AltModifier & wheel.modifiers) {
                        delta = 1 + wheel.angleDelta.x/120.0 * .1;
                        timeline.viewTransform.xScale *= delta;
                    } else if(Qt.ShiftModifier & wheel.modifiers) {
                        timeline.viewTransform.yScale *= delta;
                    } else {
                        // Zoom into the current mouse location
                        var screenPos = Qt.point(wheel.x, wheel.y)
                        var oldPos = timeline.viewTransform.inverseTransform(screenPos);
                        timeline.viewTransform.xScale *= delta;
                        timeline.viewTransform.yScale *= delta;
                        var newScreenPos = timeline.viewTransform.transform(Qt.point(oldPos.x, oldPos.y));
                        var shift = Qt.point(screenPos.x - newScreenPos.x, screenPos.y - newScreenPos.y)
                        timeline.viewTransform.shift(shift);
                    }

                    timeline.requestPaint()
                }

                hoverEnabled: true
                onPositionChanged: {
                    mouseLine.x = mouse.x
                    if(mouseDragStart && (mouse.buttons & Qt.MiddleButton))
                    {
                        var pos = Qt.point(mouse.x, mouse.y)
                        var delta = Qt.point(pos.x - mouseDragStart.x, pos.y - mouseDragStart.y)
                        timeline.viewTransform.origin.x += delta.x
                        timeline.viewTransform.origin.y += delta.y
                        mouseDragStart = pos
                        timeline.requestPaint()
                    }
                }
                onPressed:{
                    mouseDragStart = Qt.point(mouse.x, mouse.y)
                    if(mouse.modifiers & Qt.AltModifier)
                    {
                        addPointsToActiveCurves(mouse)
                    }
					else if(mouse.button == Qt.LeftButton)
                    {
                        clearSelection()
                    }
                }
                onReleased: {
                    mouseDragStart = null;
                }
            }

			// Data model coming from C++
            WGListModel
            {
                id: curvesModel
                source: curves
                ValueExtension {}
            }
            
            Repeater
            {
            	id: curveRepeater
            	property var colors: ["#b45b4e", "#99dc74", "#92cfdd", majorLineColor];
                model: curvesModel
                delegate: Curve{
                    points: Value.points
                    color: curveRepeater.colors[index %4]
                    viewTransform: timeline.viewTransform;
                }
            }
			
            //// Temporary data for testing
            //ListModel {
            //    id: curvesModel
            //    property var curves;
            //    Component.onCompleted: {
            //        curves = [
            //            {"points":[
            //                {"pos": {x: 0.0, y: 0.0}, "cp1": {x: -0.2, y: 0}, "cp2": {x: 0.2, y: 0}},
            //                {"pos": {x: 0.5, y: 0.5}, "cp1": {x: -0.1, y: -0.2}, "cp2": {x: 0.1, y: 0.2}},
            //                {"pos": {x: 1.0, y: 1.0}, "cp1": {x: -0.2, y: 0}, "cp2": {x: 0.2, y: 0}}]}
            //            ,{"points":[
            //                {"pos": {x: 0.0, y: 0.0}, "cp1": {x: -0.1, y: 0}, "cp2": {x: 0.1, y: 0.1}},
            //                {"pos": {x: 1.0, y: 0.5}, "cp1": {x: -0.1, y: -0.1}, "cp2": {x: 0.1, y: 0.1}}]}
            //            ,{"points":[
            //                {"pos": {x: 0.0, y: 0.0}, "cp1": {x: -0.1, y: 0}, "cp2": {x: 0.1, y: 0.1}},
            //                {"pos": {x: 0.8, y: 0.1}, "cp1": {x: -0.1, y: -0.1}, "cp2": {x: 0.1, y: 0.1}},
            //                {"pos": {x: 0.9, y: 0.9}, "cp1": {x: -0.1, y: -0.1}, "cp2": {x: 0.1, y: 0.1}},
            //                {"pos": {x: 1.0, y: 0.1}, "cp1": {x: -0.1, y: -0.1}, "cp2": {x: 0.1, y: 0.1}}]}
            //            ];
            //    }
            //}
			//
            //Repeater {
            //    id: curveRepeater
            //    property var colors: ["#b45b4e", "#99dc74", "#92cfdd", majorLineColor];
            //    model: curvesModel.curves
            //    delegate: Curve{
            //        points: curvesModel.curves[index].points
            //        color: curveRepeater.colors[index %4]
            //        viewTransform: timeline.viewTransform;
            //    }
            //}
        }
    }
}
