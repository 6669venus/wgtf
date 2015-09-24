import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Private 1.0

//TODO: Snapping gives a brief binding error

/*!
 \brief Slider with two handles that encompasses a range of values.
 Snapping will not work if timeObject_ = true

\code{.js}
WGRangeSlider {
    Layout.fillWidth: true
    minimumValue: 0
    maximumValue: 100
    lowerValue_: 25
    value: 75
    stepSize: 1
    enabled: true
    label_: "Range Slider:"
}
\endcode
*/

WGSliderControl {
    objectName: "WGSliderControl"
    rangeSlider_: true

    /*! This property holds the target control's id to be bound to this controls b_Value2 */
    property alias b_Target2: dataBinding2.target

    /*! This property determines b_Target's property which is to be bound to this controls b_Value2 */
    property alias b_Property2: dataBinding2.property

    /*! This property determines this control's value which will drive b_Target2's b_Property2 */
    property alias b_Value2: dataBinding2.value

    Component.onCompleted: {
        if (timeObject_ && snapping_)
        {
            snapping_ = false //Fix to turn off snapping if timeObject_
        }
    }

    Binding {
        id: dataBinding2
    }

    style : WGSliderControlStyle {
        lowerPos: slider.orientation == Qt.Horizontal ? lowerHandle.x : lowerHandle.y
        upperPos: slider.orientation == Qt.Horizontal ? upperHandle.x : upperHandle.y
    }

    RangeModel {
        id: lowerRange
        minimumValue: slider.minimumValue
        maximumValue: slider.maximumValue
        value: lowerValue_
        inverted: slider.orientation == Qt.Horizontal ? false : true

        positionAtMinimum: 0
        positionAtMaximum: slider.orientation == Qt.Horizontal ? slider.width - lowerHandle.width : slider.height - lowerHandle.height
    }

    RangeModel {
        id: upperRange
        minimumValue: slider.minimumValue
        maximumValue: slider.maximumValue
        value: slider.value
        inverted: slider.orientation == Qt.Horizontal ? false : true

        positionAtMinimum: 0
        positionAtMaximum: slider.orientation == Qt.Horizontal ? slider.width - upperHandle.width : slider.height - upperHandle.height
    }



    Rectangle {
        //invisible box for the top and bottom of the handles to snap to.
        id: handleGuide
        height: slider.height * 0.75
        width: slider.width
        color: "transparent"
        parent: slider
        anchors.verticalCenter: parent.verticalCenter
    }

    Rectangle {
        id: lowerHandle

        color: palette.LightPanelColor
        border.width: defaultSpacing.standardBorderSize
        border.color: palette.DarkColor

        height: slider.height * 0.75
        width: slider.height * 0.75

        radius: defaultSpacing.standardRadius

        visible: slider.enabled

        function updatePos() {

            // SLIDERS ARE OVERLAPPING
            if ((lowerHandle.x + (defaultSpacing.minimumRowHeight * 0.75)/2) > (upperHandle.x - (defaultSpacing.minimumRowHeight * 0.75)/2))
            {
                if (slider.height == 0)
                {
                    lowerHandle.height = (defaultSpacing.minimumRowHeight * 0.75) / 2
                    lowerHandle.width = defaultSpacing.minimumRowHeight * 0.75
                }
                else
                {
                    lowerHandle.height = (slider.height * 0.75) / 2
                    lowerHandle.width = slider.height * 0.75
                }
            }
            else // SLIDERS ARE NOT OVERLAPPING
            {
                if (slider.height == 0)
                {
                    lowerHandle.height = defaultSpacing.minimumRowHeight * 0.75
                    lowerHandle.width = defaultSpacing.minimumRowHeight * 0.75
                }
                else
                {
                    lowerHandle.height = slider.height * 0.75
                    lowerHandle.width = slider.height * 0.75
                }
            }

            if (slider.updateValueWhileDragging && !mouseArea.drag.active)
            {
                updateValue_ = false
                lowerRange.position = slider.orientation == Qt.Horizontal ? x : y
                updateValue_ = true
                if (snapping_ && updateValue_)
                {
                    if ((lowerRange.value < snapValue_ * 1.1) && (lowerRange.value > snapValue_ * 0.9))
                    {
                        updateValue_ = false
                        lowerValue_ = snapValue_
                        updateValue_ = true
                    }
                }
                if (timeObject_ && updateValue_)
                {
                    updateValue_ = false
                    sliderLowerValue.value = minsToTime(lowerRange.value)
                    updateValue_ = true
                }
                else if (updateValue_)
                {
                    lowerValue_ = lowerRange.value
                }
            }
        }

        Behavior on height{
            id: lowerGrow
            enabled: false
            NumberAnimation {
                duration: 120
                easing {
                    type: Easing.OutCirc
                    amplitude: 1.0
                    period: 0.5
                }
            }
        }

        // When created both sliders are given default value = 0.
        // If the actual value is also zero the slider wont be updated and may remain small
        onXChanged: {
            updatePos()
            upperHandle.updatePos()
        }
        onYChanged: {
            updatePos()
            upperHandle.updatePos()
        }


        Component.onCompleted: { //Handles are off centre if not parented to the slider
            parent = slider
            if (slider.orientation == Qt.Horizontal)
            {
                anchors.bottom = handleGuide.bottom
                anchors.verticalCenterOffset = (slider.height * 0.25) * -1
            }
            else
            {
                anchors.horizontalCenter = parent.horizontalCenter
            }

            lowerGrow.enabled = true
        }

        //Coloured border when the slider has focus
        Rectangle {
            anchors.fill: parent
            anchors.margins: defaultSpacing.standardBorderSize
            radius: defaultSpacing.standardRadius
            color: "transparent"
            border.width: defaultSpacing.standardBorderSize
            border.color: {
                if (parent.enabled && parent.activeFocus)
                {
                    palette.LighterShade
                }
                else if (parent.enabled && !parent.activeFocus)
                {
                    palette.LightShade
                }
                else if (!parent.enabled)
                {
                    "transparent"
                }
            }
        }
    }

    Rectangle {
        id: upperHandle

        color: palette.LightPanelColor
        border.width: defaultSpacing.standardBorderSize
        border.color: palette.DarkColor

        height: slider.height * 0.75
        width: slider.height * 0.75

        radius: defaultSpacing.standardRadius

        visible: slider.enabled

        // All position changes...
        function updatePos() { // Sliders are overlapping
            if ((lowerHandle.x + (defaultSpacing.minimumRowHeight * 0.75)/2) > (upperHandle.x - (defaultSpacing.minimumRowHeight * 0.75/2)))
            {
                if (slider.height == 0)
                {
                    upperHandle.height = (defaultSpacing.minimumRowHeight * 0.75) / 2
                    upperHandle.width = defaultSpacing.minimumRowHeight * 0.75
                }
                else
                {
                    upperHandle.height = (slider.height * 0.75) / 2
                    upperHandle.width = slider.height * 0.75
                }
            }
            else // Sliders are not overlapping
            {
                if (slider.height == 0)
                {
                    upperHandle.height = defaultSpacing.minimumRowHeight * 0.75
                    upperHandle.width = defaultSpacing.minimumRowHeight * 0.75
                }
                else
                {
                    upperHandle.height = slider.height * 0.75
                    upperHandle.width = slider.height * 0.75
                }
            }

            if (slider.updateValueWhileDragging && !mouseArea.drag.active)
            {
                updateValue_ = false
                upperRange.position = slider.orientation == Qt.Horizontal ? x : y
                updateValue_ = true
                if (snapping_ && updateValue_)
                {
                    if ((upperRange.value < snapValue_ * 1.1) && (upperRange.value > snapValue_ * 0.9))
                    {
                        updateValue_ = false
                        slider.value = snapValue_
                        updateValue_ = true
                    }
                }
                if(timeObject_ && updateValue_)
                {
                    updateValue_ = false
                    sliderValue.value = minsToTime(upperRange.value)
                    updateValue_ = true
                }
                else if (updateValue_)
                {
                    slider.value = upperRange.value
                }
            }
        }

        Behavior on height{
            id: upperGrow
            enabled: false
            NumberAnimation {
                duration: 120
                easing {
                    type: Easing.OutCirc
                    amplitude: 1.0
                    period: 0.5
                }
            }
        }

        // When created both sliders are given default value = 0.
        // If the actual value is also zero the slider wont be updated and may remain small
        // This forces both sliders to update and set their correct size
        onXChanged: {
            updatePos()
            lowerHandle.updatePos()
        }
        onYChanged: {
            updatePos()
            lowerHandle.updatePos()
        }

        Component.onCompleted: { //Handles are off centre if not parented to the slider
            parent = slider

            if (slider.orientation == Qt.Horizontal)
            {
                anchors.top = handleGuide.top
            }
            else
            {
                anchors.horizontalCenter = parent.horizontalCenter
            }

            upperGrow.enabled = true
        }

        //Coloured border when the slider has focus
        Rectangle {
            anchors.fill: parent
            anchors.margins: defaultSpacing.standardBorderSize
            radius: defaultSpacing.standardRadius
            color: "transparent"
            border.width: defaultSpacing.standardBorderSize
            border.color: {
                if (parent.enabled && parent.activeFocus)
                {
                    palette.LighterShade
                }
                else if (parent.enabled && !parent.activeFocus)
                {
                    palette.LightShade
                }
                else if (!parent.enabled)
                {
                    "transparent"
                }
            }
        }
    }

    MouseArea{
        id: mouseArea
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width - (valueBoxWidth_ * 2) - (defaultSpacing.standardMargin * 2)
        property int clickOffset: 0
        property real pressX: 0
        property real pressY: 0
        property bool lowerHandleActive: false
        property bool upperHandleActive: false

        propagateComposedEvents: false

        function lowerClamp ( val ) {
            return Math.max(lowerRange.positionAtMinimum, Math.min(lowerRange.positionAtMaximum, val))
        }

        function upperClamp ( val ) {
            return Math.max(upperRange.positionAtMinimum, Math.min(upperRange.positionAtMaximum, val))
        }

        onMouseXChanged: {
            if (pressed && slider.orientation == Qt.Horizontal && lowerHandleActive)
            {
                var pos = lowerClamp (mouse.x + clickOffset - lowerHandle.width/2)
                if (pos <= upperHandle.x) //make sure handle doesn't go past other handle
                {
                    lowerHandle.x = pos
                }
                else
                {
                    lowerHandle.x = upperHandle.x
                }
                if (Math.abs(mouse.x - pressX) >= Settings.dragThreshold)
                {
                    preventStealing = true
                }
            }
            else if (pressed && slider.orientation == Qt.Horizontal && upperHandleActive)
            {
                var pos = upperClamp (mouse.x + clickOffset - upperHandle.width/2)
                if (pos >= lowerHandle.x) //make sure handle doesn't go past other handle
                {
                    upperHandle.x = pos
                }
                else
                {
                    upperHandle.x = lowerHandle.x
                }
                if (Math.abs(mouse.x - pressX) >= Settings.dragThreshold)
                {
                    preventStealing = true
                }
            }
        }

        onMouseYChanged: {
            if (pressed && slider.orientation != Qt.Horizontal && lowerHandleActive)
            {
                var pos = lowerClamp (mouse.y + clickOffset - lowerHandle.height/2)

                if (pos <= upperHandle.y) //make sure handle doesn't go past other handle
                {
                    lowerHandle.y = pos
                }
                else
                {
                    lowerHandle.y = upperHandle.y
                }
                if (Math.abs(mouse.y - pressY) >= Settings.dragThreshold)
                {
                    preventStealing = true
                }
            }
            else if (pressed && slider.orientation != Qt.Horizontal && upperHandleActive)
            {
                var pos = upperClamp (mouse.y + clickOffset - upperHandle.height/2)
                if (pos >= lowerHandle.y) //make sure handle doesn't go past other handle
                {
                    upperHandle.y = pos
                }
                else
                {
                    upperHandle.y = lowerHandle.y
                }

                if (Math.abs(mouse.y - pressY) >= Settings.dragThreshold)
                {
                    preventStealing = true
                }
            }
        }

        onPressed: {

            var lowerPoint = mouseArea.mapToItem(lowerHandle, mouse.x, mouse.y)
            var upperPoint = mouseArea.mapToItem(upperHandle, mouse.x, mouse.y)

            if (lowerHandle.contains(Qt.point(lowerPoint.x, lowerPoint.y)) && lowerValue_ != minimumValue && slider.value != minimumValue)
            {
                clickOffset = slider.orientation != Qt.Horizontal ? lowerHandle.width/2 - lowerPoint.x : lowerHandle.height/2 - lowerPoint.y

                lowerHandleActive = true

            }
            else if (upperHandle.contains(Qt.point(upperPoint.x, upperPoint.y)))
            {
                clickOffset = slider.orientation != Qt.Horizontal ? upperHandle.width/2 - upperPoint.x : upperHandle.height/2 - upperPoint.y

                upperHandleActive = true

            }
            else
            {
                if (slider.orientation == Qt.Horizontal)
                {
                    if (Math.abs(lowerHandle.x - mouse.x) <= Math.abs(upperHandle.x - mouse.x) && mouse.x < upperHandle.x)
                    {
                        if (lowerValue_ == minimumValue && slider.value == minimumValue) //fix for handles locking up if both at min value
                        {
                            upperHandleActive = true
                        }
                        else
                        {
                            lowerHandleActive = true
                        }
                    }
                    else
                    {
                        upperHandleActive = true
                    }
                }
                else
                {
                    if (Math.abs(lowerHandle.y - mouse.y) <= Math.abs(upperHandle.y - mouse.y) && mouse.y < upperHandle.y)
                    {
                        if (lowerValue_ == minimumValue && slider.value == minimumValue) //fix for handles locking up if both at min value
                        {
                            upperHandleActive = true
                        }
                        else
                        {
                            lowerHandleActive = true
                        }
                    }
                    else
                    {
                        upperHandleActive = true
                    }
                }
            }

            if (lowerHandleActive) lowerHandle.forceActiveFocus()
            if (upperHandleActive) upperHandle.forceActiveFocus()

            pressX = mouse.x
            pressY = mouse.y

        }
        onReleased: {
            // If we don't update while dragging, this is the only
            // moment that the range is updated.
            if (!slider.updateValueWhileDragging && lowerHandleActive)
            {
                lowerRange.position = slider.orientation == Qt.Horizontal ? lowerHandle.x : lowerHandle.y;
            }
            else if (!slider.updateValueWhileDragging && upperHandleActive)
            {
                upperRange.position = slider.orientation == Qt.Horizontal ? upperHandle.x : upperHandle.y;
            }
            clickOffset = 0
            preventStealing = false

            if (lowerRange.position == upperRange.position)
            {
                upperHandle.x = lowerHandle.x
            }

            lowerHandleActive = false
            upperHandleActive = false
        }

        onWheel: {
            if (wheel.angleDelta.y > 0)
            {
                if(upperHandle.focus)
                {
                    slider.value += slider.stepSize
                }
                else if (lowerHandle.focus && lowerValue_ < slider.value)
                {
                    lowerValue_ += slider.stepSize
                }
            }
            else
            {
                if(upperHandle.focus && slider.value > lowerValue_)
                {
                    slider.value -= slider.stepSize
                }
                else if (lowerHandle.focus)
                {
                    lowerValue_ -= slider.stepSize
                }
            }

            // Returns the wheel controls back, otherwise it might break ScrollView behavior
            wheel.accepted = false
        }
    }

    Binding {
        when: !mouseArea.drag.active
        target: lowerHandle
        property: slider.orientation == Qt.Horizontal ? "x" : "y"
        value: lowerRange.position
    }

    Binding {
        when: !mouseArea.drag.active
        target: upperHandle
        property: slider.orientation == Qt.Horizontal ? "x" : "y"
        value: upperRange.position
    }
}
