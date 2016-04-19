import QtQuick 2.5
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

/*!
    A delegate for the ListView inside WGTimelineControl to show a WGTimelineFrameSlider
*/

WGTimelineFrameSlider {
    id: frameSlider

    property QtObject view
    property QtObject grid
    property QtObject rootFrame
    property QtObject keys

    // tell the view a drag has started
    onBeginDrag: {
        view.itemDragging = true
        __handlePosList[index].handleDragging = true
    }

    // reset when the drag has stopped
    onEndDrag: {
        view.itemDragging = false
        __handlePosList[index].handleDragging = false
        initialValues = []
    }

    onHandleClicked: {

        // pre-emptively populate the initial drag values before any dragging
        for (var i = 0; i < frameSlider.__handlePosList.length; i++)
        {
            frameSlider.initialValues[i] = frameSlider.__handlePosList[i].value
        }

        // reset selection if this handle is not selected.
        var handleIndexLocation = view.selectedHandles.indexOf(frameSlider.__handlePosList[index])

        if (handleIndexLocation == -1)
        {
            view.selectedHandles = [frameSlider.__handlePosList[index]]
            view.selectedBars = []
            view.selectionChanged()
        }
    }

    // add handle to selection
    onHandleShiftClicked: {
        var handleIndexLocation = view.selectedHandles.indexOf(frameSlider.__handlePosList[index])

        if (handleIndexLocation == -1)
        {
            view.selectedHandles.push(frameSlider.__handlePosList[index])
        }
        else
        {
            view.selectedHandles.splice(handleIndexLocation, 1)
        }
        view.selectionChanged()
    }

    // add or remove handle from selection
    onHandleCtrlClicked: {
        var handleIndexLocation = view.selectedHandles.indexOf(frameSlider.__handlePosList[index])

        if (handleIndexLocation == -1)
        {
            view.selectedHandles.push(frameSlider.__handlePosList[index])
        }
        else
        {
            view.selectedHandles.splice(handleIndexLocation, 1)
        }
        view.selectionChanged()
    }

    // if a handle is the one being explicitly dragged by the user, let the view know a drag is happening
    onChangeValue: {
        var handle = frameSlider.__handlePosList[index]
        if (handle.handleDragging)
        {
            var handleDelta = (handle.value - frameSlider.initialValues[index]) * (view.width / rootFrame.totalFrames)
            view.itemDragged(handleDelta, false, false, false)
        }
    }

    Repeater {
        model: keys

        WGTimelineFrameSliderHandle {
            id: frameSliderHandle
            minimumValue: frameSlider.minimumValue
            maximumValue: frameSlider.maximumValue
            showBar: false
            value: time * rootFrame.framesPerSecond
            frameType: type

            // temporary label for now
            label: eventName + " " + eventProperty + " " + eventAction + " " + String(eventValue)

            // if scrubber is over frame, fire off active state and trigger event
            Connections {
                target:rootFrame
                onCurrentFrameChanged:{
                    if (rootFrame.currentFrame == frameSliderHandle.value)
                    {
                        frameSliderHandle.keyframeActive = true
                        rootFrame.eventFired(frameSliderHandle.eventName, frameSliderHandle.eventAction, frameSliderHandle.eventValue)
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
        target: view

        //update selection
        onSelectionChanged: {
            for (var i = 0; i < frameSlider.__handlePosList.length; i++)
            {
                if (view.selectedHandles.indexOf(frameSlider.__handlePosList[i]) != -1)
                {
                    frameSlider.__handlePosList[i].selected = true
                }
                else
                {
                    frameSlider.__handlePosList[i].selected = false
                }
            }
        }

        // if anything in the view starts dragging, populate the initial values
        onItemDraggingChanged: {
            if (view.itemDragging)
            {
                for (var i = 0; i < frameSlider.__handlePosList.length; i++)
                {
                    frameSlider.initialValues[i] = frameSlider.__handlePosList[i].value
                }
            }
        }

        onItemDragged: {
            var clampedDelta = delta / (view.width / rootFrame.totalFrames)

            frameSlider.dragSelectedHandles(clampedDelta)
        }
    }

    Connections {
        target: grid

        onPreviewSelectArea: {

            var handlePoint

            for (var i = 0; i < __handlePosList.length; i++)
            {
                handlePoint = frameSlider.mapToItem(grid,frameSlider.__handlePosList[i].range.position,frameSlider.height / 2)

                // find the bar area

                var pointSelected = view.checkSelection(min,max,handlePoint,handlePoint)

                // add or remove selections as necessary
                // is it causing poor performance to do this onPreviewSelectArea???
                var handleIndexLocation = view.selectedHandles.indexOf(frameSlider.__handlePosList[i])
                if (pointSelected)
                {
                    if (handleIndexLocation == -1)
                    {
                        view.selectedHandles.push(frameSlider.__handlePosList[i])
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
