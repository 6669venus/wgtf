import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Private 1.0

/*!
 \An invisible Slider handle intended for the WGTimelineBarSlider Control.

 Shouldn't really be needed to be used on it's own but may be useful in some other kind of slider.
*/

WGSliderHandle {
    id: sliderHandle
    objectName: "SliderHandle"

    //property QtObject previousHandle: null

    //property QtObject nextHandle: null

    property string frameType

    property bool selected: false

    property bool handleDragging: false

    property string label: ""

    property bool keyframeActive: false

    property Component handleStyle: Item {

        implicitWidth: defaultSpacing.minimumRowHeight
        implicitHeight: defaultSpacing.minimumRowHeight

        Rectangle {
            anchors.centerIn: parent
            width: 2
            height: parent.height
            color: palette.lightestShade
        }

        Rectangle {
            id: keyframeWidget
            anchors.centerIn: parent

            height: parent.height - defaultSpacing.doubleMargin
            width: parent.width - defaultSpacing.doubleMargin

            border.width: 1
            border.color: selected ? palette.highlightColor : "transparent"

            color: "#CCCCCC"

            transformOrigin: Item.Center
            rotation: frameType == "linear" ? 45 : 0

            radius: frameType == "bezier" ? height : 0
            clip: true

            Rectangle {
                id: activeHighlight
                anchors.fill: parent
                anchors.margins: 2
                radius: parent.radius
                color: "red"

                visible: true
                opacity: 0

                states: [
                    State {
                        name: "INACTIVE"
                        when: !sliderHandle.keyframeActive
                        PropertyChanges { target: activeHighlight; opacity: 0.0}
                    },
                    State {
                        name: "ACTIVE"
                        when: sliderHandle.keyframeActive
                        PropertyChanges { target: activeHighlight; opacity: 1.0}
                    }

                ]
                transitions: [
                    Transition {
                        from: "ACTIVE"
                        to: "INACTIVE"
                        NumberAnimation { target: activeHighlight; properties: "opacity"; duration: 800}
                    }
                ]
            }

        }

        WGLabel {
            anchors.left: keyframeWidget.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.leftMargin: defaultSpacing.standardMargin

            visible: parentSlider.showLabel
            text: label
        }
    }

    onValueChanged: {
        // send a signal to the timeline view if this handle is the one explicitly being dragged
        if (handleDragging)
        {
            var handleDelta = sliderHandle.value - parentSlider.initialValues[parentSlider.__handlePosList.indexOf(sliderHandle)]
            view.handleDragged(handleDelta, false, false)
        }
    }

    Connections {
        target: parentSlider
        // reset selection if a frame handle is clicked
        onHandleClicked: {
            if (frameSlider.__handlePosList[index] == sliderHandle)
            {
                var handleIndexLocation = view.selectedHandles.indexOf(sliderHandle)

                if (handleIndexLocation == -1)
                {
                    view.selectedHandles = [sliderHandle]
                    view.selectedBars = []
                    view.selectionChanged()
                }
            }
        }

        // add to selection if a frame handle is shift clicked
        onHandleShiftClicked: {
            if (frameSlider.__handlePosList[index] == sliderHandle)
            {
                var handleIndexLocation = view.selectedHandles.indexOf(sliderHandle)

                if (handleIndexLocation == -1)
                {
                    view.selectedHandles.push(sliderHandle)
                }
                else
                {
                    view.selectedHandles.splice(handleIndexLocation, 1)
                }
                view.selectionChanged()
            }
        }
        // add/remove to selection if a frame handle is shift clicked
        onHandleCtrlClicked: {
            if (frameSlider.__handlePosList[index] == sliderHandle)
            {
                var handleIndexLocation = view.selectedHandles.indexOf(sliderHandle)

                if (handleIndexLocation == -1)
                {
                    view.selectedHandles.push(sliderHandle)
                }
                else
                {
                    view.selectedHandles.splice(handleIndexLocation, 1)
                }
                view.selectionChanged()
            }
        }
    }

    Connections {
        target: view
        // if a bar is being dragged and this handle has selected drag this handle
        onMouseXDragCurrentChanged: {
            if (sliderHandle.selected && view.itemDragging)
            {
                sliderHandle.value = frameSlider.initialValues[frameSlider.__handlePosList.indexOf(sliderHandle)] + view.deltaValue
            }
        }

        // if another handle is being dragged and this handle is selected, drag it.
        onHandleDragged: {
            if (sliderHandle.selected && !handleDragging)
            {
                sliderHandle.value = frameSlider.initialValues[frameSlider.__handlePosList.indexOf(sliderHandle)] + delta
            }
        }

        onSelectionChanged: {
            if (view.selectedHandles.indexOf(sliderHandle) != -1)
            {
                sliderHandle.selected = true
            }
            else
            {
                sliderHandle.selected = false
            }
        }
    }
}

