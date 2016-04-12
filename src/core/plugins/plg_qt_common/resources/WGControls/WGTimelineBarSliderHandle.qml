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
            var handleDelta = sliderHandle.value - slider.initialValues[maxHandle ? 1 : 0]
            slider.ListView.view.handleDragged(handleDelta, !maxHandle, maxHandle)
        }
    }

    Connections {
        target: slider.ListView.view
        // if a bar is being dragged and this handle has been auto selected by it's handle being selected drag this handle
        onMouseXDragCurrentChanged: {
            if (sliderHandle.selected && slider.ListView.view.itemDragging)
            {
                var clampedDelta = slider.ListView.view.deltaValue

                if(slider.initialValues[0] + slider.ListView.view.deltaValue < slider.minimumValue)
                {
                    clampedDelta = slider.minimumValue - slider.initialValues[0]
                }

                else if (slider.initialValues[1] + slider.ListView.view.deltaValue > slider.maximumValue)
                {
                    clampedDelta = slider.maximumValue - slider.initialValues[1]
                }

                sliderHandle.value = maxHandle ? slider.initialValues[1] + clampedDelta : slider.initialValues[0] + clampedDelta
            }
        }

        // if another handle is being dragged and this handle is selected, drag it.
        onHandleDragged: {
            if (sliderHandle.selected && !handleDragging)
            {
                if (minHandle && !sliderHandle.maxHandle)
                {
                    sliderHandle.value = slider.initialValues[0] + delta
                }
                else if (maxHandle && sliderHandle.maxHandle)
                {
                    sliderHandle.value = slider.initialValues[1] +  delta
                }
            }
        }
    }
}

