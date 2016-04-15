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

    color: palette.mainWindowColor

    // is fps necessary?
    property int framesPerSecond: 30

    property int totalFrames: (framesPerSecond * timeScale)

    property real frameWidth: gridCanvas.canvasWidth / totalFrames

    property int topMargin: gridCanvas.viewTransform.transformY(1) + timelineToolbar.height

    property int currentFrame: 0

    property alias model: timelineView.model

    property alias timeScale: gridCanvas.timeScale

    property bool previewPlaying: false

    property bool showLabels: true

    signal yPositionChanged(var yPos)

    signal eventFired(var eventName, var eventAction, var eventValue)

    // Sets the yPosition. Useful for linking up another Flickable/ListView
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

    // animates currentFrame by the timeScale (seconds)
    NumberAnimation on currentFrame {
        id: playbackAnim
        from: 0
        to: totalFrames
        running: false
        duration: (timeScale * 1000)
        loops: loop.checked ? Animation.Infinite : 1
    }

    // toolbar for holding useful buttons that affect the timeline
    Rectangle {
        id: timelineToolbar
        anchors.left: parent.left
        anchors.right: parent.right
        height: defaultSpacing.minimumRowHeight + defaultSpacing.doubleMargin

        color: palette.mainWindowColor

        RowLayout {
            anchors.fill: parent
            anchors.margins: defaultSpacing.standardMargin

            // start/stop playback
            WGPushButton {
                id: play
                checkable: true
                iconSource: checked ? "icons/stop_16x16.png" : "icons/play_on_16x16.png"
                onCheckedChanged: {
                    if (checked)
                    {
                        playbackAnim.start()
                        timelineFrame.previewPlaying = true
                    }
                    else
                    {
                        playbackAnim.stop()
                        playbackAnim.paused = false
                        timelineFrame.previewPlaying = false
                        pause.checked = false
                        currentFrame = 0
                    }
                }
            }

            // pause/resume playback
            WGPushButton {
                id: pause
                iconSource: "icons/pause_16x16.png"
                enabled: play.checked
                checkable: true
                onCheckedChanged: {
                    if (checked)
                    {
                        playbackAnim.pause()
                        timelineFrame.previewPlaying = false
                    }
                    else
                    {
                        playbackAnim.resume()
                        timelineFrame.previewPlaying = true
                    }
                }
            }

            // loop playback
            WGPushButton {
                id: loop
                iconSource: "icons/loop_16x16.png"
                checkable: true
                enabled: !play.checked
            }

            // time broken up into seconds and frames
            WGLabel {
                text: "Current Time: "
            }
            WGNumberBox {
                id: currentTimeBox

                Layout.preferredWidth: 50

                value: Math.floor(timelineFrame.currentFrame / timelineFrame.framesPerSecond)
                minimumValue: 0.1
                maximumValue: 100
                stepSize: 1
                decimals: 0
                suffix: "s"

                // Need to upgrade WGSpinBox to QtQuick.Controls 1.3
                onEditingFinished: {
                    timelineFrame.currentFrame = (currentTimeBox.value * timelineFrame.framesPerSecond) + currentFrameBox.value
                }

                Connections {
                    target: timelineFrame
                    onCurrentFrameChanged: {
                        currentTimeBox.value = Math.floor(timelineFrame.currentFrame / timelineFrame.framesPerSecond)
                    }
                }
            }

            WGNumberBox {
                id: currentFrameBox

                Layout.preferredWidth: 50

                value: timelineFrame.currentFrame % timelineFrame.framesPerSecond
                minimumValue: 0
                maximumValue: timelineFrame.framesPerSecond
                stepSize: 1
                decimals: 0
                suffix: "f"

                // Need to upgrade WGSpinBox to QtQuick.Controls 1.3
                onEditingFinished: {

                    // TODO make it tick back a second if pressing the down arrow
                    if (currentFrameBox.value == timelineFrame.framesPerSecond)
                    {
                        currentTimeBox.value += 1
                        currentFrameBox.value = 0
                    }

                    timelineFrame.currentFrame = (currentTimeBox.value * timelineFrame.framesPerSecond) + currentFrameBox.value
                }

                Connections {
                    target: timelineFrame
                    onCurrentFrameChanged: {
                        currentFrameBox.value = timelineFrame.currentFrame % timelineFrame.framesPerSecond
                    }
                }
            }

            // changing the framerate at the moment does bad things.
            WGLabel {
                text: framesPerSecond + "( fps)"
            }

            // show frame labels
            WGPushButton {
                id: showLables
                iconSource: checked ? "icons/tag_on_16x16.png" : "icons/tag_off_16x16.png"
                checkable: true
                checked: timelineFrame.showLabels

                onClicked: {
                    timelineFrame.showLabels = !timelineFrame.showLabels
                }
            }

            Item {
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

            // TODO: Make changing the duration less destructive
            WGLabel {
                text: "Duration: "
            }

            WGNumberBox {
                id: durationNumberBox

                Layout.preferredWidth: 50

                value: timeScale
                minimumValue: 0.1
                maximumValue: 100
                stepSize: 1
                decimals: 0
                suffix: "s"

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
        anchors.bottom: timelineArea.top
        anchors.left: parent.left
        anchors.right: parent.right

        implicitWidth: 800

        focus: true
        useAxis: xGrid
        showMouseLine: false
        showXGridLines: true
        showYGridLines: false

        z: 0

        // Trigger selection changes on dragging out a selection rectangle
        onPreviewSelectArea: {
            timelineView.selectionChanged()
        }

        // Reset all selection on a single click of the canvas
        onCanvasPressed: {
            timelineView.selectedBars = []
            timelineView.selectedHandles = []
            timelineView.selectionChanged()
        }

        // Draggable current time handle that moves automatically on playback.
        WGSlider {
            id: frameScrubber
            width: gridCanvas.canvasWidth
            height: defaultSpacing.minimumRowHeight
            x: gridCanvas.viewTransform.transformX(0)
            y: gridCanvas.viewTransform.transformY(1) - defaultSpacing.minimumRowHeight - defaultSpacing.standardMargin
            z: 5
            minimumValue: 0
            maximumValue: totalFrames
            stepSize: 1

            enabled: !timelineFrame.previewPlaying

            handleClamp: false

            onValueChanged: {
                timelineFrame.currentFrame = value
            }

            Connections {
                target: timelineFrame
                onCurrentFrameChanged: {
                    frameScrubber.value = currentFrame
                }
            }

            style: WGSliderStyle {
                groove: Item {
                    implicitWidth: defaultSpacing.minimumRowHeight
                    implicitHeight: defaultSpacing.minimumRowHeight
                    WGTextBoxFrame {
                        radius: defaultSpacing.standardRadius
                        anchors.fill: parent
                    }
                }
                bar: null
            }

            // small chunky handle with a thin red line beneath it
            WGSliderHandle {
                showBar: false
                handleStyle: WGButtonFrame {
                    id: scrubberHandle
                    implicitWidth: defaultSpacing.minimumRowHeight - defaultSpacing.rowSpacing * 2
                    implicitHeight: defaultSpacing.minimumRowHeight
                    color: palette.mainWindowColor
                    borderColor: frameScrubber.enabled ? palette.darkerShade : palette.darkShade
                    highlightColor: frameScrubber.hoveredHandle === handleIndex ? palette.lighterShade : "transparent"
                    innerBorderColor: frameScrubber.__activeHandle === handleIndex && frameScrubber.activeFocus ? palette.highlightShade : "transparent"

                    Rectangle {
                        anchors.top: parent.top
                        anchors.topMargin: -defaultSpacing.standardMargin
                        anchors.horizontalCenter: parent.horizontalCenter
                        height: gridCanvas.height - gridCanvas.viewTransform.transformY(1) + defaultSpacing.doubleMargin + defaultSpacing.minimumRowHeight
                        z: -1
                        width: 2
                        color: "#77FF0000"
                    }
                }
                onValueChanged: {
                    frameScrubber.value = value
                }
            }
        }


        // The list of bars, keyframes etc.
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

            // dragging a bar will change these but not a handle
            property real mouseXDragStart: 0
            property real mouseXDragCurrent: 0

            // records current mouse movements when dragging any handle
            property real deltaValue: (mouseXDragCurrent - mouseXDragStart) / (width / totalFrames)

            // if a bar or a handle is dragged
            property bool itemDragging: false

            // sent if the bar or handle selection is changed
            signal selectionChanged()

            // sent if a handle is dragged.
            signal handleDragged(real delta, bool minHandle, bool maxHandle)

            // sends any vertical movement of the content to the root (useful for linking other Flickables/ListViews)
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

                // a full width textbox, possibly for entering scripts? conditions? comments?
                property Component textObject: WGTextBox {
                    id: textObject
                    text: model.text
                }

                // a full width non interactive bar with a label. Useful for organisation mostly.
                property Component fullBar: WGButtonFrame {
                    id: fullBar
                    color: model.barColor
                    WGLabel {
                        anchors.centerIn: parent
                        text: model.name
                    }
                }

                // coloured moveable and scalable bar
                property Component barSlider: WGTimelineBarSlider {
                    id: barSlider

                    minimumValue: 0
                    maximumValue: totalFrames
                    stepSize: 1
                    startFrame: startTime * timelineFrame.framesPerSecond
                    endFrame: endTime * timelineFrame.framesPerSecond
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

                        // check to see if bar is selected
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

                // a multi handle slider of keyframes
                property Component frameSlider: WGTimelineFrameSlider {
                    id: frameSlider

                    minimumValue: 0
                    maximumValue: totalFrames
                    stepSize: 1

                    showLabel: timelineFrame.showLabels

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
                            id: frameSliderHandle
                            minimumValue: frameSlider.minimumValue
                            maximumValue: frameSlider.maximumValue
                            showBar: false
                            value: time * timelineFrame.framesPerSecond
                            frameType: type

                            label: eventName + " " + eventProperty + " " + eventAction + " " + eventValue

                            // if scrubber is over frame, fireoff active state and trigger event
                            Connections {
                                target:timelineFrame
                                onCurrentFrameChanged:{
                                    if (currentFrame == frameSliderHandle.value)
                                    {
                                        frameSliderHandle.keyframeActive = true
                                        timelineFrame.eventFired(frameSliderHandle.eventName, frameSliderHandle.eventAction, frameSliderHandle.eventValue)
                                    }
                                    else
                                    {
                                        frameSliderHandle.keyframeActive = false
                                    }
                                }
                            }
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

    // Non-interactive depiction of total duration and visible duration. Maths too hard to make it interactive at the moment.
    WGSlider {
        id: timelineArea

        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        handleClamp: false

        property real totalWidth: realMax - realMin

        property real realMin: gridCanvas.width > 0 ? Math.min(Math.round(gridCanvas.viewTransform.transformX(0)), 0) : 0
        property real realMax: gridCanvas.width > 0 ? Math.max(Math.round(gridCanvas.viewTransform.transformX(1)), gridCanvas.width) : 1

        property real leftBound: Math.max(gridCanvas.viewTransform.inverseTransform(Qt.point(0,0)).x,0)
        property real rightBound: Math.min(gridCanvas.viewTransform.inverseTransform(Qt.point(gridCanvas.width,0)).x,1)

        property real startFrame: gridCanvas.width > 0 ? leftBound : 0
        property real endFrame: gridCanvas.width > 0 ? rightBound : 1

        grooveClickable: false
        allowMouseWheel: false

        stepSize: 0.001

        height: defaultSpacing.standardRowHeight

        minimumValue: 0
        maximumValue: 1

        style: WGSliderStyle {
            groove: WGTextBoxFrame {
                radius: defaultSpacing.standardRadius
                anchors.verticalCenter: parent.verticalCenter
                implicitWidth: defaultSpacing.minimumRowHeight
                implicitHeight: defaultSpacing.minimumRowHeight
                color: control.enabled ? palette.textBoxColor : "transparent"
            }
            bar: Item {
                clip: true
                WGButtonFrame {
                    anchors.fill: parent
                    anchors.margins: defaultSpacing.standardBorderSize
                    highlightColor: "#77AAAAAA"
                }
            }
        }

        onLeftBoundChanged: {
            if (!__handleMoving)
            {
                sliderMinHandle.value = leftBound
            }
        }
        onRightBoundChanged: {
            if (!__handleMoving)
            {
                sliderMaxHandle.value = rightBound
            }
        }

        // invisible handles
        WGSliderHandle {
            id: sliderMinHandle
            minimumValue: timelineArea.minimumValue
            maximumValue: timelineArea.maximumValue
            showBar: false
            property QtObject rangePartnerHandle: sliderMaxHandle
            handleStyle: Item {}
        }

        WGSliderHandle {
            id: sliderMaxHandle
            minimumValue: timelineArea.minimumValue
            maximumValue: timelineArea.maximumValue
            showBar: true
            barMinPos: (sliderMinHandle.value * (timelineArea.__clampedLength / timelineArea.maximumValue)) + timelineArea.__visualMinPos
            property QtObject rangePartnerHandle: sliderMinHandle
            handleStyle: Item {}
        }
    }
}
