import QtQuick 2.4
import QtQuick.Window 2.2
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

// Timeline View
Canvas {
    id: timeline;
    contextType: "2d";
    clip: true
    Layout.fillHeight: true
    Layout.fillWidth: true
    property int horizontalPixelGap: 40
    property int verticalPixelGap: 20
	property int textMargin: 8
    property real timeScale: 1;
    property real valueScale: 1;
    property var mouseDragStart;

    property color majorLineColor: palette.PlaceholderTextColor
    property color minorLineColor: Qt.tint(palette.PlaceholderTextColor, palette.MainWindowColor)
    property color backgroundColor: Qt.tint(Qt.tint(Qt.tint(palette.MainWindowColor, palette.TextBoxColor), palette.TextBoxColor), palette.TextBoxColor)
    property color zeroAxisColor: Qt.tint(majorLineColor, '#007f00');
    property color oneAxisColor: Qt.tint(majorLineColor, '#7f6600')

    property var viewTransform: ViewTransform{
		container: timeline
	}

    onTimeScaleChanged: requestPaint();
    onValueScaleChanged: requestPaint();

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

    function isMajor(val, lineGap)
    {
        var mod4 = (Math.abs(val/lineGap) % 4)
        return mod4 < 0.000001;
    }

    function paintHorizontalLines(ctx)
    {
        var startY = timeline.viewTransform.inverseTransform(Qt.point(0,height)).y
        var endY = timeline.viewTransform.inverseTransform(Qt.point(0,0)).y
        // The maximum number of pixels between lines
        var pixelGap = verticalPixelGap
        var numlines = (height / pixelGap)
        var lineGap = nearestQuarter((endY - startY) / numlines);
        var nearStartWhole = Math.floor(startY) - Math.floor(startY) % lineGap
        var countFromWhole = Math.floor((startY - nearStartWhole) / lineGap)
        startY = nearStartWhole + countFromWhole * lineGap;

        // -- Dark lines
        ctx.beginPath();
        ctx.strokeStyle = minorLineColor;
        for (var i=startY;i<endY;i+=lineGap) {
            if(!isMajor(i, lineGap)){
                var y = viewTransform.transformY(i);
                ctx.moveTo(0, Math.floor(y) + 0.5);
                ctx.lineTo(timeline.width, Math.floor(y) + 0.5);
            }
        }
        ctx.stroke();

        // -- Darker lines
        ctx.beginPath();
        ctx.strokeStyle = majorLineColor;
        for (var i=startY;i<endY;i+=lineGap) {
            if(isMajor(i, lineGap)) {
                var y = viewTransform.transformY(i);
                ctx.moveTo(0, Math.floor(y) + 0.5);
                ctx.lineTo(timeline.width, Math.floor(y) + 0.5);
            }
        }
        ctx.stroke();

        // -- Text
        ctx.font = '12px Courier New';
        ctx.strokeStyle = majorLineColor;
        for (var i=startY;i<endY;i+=lineGap) {
            if(isMajor(i, lineGap)) {
                var y = (viewTransform.transformY(i) - 1);
                ctx.strokeText((i*valueScale).toPrecision(3), textMargin, y);
            }
        }
    }

    function paintVerticalLines(ctx)
    {
		var startX = timeline.viewTransform.inverseTransform(Qt.point(0,0)).x
        var endX = timeline.viewTransform.inverseTransform(Qt.point(width,0)).x
        // The maximum number of pixels between lines
        var pixelGap = horizontalPixelGap
        var numlines = (width / pixelGap)
        var lineGap = nearestQuarter((endX - startX) / numlines);
		var nearStartWhole = Math.floor(startX) - Math.floor(startX) % lineGap
        var countFromWhole = Math.floor((startX - nearStartWhole) / lineGap)
        startX = nearStartWhole + countFromWhole * lineGap;

		startX = Math.max(startX, 0)
		endX = Math.min(endX, 1)

        // -- Dark lines
        ctx.beginPath();
        ctx.strokeStyle = minorLineColor;
        for (var i=startX;i<endX;i+=lineGap) {
            if(!isMajor(i, lineGap)){
                var x = viewTransform.transformX(i);
                ctx.moveTo(Math.floor(x) + 0.5, 0);
                ctx.lineTo(Math.floor(x) + 0.5, timeline.height);
            }
        }
        ctx.stroke();

        // -- Darker lines
        ctx.beginPath();
        ctx.strokeStyle = majorLineColor;
        for (var i=startX;i<endX;i+=lineGap) {
            if(isMajor(i, lineGap)) {
                var x = viewTransform.transformX(i);
                ctx.moveTo(Math.floor(x) + 0.5, 0);
                ctx.lineTo(Math.floor(x) + 0.5, timeline.height);
            }
        }
        ctx.stroke();

        // -- Text
        ctx.font = '12px Courier New';
        ctx.strokeStyle = majorLineColor;
        for (var i=startX;i<=endX;i+=lineGap) {
            if(isMajor(i, lineGap)) {
                var text = (i*timeScale).toPrecision(3);
                ctx.resetTransform();
                var x = (viewTransform.transformX(i) - 1);
                ctx.translate(x, ctx.measureText(text).width + textMargin);
                ctx.rotate(-Math.PI/2);
                ctx.strokeText(text, 0, 0);
            }
        }
        ctx.resetTransform();
    }

    onPaint: {
        var height = timeline.height;
        var ctx = timeline.getContext('2d');
        ctx.fillStyle = backgroundColor
        ctx.fillRect(0, 0, timeline.width, timeline.height);

        paintHorizontalLines(ctx)
        paintVerticalLines(ctx)

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
            var screenPos = Qt.point(wheel.x, wheel.y)
            var oldPos = timeline.viewTransform.inverseTransform(screenPos);
            if(Qt.AltModifier & wheel.modifiers) {
                delta = 1 + wheel.angleDelta.x/120.0 * .1;
                timeline.viewTransform.xScale *= delta;
            } else if(Qt.ShiftModifier & wheel.modifiers) {
                timeline.viewTransform.yScale *= delta;
            } else {
                // Zoom into the current mouse location
                timeline.viewTransform.xScale *= delta;
                timeline.viewTransform.yScale *= delta;
            }
            var newScreenPos = timeline.viewTransform.transform(Qt.point(oldPos.x, oldPos.y));
            var shift = Qt.point(screenPos.x - newScreenPos.x, screenPos.y - newScreenPos.y)
            timeline.viewTransform.shift(shift);

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
        }
        onReleased: {
            mouseDragStart = null;
        }
    }
}
