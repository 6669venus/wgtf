import QtQuick 2.0
import BWControls 1.0
import WGControls 1.0

Canvas {
    id: curve
    anchors.fill: parent
    // Bezier Point Data has 3 points, the position and two control points relative to the position
    // point{
    //      point pos;
    //      point cp1;
    //      point cp2;
    // }
    property alias pointRepeater: pointRepeater;
    property var points;
    property color color;
    property bool enabled: true;
    property var _scaleX: viewTransform.xScale;
    property var _scaleY: viewTransform.yScale;
    property var _originX: viewTransform.origin.x;
    property var _originY: viewTransform.origin.y;
    property var viewTransform: ViewTransform{
        container: curve
    }

    on_OriginXChanged: { requestPaint() }
    on_OriginYChanged: { requestPaint() }
    on_ScaleXChanged: { requestPaint() }
    on_ScaleYChanged: { requestPaint() }

	function numPoints()
	{
		var pointIt = iterator(curve.points)
        var count = 0;
		while(pointIt.moveNext()){++count;}
		return count;
	}

    function getPoint(index)
    {
		if(index === -1)
			return null;
        var pointIt = iterator(curve.points)
        var count = 0;
        while(pointIt.moveNext()){
            if(count === index)
                return pointIt.current;
            ++count;
        }
        return null;
    }

	function constrainHandles()
	{
		for(var i = 0; i < pointRepeater.count; ++i){
			var point = pointRepeater.itemAt(i);
			point.constrainHandles();
		}
	}

    WGListModel
    {
    	id: pointModel
    	source: points
    
    	ValueExtension {}
    }
    
    Repeater
    {
		id: pointRepeater
        model: pointModel
        delegate: Point{
            Component.onCompleted:{
                prevPoint = getPoint(index-1)
                nextPoint = getPoint(index+1)
            }

            point: Value;
			parentCurve: curve;
            baseColor: curve.color;
            enabled: curve.enabled;
            viewTransform: curve.viewTransform;
        }
    }
    
    onPaint: {
        var ctx = getContext('2d');
        ctx.clearRect(0, 0, width, height);

    	var curveIt = iterator(curve.points)
        if(curveIt.moveNext()){
            // -- Glow line
            ctx.lineWidth = 4.0;
            ctx.strokeStyle = Qt.rgba(color.r, color.g, color.b, 0.3)
            //ctx.strokeStyle = enabled ? color : Qt.darker(strokeStyle);
            ctx.clearRect(0,0,width,height)
    
            ctx.beginPath();
            var point = curveIt.current;
            var c1 = Qt.point(point.pos.x + point.cp2.x, point.pos.y + point.cp2.y)
            c1 = viewTransform.transform(c1);
            var pos = viewTransform.transform(point.pos);
            ctx.moveTo(viewTransform.transformX(0), pos.y);
            ctx.lineTo(pos.x, pos.y);
			var c2;
            do{
                point = curveIt.current;
                pos = viewTransform.transform(point.pos);
                c2 = Qt.point(point.pos.x + point.cp1.x, point.pos.y + point.cp1.y);
                c2 = viewTransform.transform(c2);
    
                ctx.bezierCurveTo(c1.x, c1.y, c2.x, c2.y, pos.x, pos.y);
    
                c1 = Qt.point(point.pos.x + point.cp2.x, point.pos.y + point.cp2.y)
                c1 = viewTransform.transform(c1);
            } while(curveIt.moveNext())
			ctx.lineTo(viewTransform.transformX(1), pos.y);
            ctx.stroke();
    	}
    	var curveIt = iterator(curve.points)
    	// Until we can get the length property we iterate the collection to count
    	var count = 0;
        if(curveIt.moveNext()){
            // -- Solid line
            ctx.lineWidth = 1.0;
            ctx.strokeStyle = enabled ? color : Qt.darker(color);
    
            ctx.beginPath();
            var point = curveIt.current;
            var c1 = Qt.point(point.pos.x + point.cp2.x, point.pos.y + point.cp2.y)
            c1 = viewTransform.transform(c1);
            var pos = viewTransform.transform(point.pos);
            ctx.moveTo(viewTransform.transformX(0), pos.y);
            ctx.lineTo(pos.x, pos.y);
			do{
                point = curveIt.current;
                pos = viewTransform.transform(point.pos);
                var c2 = Qt.point(point.pos.x + point.cp1.x, point.pos.y + point.cp1.y);
                c2 = viewTransform.transform(c2);
    
                ctx.bezierCurveTo(c1.x, c1.y, c2.x, c2.y, pos.x, pos.y);
    
                c1 = Qt.point(point.pos.x + point.cp2.x, point.pos.y + point.cp2.y)
                c1 = viewTransform.transform(c1);
    			++count;
            } while(curveIt.moveNext())
            ctx.lineTo(viewTransform.transformX(1), pos.y);
            ctx.stroke();
    	}
    }
	
    //Repeater
    //{
    //    model: curve.points
    //    delegate: Point{
    //        point: curve.points[index];
    //        baseColor: curve.color;
    //        enabled: curve.enabled;
    //        cp1Enabled: index != 0
    //        cp2Enabled: index != curve.points.length - 1
    //        viewTransform: curve.viewTransform;
    //    }
    //}
	//
    //onPaint: {
    //    if(curve.points.length > 1){
    //        var ctx = getContext('2d');
    //        ctx.clearRect(0, 0, width, height);
	//
    //        // -- Glow line
    //        ctx.lineWidth = 4.0;
    //        var curveAlpha = curve.enabled ? 1 : .5
    //        ctx.strokeStyle = Qt.rgba(color.r, color.g, color.b, curveAlpha * 0.3)
    //        ctx.clearRect(0,0,width,height)
	//
    //        ctx.beginPath();
    //        var point = curve.points[0];
    //        var c1 = Qt.point(point.pos.x + point.cp2.x, point.pos.y + point.cp2.y)
    //        c1 = viewTransform.transform(c1);
    //        var pos = viewTransform.transform(point.pos);
    //        ctx.moveTo(viewTransform.transformX(0), pos.y)
    //        ctx.lineTo(pos.x, pos.y);
    //        var c2;
    //        for (var k=1; k < curve.points.length; ++k) {
    //            point = curve.points[k];
    //            pos = viewTransform.transform(point.pos);
    //            c2 = Qt.point(point.pos.x + point.cp1.x, point.pos.y + point.cp1.y);
    //            c2 = viewTransform.transform(c2);
	//
    //            ctx.bezierCurveTo(c1.x, c1.y, c2.x, c2.y, pos.x, pos.y);
	//
    //            c1 = Qt.point(point.pos.x + point.cp2.x, point.pos.y + point.cp2.y)
    //            c1 = viewTransform.transform(c1);
    //        }
    //        ctx.lineTo(viewTransform.transformX(1), pos.y)
    //        ctx.stroke();
	//
    //        // -- Solid line
    //        ctx.lineWidth = 1.0;
    //        ctx.strokeStyle = Qt.rgba(color.r, color.g, color.b, curveAlpha);
	//
    //        ctx.beginPath();
    //        point = curve.points[0];
    //        c1 = Qt.point(point.pos.x + point.cp2.x, point.pos.y + point.cp2.y)
    //        c1 = viewTransform.transform(c1);
    //        pos = viewTransform.transform(point.pos);
    //        ctx.moveTo(viewTransform.transformX(0), pos.y)
    //        ctx.lineTo(pos.x, pos.y);
    //        for (k=1; k < curve.points.length; ++k) {
    //            point = curve.points[k];
    //            pos = viewTransform.transform(point.pos);
    //            c2 = Qt.point(point.pos.x + point.cp1.x, point.pos.y + point.cp1.y);
    //            c2 = viewTransform.transform(c2);
	//
    //            ctx.bezierCurveTo(c1.x, c1.y, c2.x, c2.y, pos.x, pos.y);
	//
    //            c1 = Qt.point(point.pos.x + point.cp2.x, point.pos.y + point.cp2.y)
    //            c1 = viewTransform.transform(c1);
    //        }
    //        ctx.lineTo(viewTransform.transformX(1), pos.y)
    //        ctx.stroke();
    //    }
    //}
}
