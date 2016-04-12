import QtQuick 2.5
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import QtQuick.Window 2.1
import BWControls 1.0
import WGControls 2.0

/*!
    \brief Timeline Panel WIP.
*/

WGGridCanvas {
    id: gridCanvas

    focus: true
    useAxis: xGrid
    showMouseLine: true
    showXGridLines: true
    showYGridLines: false

    z: 0

    property alias model: timelineView.model

    timeScale: 10

    // is fps necessary?
    property int framesPerSecond: 30

    property int totalFrames: (framesPerSecond * timeScale)
    property real frameWidth: canvasWidth / totalFrames

    function pixelsToFrames(pixels) {
        return Math.round(pixels / frameWidth)
    }

    function framesToPixels(frames) {
        return frames * frameWidth
    }

    onPreviewSelectArea: {
        timelineView.selectedBars = []
        timelineView.selectionChanged()
    }

    onCanvasPressed: {
        timelineView.selectedBars = []
        timelineView.selectionChanged()
    }

    ListView {
        id: timelineView
        model: barModel

        width: gridCanvas.canvasWidth
        height: gridCanvas.canvasHeight

        x: gridCanvas.viewTransform.transformX(0)
        y: gridCanvas.viewTransform.transformY(1)

        interactive: false

        spacing: 1
        z: 1

        property var selectedBars: []

        //dragging a bar will change these but not a handle
        property real mouseXDragStart: 0
        property real mouseXDragCurrent: 0

        property real deltaValue: (mouseXDragCurrent - mouseXDragStart) / (width / gridCanvas.totalFrames)

        //if a bar or a handle is dragged
        property bool itemDragging: false

        // sent if the bar or handle selection is changed
        signal selectionChanged()

        // sent if a handle is dragged.
        signal handleDragged(real delta, bool minHandle, bool maxHandle)

        // currently the only delegate is the WGTimelineBarSlider but this will change
        delegate: WGTimelineBarSlider {
            id: slider

            height: defaultSpacing.minimumRowHeight
            anchors.left: parent.left
            anchors.right: parent.right
            minimumValue: 0
            maximumValue: gridCanvas.totalFrames
            stepSize: 1
            startFrame: startTime * gridCanvas.framesPerSecond
            endFrame: endTime * gridCanvas.framesPerSecond
            barColor: model.barColor

            barIndex: index

            // check if minPoint and maxPoint lie wholly within min and max
            // or if minPoint and maxPoint y's are wholly within AND minPoint and maxPoint x's are wholly without
            function checkSelection(min, max, minPoint, maxPoint)
            {
                //y selection is pretty basic
                if (minPoint.y >= min.y && maxPoint.y <= max.y)
                {
                    //x selection is more picky. Could make this select any bar the selection crosses... but this may be too coarse.
                    if ((minPoint.x <= min.x && maxPoint.x >= max.x)
                            || (minPoint.x >= min.x && maxPoint.x <= max.x))
                    {
                        return true
                    }
                    else
                    {
                        return false
                    }
                }
            }

            Connections {
                target: gridCanvas

                onPreviewSelectArea: {
                    min = gridCanvas.viewTransform.inverseTransform(min)
                    max = gridCanvas.viewTransform.inverseTransform(max)

                    // find the bar area
                    var minPoint = slider.mapToItem(gridCanvas,slider.__handlePosList[0].range.position,0)
                    var maxPoint = slider.mapToItem(gridCanvas,slider.__handlePosList[1].range.position,slider.height)

                    var barSelected = checkSelection(min,max,minPoint,maxPoint)

                    // add or remove selections as necessary
                    // is it causing poor performance to do this onPreviewSelectArea???
                    var barIndexLocation = -1
                    if (barSelected)
                    {
                        barIndexLocation = slider.ListView.view.selectedBars.indexOf(barIndex)
                        if (barIndexLocation == -1)
                        {
                            slider.ListView.view.selectedBars.push(slider.barIndex)
                            slider.ListView.view.selectionChanged()
                        }
                    }
                    else
                    {
                        barIndexLocation = slider.ListView.view.selectedBars.indexOf(barIndex)
                        if (barIndexLocation != -1)
                        {
                            slider.ListView.view.selectedBars.slice(barIndexLocation, 1)
                            slider.ListView.view.selectionChanged()
                        }
                    }
                }
            }
        }
    }
}
