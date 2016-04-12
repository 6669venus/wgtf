import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Private 1.0
import QtQuick.Layouts 1.1
import WGControls 2.0

/*!
 \brief Slider with a large bar and invisible handles that should really only be used in the Timeline Control

\code{.js}

**** TODO **** {
    Layout.fillWidth: true
    minimumValue: 0
    maximumValue: 100
    lowerValue: 25
    upperValue: 75
    stepSize: 1
    enabled: true
}

\endcode
*/


WGSlider {
    objectName: "slider"
    id: slider

    activeFocusOnPress: true

    style: WGTimelineBarSliderStyle{}

    property real startFrame: 0
    property real endFrame: 1

    // initial handle values to apply deltas to
    property var initialValues: []

    property int barIndex: -1

    grooveClickable: false

    handleClamp: false

    clickAreaBuffer: 50

    allowMouseWheel: false

    // tell the view a drag has started
    onBeginDrag: {
        __handlePosList[index].handleDragging = true
        view.itemDragging = true
    }

    onEndDrag: {
        __handlePosList[index].handleDragging = false
        view.itemDragging = false
        initialValues = []
    }

    Connections {
        target: view
        // check to see if the bar is still selected or not and then auto-select the handles
        onSelectionChanged: {
            if (view.selectedBars.indexOf(slider.barIndex) != -1)
            {
                slider.__handlePosList[0].selected = true
                slider.__handlePosList[1].selected = true
            }
            else
            {
                slider.__handlePosList[0].selected = false
                slider.__handlePosList[1].selected = false
            }
        }
        // if anything in the view starts dragging, populate the initial values
        onItemDraggingChanged: {
            if (view.itemDragging)
            {
                for (var i = 0; i < __handlePosList.length; i++)
                {
                    initialValues[i] = __handlePosList[i].value
                }
            }
        }
    }

    property Component barContent: Rectangle {
        id: barContent
        color: slider.barColor

        border.width: 1
        border.color: Qt.darker(color, 1.5)

        // change the styling if bar is selected
        Connections {
            target: view
            onSelectionChanged: {
                barContent.border.color = slider.barIndex === view.selectedBars.indexOf(slider.barIndex) != -1 ? Qt.darker(color, 2.0) : Qt.darker(color, 1.5)
                barSelection.visible = view.selectedBars.indexOf(slider.barIndex) != -1
            }
        }

        // opaque white selection rectangle. Initially invisible
        Rectangle {
            id: barSelection
            anchors.fill: parent
            anchors.margins: defaultSpacing.doubleBorderSize
            visible: false
            color: "#99FFFFFF"
        }

        // Mousearea for dragging the bar around. Will automatically drag the handles and tell the view something is being dragged
        MouseArea {
            id: barMouseArea
            anchors.fill: parent

            // buffer so the handle and bar mouseareas don't overlap
            anchors.leftMargin: 3
            anchors.rightMargin: 3

            // give the view the current mouse delta (only the X really matters)
            onPositionChanged: {
                var currentPos = barMouseArea.mapToItem(slider,mouse.x,mouse.y).x

                //clamp it to the min or max values or weird things happen when the mouse strays too far from the current window
                currentPos.x = Math.max(0, Math.min(slider.width, currentPos.x))
                view.mouseXDragCurrent = currentPos
            }

            onPressed: {
                // populate the initial mouse values in order to calculate delta
                view.mouseXDragStart = barMouseArea.mapToItem(slider,mouse.x,mouse.y).x
                view.mouseXDragCurrent = view.mouseXDragStart

                view.itemDragging = true

                beginUndoFrame();
                // add to the selected bars if Shift clicked
                if ((mouse.button == Qt.LeftButton) && (mouse.modifiers & Qt.ShiftModifier))
                {
                    if (view.selectedBars.indexOf(barIndex) == -1)
                    {
                        view.selectedBars.push(barIndex)
                    }
                }
                // add or remove bars if Ctrl clicked
                else if ((mouse.button == Qt.LeftButton) && (mouse.modifiers & Qt.ControlModifier))
                {
                    var barIndexLocation = view.selectedBars.indexOf(barIndex)
                    if (barIndexLocation == -1)
                    {
                        view.selectedBars.push(barIndex)
                    }
                    else
                    {
                        view.selectedBars.splice(barIndexLocation, 1)
                    }
                }
                // or just select this one if no shift or ctrl
                else if (mouse.button == Qt.LeftButton && view.selectedBars.indexOf(barIndex) == -1)
                {
                    view.selectedBars = [barIndex]
                }

                view.selectionChanged();

                preventStealing = true
                __handleMoving = true
            }

            onReleased: {
                endUndoFrame();
                preventStealing = false
                __handleMoving = false
                view.itemDragging = false

                view.mouseXDragStart = 0
                view.mouseXDragCurrent = 0
            }
        }
    }

    WGTimelineBarSliderHandle {
        id: sliderMinHandle
        minimumValue: slider.minimumValue
        maximumValue: sliderMaxHandle.value
        showBar: false
        rangePartnerHandle: sliderMaxHandle
        value: slider.startFrame
        maxHandle: false

        onValueChanged: {
            slider.startFrame = value
        }

        // need to set max value here or the value might be clamped before the value is valid
        Component.onCompleted: {
            maximumValue = Qt.binding(function() { return sliderMaxHandle.value - 1 })
        }
    }

    WGTimelineBarSliderHandle {
        id: sliderMaxHandle
        minimumValue: sliderMinHandle.value
        maximumValue: slider.maximumValue
        showBar: true
        barMinPos: (sliderMinHandle.value * (parentSlider.__clampedLength / (parentSlider.maximumValue - parentSlider.minimumValue))) + parentSlider.__visualMinPos
        rangePartnerHandle: sliderMinHandle
        value: slider.endFrame
        maxHandle: true

        onValueChanged: {
            slider.endFrame = value
        }

        // need to set min value here or the value might be clamped before the value is valid
        Component.onCompleted: {
            minimumValue = Qt.binding(function() { return sliderMinHandle.value + 1 })
        }
    }
}
