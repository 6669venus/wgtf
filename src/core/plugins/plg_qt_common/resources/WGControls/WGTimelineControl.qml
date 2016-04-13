import QtQuick 2.5
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import QtQuick.Window 2.1
import BWControls 1.0
import WGControls 2.0

/*!
    \brief Timeline Panel WIP.
*/

Rectangle {
    id: timelineFrame

    // is fps necessary?
    property int framesPerSecond: 30

    property int totalFrames: (framesPerSecond * timeScale)

    property real frameWidth: gridCanvas.canvasWidth / totalFrames

    property int topMargin: gridCanvas.viewTransform.transformY(1) + timelineToolbar.height

    property alias model: timelineView.model

    property alias timeScale: gridCanvas.timeScale

    signal yPositionChanged(var yPos)

    function setViewYPosition(yPos) {
        if (timelineView.contentY != yPos)
        {
            timelineView.contentY = yPos
        }
    }

    function pixelsToFrames(pixels) {
        return Math.round(pixels / frameWidth)
    }

    function framesToPixels(frames) {
        return frames * frameWidth
    }

    Rectangle {
        id: timelineToolbar
        anchors.left: parent.left
        anchors.right: parent.right
        height: defaultSpacing.minimumRowHeight + defaultSpacing.doubleMargin

        color: palette.mainWindowColor

        RowLayout {
            anchors.fill: parent
            anchors.margins: defaultSpacing.standardMargin
            Item {
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

            WGLabel {
                text: "Duration: "
            }

            WGNumberBox {
                id: durationNumberBox

                Layout.preferredWidth: 50

                value: timeScale
                minimumValue: 0.1
                maximumValue: 100
                stepSize: 0.1
                decimals: 1

                // Need to upgrade WGSpinBox to QtQuick.Controls 1.3
                onEditingFinished: {
                    timelineFrame.timeScale = durationNumberBox.value
                }

                Connections {
                    target: timelineFrame
                    onTimeScaleChanged: {
                        durationNumberBox.value = timeScale
                    }
                }
            }
        }
    }


    WGGridCanvas {
        id: gridCanvas

        anchors.top: timelineToolbar.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        focus: true
        useAxis: xGrid
        showMouseLine: false
        showXGridLines: true
        showYGridLines: false

        z: 0

        onPreviewSelectArea: {
            //timelineView.selectedBars = []
            timelineView.selectionChanged()
        }

        onCanvasPressed: {
            timelineView.selectedBars = []
            timelineView.selectedHandles = []
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

            property var selectedHandles: []

            //dragging a bar will change these but not a handle
            property real mouseXDragStart: 0
            property real mouseXDragCurrent: 0

            property real deltaValue: (mouseXDragCurrent - mouseXDragStart) / (width / totalFrames)

            //if a bar or a handle is dragged
            property bool itemDragging: false

            // sent if the bar or handle selection is changed
            signal selectionChanged()

            // sent if a handle is dragged.
            signal handleDragged(real delta, bool minHandle, bool maxHandle)

            onContentYChanged: {
                timelineFrame.yPositionChanged(contentY)
            }

            // this really doesn't seem like the ideal way to reflect on the model data... but will do for now.
            delegate: Loader {
                id: rowDelegate
                height: defaultSpacing.minimumRowHeight
                anchors.left: parent.left
                anchors.right: parent.right

                property var view: rowDelegate.ListView.view

                sourceComponent: {
                    if (type == "textBox")
                    {
                        return textObject
                    }
                    else if (type == "fullBar")
                    {
                        return fullBar
                    }
                    else if (type == "barSlider")
                    {
                        return barSlider
                    }
                    else if (type == "frameSlider")
                    {
                        return frameSlider
                    }
                }

                // TODO: move these to a WGTimelineBlahDelegate control??

                property Component textObject: WGTextBox {
                    id: textObject
                    text: model.text
                }

                property Component fullBar: WGButtonFrame {
                    id: fullBar
                    color: model.barColor
                    WGLabel {
                        anchors.centerIn: parent
                        text: model.name
                    }
                }

                property Component barSlider: WGTimelineBarSlider {
                    id: barSlider

                    minimumValue: 0
                    maximumValue: totalFrames
                    stepSize: 1
                    startFrame: startTime * framesPerSecond
                    endFrame: endTime * framesPerSecond
                    barColor: model.barColor

                    barIndex: index

                    //TODO duplicating a lot of code for selection here...

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
                            var minPoint = barSlider.mapToItem(gridCanvas,barSlider.__handlePosList[0].range.position,0)
                            var maxPoint = barSlider.mapToItem(gridCanvas,barSlider.__handlePosList[1].range.position,barSlider.height)

                            var barSelected = checkSelection(min,max,minPoint,maxPoint)

                            // add or remove selections as necessary
                            // is it causing poor performance to do this onPreviewSelectArea???
                            var barIndexLocation = -1
                            if (barSelected)
                            {
                                barIndexLocation = view.selectedBars.indexOf(barIndex)
                                if (barIndexLocation == -1)
                                {
                                    view.selectedBars.push(barSlider.barIndex)
                                    view.selectionChanged()
                                }
                            }
                            else
                            {
                                barIndexLocation = view.selectedBars.indexOf(barIndex)
                                if (barIndexLocation != -1)
                                {
                                    view.selectedBars.splice(barIndexLocation, 1)
                                    view.selectionChanged()
                                }
                            }
                        }
                    }
                }

                property Component frameSlider: WGTimelineFrameSlider {
                    id: frameSlider

                    minimumValue: 0
                    maximumValue: totalFrames
                    stepSize: 1

                    showLabel: showHandleLabel

                    //TODO duplicating a lot of code for selection here...

                    // check if minPoint and maxPoint lie wholly within min and max
                    // or if minPoint and maxPoint y's are wholly within AND minPoint and maxPoint x's are wholly without
                    function checkSelection(min, max, point)
                    {
                        //y selection is pretty basic
                        if (point.y >= min.y && point.y <= max.y)
                        {
                            //x selection is more picky. Could make this select any bar the selection crosses... but this may be too coarse.
                            if (point.x >= min.x && point.x <= max.x)
                            {
                                return true
                            }
                            else
                            {
                                return false
                            }
                        }
                    }

                    Repeater {
                        model: keyFrames

                        WGTimelineFrameSliderHandle {
                            minimumValue: frameSlider.minimumValue
                            maximumValue: frameSlider.maximumValue
                            showBar: false
                            value: time * framesPerSecond
                            frameType: type

                            label: frameProperty + " = " + model.value

                            /*
                            onValueChanged: {
                                slider.startFrame = value
                            }*/
                        }
                    }

                    Connections {
                        target: gridCanvas

                        onPreviewSelectArea: {
                            min = gridCanvas.viewTransform.inverseTransform(min)
                            max = gridCanvas.viewTransform.inverseTransform(max)

                            var handlePoint

                            for (var i = 0; i < __handlePosList.length; i++)
                            {
                                handlePoint = frameSlider.mapToItem(gridCanvas,frameSlider.__handlePosList[i].range.position,frameSlider.height / 2)

                                // find the bar area

                                var pointSelected = checkSelection(min,max,handlePoint)

                                // add or remove selections as necessary
                                // is it causing poor performance to do this onPreviewSelectArea???
                                var handleIndexLocation = view.selectedHandles.indexOf(frameSlider.__handlePosList[i])

                                if (pointSelected)
                                {
                                    if (handleIndexLocation == -1)
                                    {
                                        view.selectedHandles.push(__handlePosList[i])
                                        view.selectionChanged()
                                    }
                                }
                                else
                                {
                                    if (handleIndexLocation != -1)
                                    {
                                        view.selectedHandles.splice(handleIndexLocation, 1)
                                        view.selectionChanged()
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
