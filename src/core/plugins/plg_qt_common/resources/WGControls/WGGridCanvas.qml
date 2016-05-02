import QtQuick 2.4
import QtQuick.Window 2.2
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

// Generic Grid Canvas
Canvas {
    id: gridCanvas;
    objectName: "WGGridCanvas"
    contextType: "2d";
    clip: true

    /*! Determines what axis of the grid will be tranformable
        The default value is \c xyGrid*/
    property int useAxis: xyGrid

    /*! Determines if the grid labels will be drawn */
    property bool useGridLabels: true

    /*! Determines if the zeroAxis(green) and oneAxis(yellow) border lines are drawn.
        Continues to draw grid lines beyond borders*/
    property bool useBoarders: true

    // fake enums for useAxis
    readonly property int xyGrid: 0
    readonly property int xGrid: 1
    readonly property int yGrid: 2

    /*! This property determines if the X grid lines will be drawn
    */
    property bool showXGridLines: true

    /*! This property determines if the Y grid lines will be drawn
    */
    property bool showYGridLines: true

    /*! This property determines whether the mouse line position indicator is shown
        The default value is \c true*/
    property bool showMouseLine: true

    /*! This property defines the maximum pixel resolution of the horizontal grid.
        When zoomed the grid will subdivide if it passes this pizel resolution.
        The default value is \c 40
    */
    property int horizontalPixelGap: 40

    /*! This property defines the maximum pixel resolution of the vertical grid.
        When zoomed the grid will subdivide if it passes this pizel resolution.
        The default value is \c 20
    */
    property int verticalPixelGap: 20

    /*! This property defines the margin for value axis text
        The default value is \c 8
    */
    property int textMargin: 8

    /*! This property defines time scale portion of the graph
        The default value is \c 1
    */
    property real timeScale: 1;

    /*! This property defines the value scale portion of the graph
        The default value is \c 1
    */
    property real valueScale: 1;
    property var mouseDragStart;

    property int canvasWidth: Math.abs(viewTransform.transformX(0) - viewTransform.transformX(1))
    property int canvasHeight: Math.abs(viewTransform.transformY(0) - viewTransform.transformY(1))

    property color majorLineColor: palette.placeholderTextColor
    property color minorLineColor: Qt.tint(palette.placeholderTextColor, palette.mainWindowColor)
    property color backgroundColor: Qt.tint(Qt.tint(Qt.tint(palette.mainWindowColor, palette.textBoxColor), palette.textBoxColor), palette.textBoxColor)
    property color zeroAxisColor: Qt.tint(majorLineColor, '#007f00');
    property color oneAxisColor: Qt.tint(majorLineColor, '#7f6600')

    property var viewTransform: WGViewTransform{
        container: gridCanvas
    }

    WGSelectionArea
    {
        id: selectionArea
        onSelectArea: {
            gridCanvas.selectArea(min,max,mouse)
            z = 0
        }
        onPreviewSelectArea: {
            gridCanvas.previewSelectArea(min,max,mouse)
            z = 5
        }
    }

    signal selectArea(point min, point max, var mouse)

    signal previewSelectArea(point min, point max, var mouse)

    signal canvasPressed();

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
        var startY = gridCanvas.viewTransform.inverseTransform(Qt.point(0,height)).y
        var endY = gridCanvas.viewTransform.inverseTransform(Qt.point(0,0)).y
        // The maximum number of pixels between lines
        var pixelGap = verticalPixelGap
        var numlines = (height / pixelGap)
        var lineGap = nearestQuarter((endY - startY) / numlines);
        var nearStartWhole = Math.floor(startY) - Math.floor(startY) % lineGap
        var countFromWhole = Math.floor((startY - nearStartWhole) / lineGap)
        startY = nearStartWhole + countFromWhole * lineGap;

        if (showYGridLines) {
            // -- Dark lines
            ctx.beginPath();
            ctx.strokeStyle = minorLineColor;
            for (var i=startY;i<endY;i+=lineGap) {
                if(!isMajor(i, lineGap)){
                    var y = viewTransform.transformY(i);
                    ctx.moveTo(0, Math.floor(y) + 0.5);
                    ctx.lineTo(gridCanvas.width, Math.floor(y) + 0.5);
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
                    ctx.lineTo(gridCanvas.width, Math.floor(y) + 0.5);
                }
            }
            ctx.stroke();
        }

        // -- Text
        if (useGridLabels) {
            if (useAxis == 0 || useAxis == 2) { // draw Y axis text
                ctx.font = '12px Courier New';
                ctx.strokeStyle = majorLineColor;
                for (var i=startY;i<endY;i+=lineGap) {
                    if(isMajor(i, lineGap)) {
                        var y = (viewTransform.transformY(i) - 1);
                        ctx.strokeText((i*valueScale).toPrecision(3), textMargin, y);
                    }
                }
            }
        }
    }

    function paintVerticalLines(ctx)
    {
        var startX = gridCanvas.viewTransform.inverseTransform(Qt.point(0,0)).x
        var endX = gridCanvas.viewTransform.inverseTransform(Qt.point(width,0)).x
        // The maximum number of pixels between lines
        var pixelGap = horizontalPixelGap
        var numlines = (width / pixelGap)
        var lineGap = nearestQuarter((endX - startX) / numlines);
        var nearStartWhole = Math.floor(startX) - Math.floor(startX) % lineGap
        var countFromWhole = Math.floor((startX - nearStartWhole) / lineGap)
        startX = nearStartWhole + countFromWhole * lineGap;

        if (useBoarders == true) {
            startX = Math.max(startX, 0)
            endX = Math.min(endX, 1)
        }

        if (showXGridLines) {
            // -- Dark lines
            ctx.beginPath();
            ctx.strokeStyle = minorLineColor;
            for (var i=startX;i<endX;i+=lineGap) {
                if(!isMajor(i, lineGap)){
                    var x = viewTransform.transformX(i);
                    ctx.moveTo(Math.floor(x) + 0.5, 0);
                    ctx.lineTo(Math.floor(x) + 0.5, gridCanvas.height);
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
                    ctx.lineTo(Math.floor(x) + 0.5, gridCanvas.height);
                }
            }
            ctx.stroke();
        }

        // -- Text
        if (useGridLabels) {
            if (useAxis == 0 || useAxis == 1) { // draw X axis text
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
            }
        }
        ctx.resetTransform();
    }

    onPaint: {
        var height = gridCanvas.height;
        var ctx = gridCanvas.getContext('2d');
        ctx.fillStyle = backgroundColor
        ctx.fillRect(0, 0, gridCanvas.width, gridCanvas.height);

        // todo re-enable this line. Draw lines for allowable transform axis
        //if(axisType == 0) {
        if(true) {
            paintHorizontalLines(ctx)
            paintVerticalLines(ctx)
        }
        else if (useAxis == 1) {
            paintVerticalLines(ctx)
        }
        else
        {
            paintHorizontalLines(ctx)
        }

        // -- Green lines
        if (useBoarders) {
            ctx.beginPath();
            ctx.strokeStyle = zeroAxisColor
            var pos = viewTransform.transform(Qt.point(0,0));
            if (useAxis == 0) {  // use XY axis
                ctx.moveTo(0, pos.y)
                ctx.lineTo(parent.width, pos.y)
                ctx.moveTo(pos.x, 0)
                ctx.lineTo(pos.x, parent.height)
            }
            else if (useAxis == 1) {  // use X axis
                ctx.moveTo(pos.x, 0)
                ctx.lineTo(pos.x, parent.height)
            }
            else {  // use Y axis
                ctx.moveTo(0, pos.y)
                ctx.lineTo(parent.width, pos.y)
            }
            ctx.stroke();

            // -- Yellow lines
            ctx.beginPath();
            ctx.strokeStyle = oneAxisColor
            var pos = viewTransform.transform(Qt.point(1,1));

            if (useAxis == 0) {  // use XY axis
                ctx.moveTo(pos.x, 0)
                ctx.lineTo(pos.x, parent.height)
                ctx.moveTo(0, pos.y)
                ctx.lineTo(parent.width, pos.y)
            }
            else if (useAxis == 1) { // use X axis
                ctx.moveTo(pos.x, 0)
                ctx.lineTo(pos.x, parent.height)
            }
            else { // use Y axis
                ctx.moveTo(0, pos.y)
                ctx.lineTo(parent.width, pos.y)
            }
            ctx.stroke();
        } // end if (useBoarders)
    }

    function getWidth( startTime, endTime )
    {
        //var startX = gridCanvas.viewTransform.inverseTransform(Qt.point(0,0)).x
        var endX = gridCanvas.viewTransform.inverseTransform(Qt.point(width,0)).x
        console.log("----------endX is " + endX)
        var pixelEndX = viewTransform.transformX(endX);
        console.log("----------pixelEndX is " + pixelEndX)
        var startPixelLocation = (timeScale/startTime * pixelEndX)
        console.log("----------startPixelLocation is " + startPixelLocation)
        var endPixelLocation =  (timeScale/endTime * pixelEndX)
        console.log("----------endPixelLocation is " + endPixelLocation)
        var newWidth = startPixelLocation - endPixelLocation
        console.log("----------newWidth is " + newWidth)
        return newWidth
    }

    Rectangle {
        id: mouseLine;
        height: useAxis == 2 ? 1 : parent.height
        width: useAxis == 2 ? parent.width : 1
        color: majorLineColor
        visible: showMouseLine
    }

    MouseArea
    {
        anchors.fill: parent;
        acceptedButtons: Qt.AllButtons
        onWheel: {
            var delta = 1 + wheel.angleDelta.y/120.0 * .1;
            var screenPos = Qt.point(wheel.x, wheel.y)
            var oldPos = gridCanvas.viewTransform.inverseTransform(screenPos);

            if (useAxis == 0) { //XY axis
                if(Qt.AltModifier & wheel.modifiers) {
                    delta = 1 + wheel.angleDelta.x/120.0 * .1;
                    gridCanvas.viewTransform.xScale *= delta;
                } else if(Qt.ShiftModifier & wheel.modifiers) {
                    gridCanvas.viewTransform.yScale *= delta;
                } else {
                    // Zoom into the current mouse location
                    gridCanvas.viewTransform.xScale *= delta;
                    gridCanvas.viewTransform.yScale *= delta;
                }
            }
            else if (useAxis == 1){
                gridCanvas.viewTransform.xScale *= delta;
            }
            else if (useAxis == 2) {
                gridCanvas.viewTransform.yScale *= delta;
            }
            var newScreenPos = gridCanvas.viewTransform.transform(Qt.point(oldPos.x, oldPos.y));
            var shift = Qt.point(screenPos.x - newScreenPos.x, screenPos.y - newScreenPos.y)
            gridCanvas.viewTransform.shift(shift);

            gridCanvas.requestPaint()
        }

        hoverEnabled: true
        onPositionChanged: {
            if (useAxis == 2){
                mouseLine.y = mouse.y
            }
            else {
                mouseLine.x = mouse.x
            }
            if(mouseDragStart && (mouse.buttons & Qt.MiddleButton))
            {
                var pos = Qt.point(mouse.x, mouse.y)
                var delta = Qt.point(pos.x - mouseDragStart.x, pos.y - mouseDragStart.y)
                if (useAxis == 0) {
                    gridCanvas.viewTransform.origin.x += delta.x
                    gridCanvas.viewTransform.origin.y += delta.y
                }
                else if (useAxis == 1){
                    gridCanvas.viewTransform.origin.x += delta.x
                }
                else {
                    gridCanvas.viewTransform.origin.y += delta.y
                }
                mouseDragStart = pos
                gridCanvas.requestPaint()
            }
        }
        onPressed:{
            mouseDragStart = Qt.point(mouse.x, mouse.y)
            if (mouse.button == Qt.LeftButton && (mouse.modifiers & Qt.ShiftModifier))
            {
                mouse.accepted = false;
            }
            else if (mouse.button == Qt.LeftButton && (mouse.modifiers & Qt.ControlModifier))
            {
                mouse.accepted = false;
            }
            else if (mouse.button == Qt.MiddleButton)
            {
                //for some reason this needs to be here
            }
            else
            {
                canvasPressed()
                mouse.accepted = false;
            }
        }
        onReleased: {
            mouseDragStart = null;
        }
    }
}
