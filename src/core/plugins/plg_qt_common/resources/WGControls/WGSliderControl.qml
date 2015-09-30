import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Private 1.0
import QtQuick.Layouts 1.1
import BWControls 1.0

//TODO: Test orientation = vertical. Create vertical slider. Remove option here

/*!
 \brief Slider with value spinbox.
 Purpose: Provide the user with a single value clamped between min and max value

Example:
\code{.js}
WGSliderControl {
    Layout.fillWidth: true
    minimumValue: 0
    maximumValue: 100
    stepSize: 1
    value: 40
    snapping_: true
    snapValue_: 75
    label_: "Snapping Slider:"
}
\endcode
*/

Item {
    id: sliderFrame
    objectName: "WGSliderControl"

    /*! This property holds the maximum value of the slider.
        The default value is \c{1.0}.
    */
    property alias maximumValue: slider.maximumValue

    /*! This property holds the minimum value of the slider.
        The default value is \c{0.0}.
    */
    property alias minimumValue: slider.minimumValue

    /*!
        This property indicates the slider step size.

        A value of 0 indicates that the value of the slider operates in a
        continuous range between \l minimumValue and \l maximumValue.

        Any non 0 value indicates a discrete stepSize.

        The default value is \c{0.0}.
    */
    property alias stepSize: slider.stepSize

    /*!
        This property holds the layout orientation of the slider.
        The default value is \c Qt.Horizontal.
    */
    /* TODO: It is likely that this does not work. It should be tested and disabled
       A separate vertical slider should probably be made */
    property alias orientation: slider.orientation

    /*! This property defines what sliderstyle styling component to use for this control */
    property alias style: slider.style

    /*! This property defines the value indicated by the control
        The default value is \c 0.0
    */
    //property alias value: slider.value
    property real value: 0.0

    /*! This property defines the colour of the slider */
    property alias barColor_: slider.barColor_

    //TODO: Review this, should it be internal? If so rename with "__" prefix
    /*! \internal */
    property alias slider: slider

    //TODO: Review this, should it be internal? If so rename with "__" prefix
    /*! \internal */
    property alias sliderValue: sliderValue

    //TODO: Review this, should it be internal? If so rename with "__" prefix
    /*! \internal */
    property alias sliderLowerValue: sliderLowerValue

    /*! This property determines the prefix string displayed within the slider textbox.
        Typically used to display unit type.
        The default value is an empty string.
    */
    property string prefix: ""

    /*! This property determines the suffix string displayed within the slider textbox.
        Typically used to display unit type.
        The default value is an empty string.
    */
    property string suffix: ""

    /*! This property converts the number box to display a time in hh:mm.
        Slider value should be in total minutes
    */
    property bool timeObject_: false

    /*! This property defines the number of decimal places displayed in the textbox
        The default value is \c 1
    */
    property int decimals_: 1

    /*! This property adds a single 'sticky' point to the slider.
        The position of the snap point is defined by \c snapValue_
        The default value is \c false
    */
    property bool snapping_: false

    /*! This property defines the location of the sticky snap point when \c snapping_ is set to \c true
        The default value is \c 0.0
    */
    property real snapValue_: 0.0

    /*! This property is used to define the buttons label when used in a WGFormLayout
        The default value is an empty string
    */
    //TODO: This should be renamed, it does not require "_"
    property string label_: ""

    //TODO: Review this, should it be internal? If so rename with "__" prefix
    /*! \internal */
    property int valueBoxWidth_: sliderValue.implicitWidth

    //TODO: Review this, should it be internal? If so rename with "__" prefix
    /*! \internal */
    property alias showValue_: slider.showValue_

    //TODO: Review this, should it be internal? If so rename with "__" prefix
    /*! \internal */
    property alias rangeSlider_: slider.rangeSlider_

    //TODO: Review this, should it be internal? If so rename with "__" prefix
    /*! \internal */
    property bool fakeLowerValue_: false

    //TODO: Review this, should it be internal? If so rename with "__" prefix
    /*! \internal */
    property alias topSnapPoint: topSnapPoint

    //TODO: Review this, should it be internal? If so rename with "__" prefix
    /*! \internal */
    property real lowerValue_: 0

    //TODO: Review this, should it be internal? If so rename with "__" prefix
    /*! \internal */
    property variant oldValue

    //don't change these:
    /*! \internal */
    property bool updateValue_: true
    /*! \internal */
    property real valueWidth: maximumValue - minimumValue
    /*! \internal */
    property int snapOffset: valueWidth % 2 == 0 ? 0 : 1

    implicitHeight: parent.rowHeight_ ? parent.rowHeight_ : 22

    property alias b_Target: dataBinding.target
    property alias b_Property: dataBinding.property
    property alias b_Value: dataBinding.value

    Binding {
        id: dataBinding
    }

    onValueChanged: {
        setValueHelper(slider, "value", sliderFrame.value);
        setValueHelper(sliderFrame, "oldValue", sliderFrame.value);
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
				setValueHelper(sliderFrame, "value", data)
				if(sliderFrame.value != data)
				{
					bPasted = false;
				}
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
        setValueHelper(slider, "value", sliderFrame.value);
        setValueHelper(sliderFrame, "oldValue", sliderFrame.value);
    }

    //convert minutes to hh.mm
    //TODO: Review this, should it be internal?
    /*! \internal */
    function minsToTime(totalMins)
    {
        var hours = Math.floor(totalMins / 60)
        var mins = (totalMins / 60) - hours

        return hours + mins
    }

    //convert minutes to "hh:mm" string
    //TODO: Review this, should it be internal?
    /*! \internal */
    function minsToTimeStr(totalMins)
    {
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
    //TODO: Review this, should it be internal?
    /*! \internal */
    function timeToMins(time) {
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

                if (timeObject_)
                {
                    value = minsToTime(lowerValue_)
                    sliderLowerValue.__text = minsToTimeStr(lowerValue_)
                }
                else
                {
                    value = lowerValue_
                }
            }

            minimumValue: {
                if(timeObject_)
                {
                    minsToTime(slider.minimumValue)
                }
                else
                {
                    slider.minimumValue
                }
            }

            maximumValue: {
                if (rangeSlider_)
                {
                    sliderValue.value
                }
                else
                {
                    if(timeObject_)
                    {
                        minsToTime(slider.maximumValue)
                    }
                    else
                    {
                        slider.maximumValue
                    }
                }
            }

            stepSize: slider.stepSize

            //reset the number box value to the slider value to fix any bad decimals for a timeObject_
            onEditingFinished: {
                if (timeObject_)
                {
                    updateValue_ = false
                    value = minsToTime(lowerValue_)
                    sliderLowerValue.__text = minsToTimeStr(lowerValue_)
                    updateValue_ = true
                }
            }

            onValueChanged: {
                if (timeObject_ && updateValue_)
                {
                    updateValue_ = false
                    lowerValue_ = timeToMins(value)
                    updateValue_ = true
                }
                else if (updateValue_)
                {
                    updateValue_ = false
                    lowerValue_ = value
                    updateValue_ = true
                }
            }

            Connections {
                target: sliderFrame
                onLowerValue_Changed: {
                    if (timeObject_)
                    {
                        sliderLowerValue.value = minsToTime(lowerValue_)
                        sliderLowerValue.__text = minsToTimeStr(lowerValue_)
                    }
                    else
                    {
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
			enabled: globalSettings.wgCopyableEnabled ? false:true

            Layout.preferredWidth: {
                if (orientation == Qt.Horizontal)
                {
                    var roundedWidth = 0
                    if (!showValue_)
                    {
                        roundedWidth = Math.round(sliderFrame.width)
                    }
                    else if (rangeSlider_)
                    {
                        roundedWidth = Math.round(sliderFrame.width - sliderValue.width - sliderLowerValue.width - (defaultSpacing.rowSpacing * 2))
                    }
                    else if (fakeLowerValue_)
                    {
                        roundedWidth = Math.round(sliderFrame.width - sliderValue.width - fakeLowerValue.width - (defaultSpacing.rowSpacing * 2))
                    } else
                    {
                        roundedWidth = Math.round(sliderFrame.width - sliderValue.width - defaultSpacing.rowSpacing)
                    }
                    roundedWidth
                }
                else
                {
                    sliderFrame.width
                }
            }

            Layout.preferredHeight: {
                if (orientation != Qt.Horizontal)
                {
                    var roundedHeight = 0
                    if (rangeSlider_)
                    {
                        roundedHeight = Math.round(sliderFrame.height - sliderValue.height - sliderLowerValue.height - (defaultSpacing.topBottomMargin * 2))
                    }
                    else
                    {
                        roundedHeight = Math.round(sliderFrame.height - sliderValue.height - defaultSpacing.topBottomMargin)
                    }
                    if(snapping_)
                    {
                        if (roundedHeight%2 != 0)
                        {
                            roundedHeight -= defaultSpacing.separatorWidth / 2
                        }
                    }
                    roundedHeight
                }
                else
                {
                    sliderFrame.height
                }
            }

            //override to turn off mousewheel unless first clicked
            MouseArea {
                anchors.fill: parent
                propagateComposedEvents: true
                z: 10

                onWheel: {
                    if (slider.activeFocus)
                    {
                        wheel.accepted = false
                    }
                    else if (!slider.activeFocus)
                    {
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

            onPressedChanged:{
                if(!pressed && (value != oldValue))
                {
                    setValueHelper(sliderFrame, "value", value);
                    setValueHelper(sliderFrame, "oldValue", value);
                }
            }

            onValueChanged: {
                if (snapping_ && updateValue_ && !rangeSlider_)
                {
                    if ((value < snapValue_ * 1.1) && (value > snapValue_ * 0.9))
                    {
                        value = snapValue_
                        updateValue_ = false
                        sliderValue.value = value
                        updateValue_ = true
                    }
                }
                if (timeObject_ && updateValue_ && !rangeSlider_)
                {
                    updateValue_ = false
                    sliderValue.value = minsToTime(value)
                    updateValue_ = true
                }
                else if (updateValue_ && !rangeSlider_)
                {
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

                if (timeObject_)
                {
                    value = minsToTime(slider.value)
                    sliderValue.__text = minsToTimeStr(slider.value)
                }
                else
                {
                    value = slider.value
                }
            }

            minimumValue: {
                if (rangeSlider_)
                {
                    sliderLowerValue.value
                }
                else
                {
                    if (timeObject_)
                    {
                        minsToTime(slider.minimumValue)
                    }
                    else
                    {
                        slider.minimumValue
                    }
                }
            }

            maximumValue: timeObject_ ? minsToTime(slider.maximumValue) : slider.maximumValue

            stepSize: slider.stepSize

            //reset the number box value to the slider value to fix any bad decimals for a timeObject_
            onEditingFinished: {
                if (timeObject_)
                {
                    updateValue_ = false
                    value = minsToTime(slider.value)
                    sliderValue.__text = minsToTimeStr(slider.value)
                    updateValue_ = true
                }
                setValueHelper(sliderFrame, "value", value);
                setValueHelper(sliderFrame, "oldValue", value);
            }

            onValueChanged: {
                if (timeObject_ && updateValue_)
                {
                    updateValue_ = false
                    slider.value = timeToMins(value)
                    updateValue_ = true
                }
                else if (updateValue_)
                {
                    updateValue_ = false
                    slider.value = value
                    updateValue_ = true
                }
            }


            Connections {
                target: sliderFrame
                onValueChanged: {
                    if (timeObject_)
                    {
                        sliderValue.value = minsToTime(slider.value)
                        sliderValue.__text = minsToTimeStr(slider.value)
                    }
                    else
                    {
                        sliderValue.value = value
                    }
                }
            }
        }
    }
}
