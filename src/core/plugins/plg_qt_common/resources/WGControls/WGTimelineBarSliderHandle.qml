import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Private 1.0

/*!
 \An invisible Slider handle intended for the WGTimelineBarSlider Control.

 Shouldn't really be needed to be used on it's own but may be useful in some other kind of slider.
*/

WGRangeSliderHandle {
    id: sliderHandle
    objectName: "SliderHandle"

    handleOffset: maxHandle ? -2 : -defaultSpacing.doubleMargin + 2

    hoverCursor: Qt.SizeHorCursor
    dragCursor: Qt.SizeHorCursor

    property bool selected: false

    property bool handleDragging: false

    property Component handleStyle: Item {
        implicitWidth: defaultSpacing.doubleMargin
        implicitHeight: defaultSpacing.minimumRowHeight
    }

    onValueChanged: {
        // send a signal to the timeline view if this handle is the one explicitly being dragged
        if (handleDragging)
        {
            var handleDelta = sliderHandle.value - parentSlider.initialValues[maxHandle ? 1 : 0]
            view.handleDragged(handleDelta, !maxHandle, maxHandle)
        }
    }

    Connections {
        target: view
        // if a bar is being dragged and this handle has been auto selected by it's bar being selected drag this handle
        onMouseXDragCurrentChanged: {
            if (sliderHandle.selected && view.itemDragging)
            {
                var clampedDelta = view.deltaValue

                if(parentSlider.initialValues[0] + clampedDelta < parentSlider.minimumValue)
                {
                    clampedDelta = parentSlider.minimumValue - parentSlider.initialValues[0]
                }
                else if (parentSlider.initialValues[1] + view.deltaValue > parentSlider.maximumValue)
                {
                    clampedDelta = parentSlider.maximumValue - parentSlider.initialValues[1]
                }

                sliderHandle.value = maxHandle ? parentSlider.initialValues[1] + clampedDelta : barSlider.initialValues[0] + clampedDelta
            }
        }

        // if another handle is being dragged and this handle is selected, drag it.
        onHandleDragged: {
            if (sliderHandle.selected && !handleDragging)
            {
                if (minHandle && !sliderHandle.maxHandle)
                {
                    sliderHandle.value = parentSlider.initialValues[0] + delta
                }
                else if (maxHandle && sliderHandle.maxHandle)
                {
                    sliderHandle.value = parentSlider.initialValues[1] +  delta
                }
                else if (!minHandle && !maxHandle)
                {
                    var clampedDelta = delta

                    if(parentSlider.initialValues[0] + clampedDelta < parentSlider.minimumValue)
                    {
                        clampedDelta = parentSlider.minimumValue - parentSlider.initialValues[0]
                    }
                    else if (parentSlider.initialValues[1] + clampedDelta > parentSlider.maximumValue)
                    {
                        clampedDelta = parentSlider.maximumValue - parentSlider.initialValues[1]
                    }

                    if (sliderHandle.maxHandle)
                    {
                        sliderHandle.value = parentSlider.initialValues[1] + clampedDelta
                    }
                    else
                    {
                        sliderHandle.value = parentSlider.initialValues[0] + clampedDelta
                    }
                }
            }
        }
    }
}

