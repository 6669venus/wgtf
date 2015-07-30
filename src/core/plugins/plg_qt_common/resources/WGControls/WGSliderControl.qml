import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Private 1.0
import QtQuick.Layouts 1.1
import BWControls 1.0

//Slider with value textbox
//May need to refer more standard Slider properties

//This may not work with orientation = vertical

Item {
    id: sliderFrame
    property alias maximumValue: slider.maximumValue
    property alias minimumValue: slider.minimumValue
    property alias stepSize: slider.stepSize
    property alias orientation: slider.orientation
    property alias style: slider.style
    property alias value: slider.value

    property alias barColor_: slider.barColor_

    property alias slider: slider
    property alias sliderValue: sliderValue
    property alias sliderLowerValue: sliderLowerValue

    property string prefix: ""
    property string suffix: ""

    //Converts the number box to display a time in hh:mm. Slider value should be in total minutes
    property bool timeObject_: false

    //decimal places
    property int decimals_: 1

    //adds a single 'sticky' point to the slider
    //for multiple points probably best to change stepSize
    property bool snapping_: false
    property real snapValue_: 0.0

    property string label_: ""

    property int valueBoxWidth_: sliderValue.implicitWidth

    property alias showValue_: slider.showValue_
    property alias rangeSlider_: slider.rangeSlider_

    property bool fakeLowerValue_: false

    property alias topSnapPoint: topSnapPoint

    property real lowerValue_: 0

	property variant oldValue

    //don't change these:
    property bool updateValue_: true
    property real valueWidth: maximumValue - minimumValue
    property int snapOffset: valueWidth % 2 == 0 ? 0 : 1

    implicitHeight: {
        if (parent.rowHeight_){
            parent.rowHeight_
        } else {
            22
        }
    }

    property alias b_Target: dataBinding.target
    property alias b_Property: dataBinding.property
    property alias b_Value: dataBinding.value

    Binding {
        id: dataBinding

    }

    // support copy&paste
    WGCopyable {
        id: copyableControl

        BWCopyable {
            id: copyableObject

            onDataCopied : {
                setValue( slider.value )
            }

            onDataPasted : {
                slider.value = data
            }
        }

        onSelectedChanged : {
            if(selected)
            {
                selectControl( copyableObject )
            }
            else
            {
                deselectControl( copyableObject )
            }
        }
    }

    Component.onCompleted: {
        copyableControl.disableChildrenCopyable( sliderFrame );
    }

    //convert minutes to hh.mm
    function minsToTime(totalMins){
        var hours = Math.floor(totalMins / 60)
        var mins = (totalMins / 60) - hours

        return hours + mins
    }

    //convert minutes to "hh:mm" string
    function minsToTimeStr(totalMins){
        var hours = Math.floor(totalMins / 60)
        var mins = (totalMins % 60)

        var returnTime = hours + ":" + mins

        // Prepend "0" to the minutes, so single digit minutes don't look weird
        if ( (mins - 10) < 0 )
        {
            returnTime = hours + ":" + "0" + mins;
        }

        return returnTime
    }

    //convert hh.mm to minutes
    function timeToMins(time){
        var hours = Math.floor(time) * 60
        var mins = (time - Math.floor(time)) * 60

        return hours + mins
    }

    WGExpandingRowLayout {
        spacing: defaultSpacing.rowSpacing

        Rectangle {
            id: fakeLowerValue
            color: "transparent"
            Layout.preferredWidth: visible ? valueBoxWidth_ : 0
            Layout.preferredHeight: sliderFrame.height
            visible: fakeLowerValue_ ? true : false
        }

        WGNumberBox {
            id: sliderLowerValue
            Layout.preferredHeight: sliderFrame.height
            visible: showValue_ && rangeSlider_
            decimals: decimals_
            Layout.preferredWidth: visible ? valueBoxWidth_ : 0

            prefix: sliderFrame.prefix
            suffix: sliderFrame.suffix

            value: lowerValue_

            Component.onCompleted: {
                // We will update the text without the validator if it is a timeObject
                sliderLowerValue.useValidatorOnInputText = !timeObject_

                if (timeObject_){
                    value = minsToTime(lowerValue_)
                    sliderLowerValue.__text = minsToTimeStr(lowerValue_)
                } else {
                    value = lowerValue_
                }
            }

            minimumValue: {
                if(timeObject_){
                    minsToTime(slider.minimumValue)
                } else {
                    slider.minimumValue
                }
            }

            maximumValue: {
                if (rangeSlider_){
                    sliderValue.value
                } else {
                    if(timeObject_){
                        minsToTime(slider.maximumValue)
                    } else {
                        slider.maximumValue
                    }
                }
            }

            stepSize: slider.stepSize

            //reset the number box value to the slider value to fix any bad decimals for a timeObject_
            onEditingFinished: {
                if (timeObject_){
                    updateValue_ = false
                    value = minsToTime(lowerValue_)
                    sliderLowerValue.__text = minsToTimeStr(lowerValue_)
                    updateValue_ = true
                }
            }

            onValueChanged: {
                if(timeObject_ && updateValue_){
                    updateValue_ = false
                    lowerValue_ = timeToMins(value)
                    updateValue_ = true
                } else if (updateValue_){
                    updateValue_ = false
                    lowerValue_ = value
                    updateValue_ = true
                }
            }

            Connections {
                target: sliderFrame
                onLowerValue_Changed: {
                    if (timeObject_){
                        sliderLowerValue.value = minsToTime(lowerValue_)
                        sliderLowerValue.__text = minsToTimeStr(lowerValue_)
                    } else {
                        sliderLowerValue.value = lowerValue_
                    }
                }
            }
        }

        Slider {

            property bool showValue_: true
            property bool rangeSlider_: false

            property color barColor_: palette.HighlightColor

            id: slider
            updateValueWhileDragging: true
            stepSize: 1.0

            Layout.fillWidth: true

            activeFocusOnPress: true


            Layout.preferredWidth: {
                if (orientation == Qt.Horizontal){
                    var roundedWidth = 0
                    if (!showValue_){
                        roundedWidth = Math.round(sliderFrame.width)
                    } else if (rangeSlider_){
                        roundedWidth = Math.round(sliderFrame.width - sliderValue.width - sliderLowerValue.width - (defaultSpacing.rowSpacing * 2))
                    } else if (fakeLowerValue_) {
                        roundedWidth = Math.round(sliderFrame.width - sliderValue.width - fakeLowerValue.width - (defaultSpacing.rowSpacing * 2))
                    } else {
                        roundedWidth = Math.round(sliderFrame.width - sliderValue.width - defaultSpacing.rowSpacing)
                    }
                    roundedWidth
                } else {
                    sliderFrame.width
                }
            }

            Layout.preferredHeight: {
                if (orientation != Qt.Horizontal){
                    var roundedHeight = 0
                    if (rangeSlider_){
                        roundedHeight = Math.round(sliderFrame.height - sliderValue.height - sliderLowerValue.height - (defaultSpacing.topBottomMargin * 2))
                    } else {
                        roundedHeight = Math.round(sliderFrame.height - sliderValue.height - defaultSpacing.topBottomMargin)
                    }
                    if(snapping_){
                        if (roundedHeight%2 != 0){
                            roundedHeight -= defaultSpacing.separatorWidth / 2
                        }
                    }
                    roundedHeight
                } else {
                    sliderFrame.height
                }
            }

            //override to turn off mousewheel unless first clicked
            MouseArea {
                anchors.fill: parent
                propagateComposedEvents: true
                z: 10

                onWheel: {
                    if (slider.activeFocus){
                        wheel.accepted = false
                    } else if (!slider.activeFocus){
                        wheel.accepted = true
                    }
                }

                onClicked: {
                    mouse.accepted = false
                }
                onPressed: {
                    mouse.accepted = false
                }
                onPressAndHold: {
                    mouse.accepted = false
                }
            }

			//Start Undo Frame when slider pressed.
			//Only end undo frame if value has actually changed, otherwise abort
			//This prevents 'Unknown' history event appearing when slider bar is clicked instead of sliding.
			onPressedChanged:{
				if(pressed)
				{
					oldValue = value
					beginUndoFrame();
				}
				else if (value != oldValue)
				{
					endUndoFrame();
				}
				else if (value == oldValue)
				{
					abortUndoFrame();
				}
			}

			onValueChanged: {
                if(snapping_ && updateValue_ && !rangeSlider_){
                    if ((value < snapValue_ * 1.1) && (value > snapValue_ * 0.9)){
                        value = snapValue_
                        updateValue_ = false
                        sliderValue.value = value
                        updateValue_ = true
                    }
                }
                if(timeObject_ && updateValue_ && !rangeSlider_){
                    updateValue_ = false
                    sliderValue.value = minsToTime(value)
                    updateValue_ = true
                } else if (updateValue_ && !rangeSlider_){
                    sliderValue.value = value
                }
            }

            //vertical marks for the snap point

            WGSeparator {
                id: topSnapPoint
                visible: snapping_
                vertical_: true
                z: -1
                anchors.top: parent.top
                height: parent.height / 3

                anchors.horizontalCenter: parent.horizontalCenter
                anchors.horizontalCenterOffset: ((snapValue_ - (valueWidth/2) - snapOffset) * ((slider.width - Math.round(slider.height * 0.75)) / valueWidth))
            }

            WGSeparator {
                id: bottomSnapPoint
                visible: snapping_
                vertical_: true
                z: -1
                anchors.bottom: parent.bottom
                height: parent.height /3
                anchors.horizontalCenter: parent.horizontalCenter

                //Horizontal offset = (Snap Value - Midpoint Value) * ((Slider Pixel Width - Handle Pixel Width) / (Slider Value "Width"))
                anchors.horizontalCenterOffset: ((snapValue_ - (valueWidth/2) - snapOffset) * ((slider.width - Math.round(slider.height * 0.75)) / valueWidth))
            }

            style : WGSliderControlStyle{

            }
        }

        WGNumberBox {
            id: sliderValue

            Layout.preferredHeight: sliderFrame.height
            visible: showValue_
            decimals: decimals_
            Layout.preferredWidth: visible ? valueBoxWidth_ : 0

            prefix: sliderFrame.prefix
            suffix: sliderFrame.suffix

            value: slider.value

            Component.onCompleted: {
                // We will update the text without the validator if it is a timeObject
                sliderValue.useValidatorOnInputText = !timeObject_

                if (timeObject_){
                    value = minsToTime(slider.value)
                    sliderValue.__text = minsToTimeStr(slider.value)
                } else {
                    value = slider.value
                }
            }

            minimumValue: {
                if (rangeSlider_){
                    sliderLowerValue.value
                } else {
                    if(timeObject_){
                        minsToTime(slider.minimumValue)
                    } else {
                        slider.minimumValue
                    }
                }
            }

            maximumValue: {
                if(timeObject_){
                    minsToTime(slider.maximumValue)
                } else {
                    slider.maximumValue
                }
            }

            stepSize: slider.stepSize

            //reset the number box value to the slider value to fix any bad decimals for a timeObject_
            onEditingFinished: {
                if (timeObject_){
                    updateValue_ = false
                    value = minsToTime(slider.value)
                    sliderValue.__text = minsToTimeStr(slider.value)
                    updateValue_ = true
                }
            }

            onValueChanged: {
                if(timeObject_ && updateValue_){
                    updateValue_ = false
                    slider.value = timeToMins(value)
                    updateValue_ = true
                } else if (updateValue_){
                    updateValue_ = false
                    slider.value = value
                    updateValue_ = true
                }
            }

            Connections {
                target: sliderFrame
                onValueChanged: {
                    if (timeObject_){
                        sliderValue.value = minsToTime(slider.value)
                        sliderValue.__text = minsToTimeStr(slider.value)
                    } else {
                        sliderValue.value = value
                    }
                }
            }
        }
    }
}


