import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Private 1.0
import QtQuick.Layouts 1.1
import WGControls 2.0

/*!
 \brief Slider with a large bar and invisible handles

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

    property int barIndex: -1

    property real __valueDelta

    grooveClickable: false

    handleClamp: false

    clickAreaBuffer: 50

    allowMouseWheel: false

    property Component barContent: Rectangle {
        id: barContent
        color: slider.barColor

        border.width: 1
        border.color: slider.barIndex === slider.ListView.view.selectedBar ? Qt.darker(color, 2.0) : Qt.darker(color, 1.5)

        Rectangle {
            id: barSelection
            anchors.fill: parent
            anchors.margins: defaultSpacing.doubleBorderSize
            visible: slider.barIndex === slider.ListView.view.selectedBar
            color: "#44FFFFFF"
        }

        // Mousearea for dragging the bar around. Would be good to make this drag selected handles instead.
        // and clicking the bar always selects the two handles.
        MouseArea {
            id: barMouseArea
            anchors.fill: parent
            anchors.leftMargin: 3
            anchors.rightMargin: 3

            property point initialPos: Qt.point(0,0)
            property point currentPos: Qt.point(0,0)

            property var initialVals: [0]
            property var currentVals: [0]

            property real delta: (currentPos.x - initialPos.x) / (slider.__clampedLength / (slider.maximumValue - slider.minimumValue))

            onPositionChanged: {
                if (pressed && __handleMoving)
                {
                    currentPos = barMouseArea.mapToItem(slider,mouse.x,mouse.y)
                    currentPos.x = Math.max(0, Math.min(slider.width, currentPos.x))

                    var newValues = []
                    var newIndexes = []

                    var clampedDelta = delta

                    for (var i = 0; i < initialVals.length; i++)
                    {
                        if(initialVals[i] + delta < slider.minimumValue)
                        {
                            clampedDelta = slider.minimumValue - initialVals[i]
                        }
                        else if (initialVals[i] + delta > slider.maximumValue)
                        {
                            clampedDelta = slider.maximumValue - initialVals[i]
                        }
                    }

                    for (var j = 0; j < initialVals.length; j++)
                    {
                        newIndexes.push(j)
                        newValues.push(initialVals[j] + clampedDelta)
                    }

                    slider.setHandleValue(newValues,newIndexes)
                }
            }

            onPressed: {
                initialPos = barMouseArea.mapToItem(slider,mouse.x,mouse.y)

                for (var i = 0; i < __handlePosList.length; i++)
                {
                    initialVals[i] = __handlePosList[i].value
                }

                preventStealing = true
                beginUndoFrame();
                __handleMoving = true

                slider.ListView.view.selectedBar = barIndex
            }

            onReleased: {
                endUndoFrame();
                preventStealing = false
                __handleMoving = false

                initialPos = Qt.point(0,0)
                currentPos = Qt.point(0,0)

                initialVals = [0]
                currentVals = [0]
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

        Component.onCompleted: {
            minimumValue = Qt.binding(function() { return sliderMinHandle.value + 1 })
        }
    }
}
