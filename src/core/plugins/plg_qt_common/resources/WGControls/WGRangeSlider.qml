import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Private 1.0
import QtQuick.Layouts 1.1
import BWControls 1.0

/*!
 \brief Slider with two handles that encompasses a range of values.

\code{.js}
WGRangeSlider {
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

    /*! This property defines the lower value indicated by the control
        The default value is \c 0.0
    */

    property real lowerValue

    /*! This property defines the upper value indicated by the control
        The default value is \c 0.0
    */

    property real upperValue

    /*! This property defines the colour of the slider */
    property alias barColor: slider.barColor

    //TODO: Review this, should it be internal? If so rename with "__" prefix
    /*! \internal */
    property alias slider: slider

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

    /*! This property defines the number of decimal places displayed in the textbox
        The default value is \c 1
    */
    property int decimals: 1

    /*! This property is used to define the buttons label when used in a WGFormLayout
        The default value is an empty string
    */
    //TODO: This should be renamed, it does not require "_"
    property string label_: ""

    /*! This property is used to define the width of the number box.
        The default value is the implicit width.
    */
    property int valueBoxWidth: sliderUpperValue.implicitWidth

    /*! This property determines if the number box will be visible or not.
        The default value is true.
    */
    property bool showValue: true

    /*! This property is used to leave an empty space in the case where you want a regular slider
        to line up with a range slider.
        The default value is false.
    */
    property bool fakeLowerValue: false

    implicitHeight: parent.rowHeight_ ? parent.rowHeight_ : 22

    property alias b_Target: dataBinding.target
    property alias b_Property: dataBinding.property
    property alias b_Value: dataBinding.value

    /*! This property holds the target control's id to be bound to this controls b_Value2 */
    property alias b_Target2: dataBinding2.target

    /*! This property determines b_Target's property which is to be bound to this controls b_Value2 */
    property alias b_Property2: dataBinding2.property

    /*! This property determines this control's value which will drive b_Target2's b_Property2 */
    property alias b_Value2: dataBinding2.value

    Binding {
        id: dataBinding
    }

    Binding {
        id: dataBinding2
    }

    onLowerValueChanged: {
        setValueHelper(slider, "value", sliderFrame.lowerValue);
    }

    onUpperValueChanged: {
        setValueHelper(slider, "value", sliderFrame.upperValue);
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
    }

    WGExpandingRowLayout {
        anchors.fill: parent


        WGNumberBox {
            id: sliderLowerValue

            Layout.preferredHeight: defaultSpacing.minimumRowHeight
            visible: showValue
            decimals: decimals
            Layout.preferredWidth: visible ? valueBoxWidth : 0

            prefix: sliderFrame.prefix
            suffix: sliderFrame.suffix

            value: sliderFrame.lowerValue

            minimumValue: slider.__handlePosList[0].minimumValue
            maximumValue: slider.__handlePosList[0].maximumValue

            stepSize: slider.stepSize

            onEditingFinished: {
                setValueHelper(sliderFrame, "value", value);
            }

            onValueChanged: {
                sliderFrame.lowerValue = value
            }


            Binding {
                target: sliderLowerValue
                property: "value"
                value: sliderFrame.lowerValue
            }
        }

        WGSlider {
            id: slider

            property bool showValue: true

            stepSize: 1.0

            activeFocusOnPress: true

            Layout.fillWidth: true

            Layout.preferredHeight: Math.round(sliderFrame.height)

            onPressedChanged:{
                if(!pressed && (value != oldValue))
                {
                    setValueHelper(sliderFrame, "value", value);
                }
            }

            WGSliderHandle {
                id: sliderMinHandle
                minimumValue: 0
                maximumValue: sliderMaxHandle.value
                showBar: false
                rangePartnerHandle: sliderMaxHandle
                value: sliderFrame.lowerValue
                maxHandle: false

                onValueChanged: {
                    sliderFrame.lowerValue = value
                }

                Binding {
                    target: sliderMinHandle
                    property: "value"
                    value: sliderFrame.lowerValue
                }
            }

            WGSliderHandle {
                id: sliderMaxHandle
                minimumValue: sliderMinHandle.value
                maximumValue: 100
                showBar: true
                barMinPos: sliderMinHandle.range.position
                rangePartnerHandle: sliderMinHandle
                value: sliderFrame.upperValue
                maxHandle: true

                onValueChanged: {
                    sliderFrame.upperValue = value
                }

                Binding {
                    target: sliderMaxHandle
                    property: "value"
                    value: sliderFrame.upperValue
                }
            }

            style : WGSliderStyle{

            }
        }

        WGNumberBox {
            id: sliderUpperValue

            Layout.preferredHeight: defaultSpacing.minimumRowHeight
            visible: showValue
            decimals: decimals
            Layout.preferredWidth: visible ? valueBoxWidth : 0

            prefix: sliderFrame.prefix
            suffix: sliderFrame.suffix

            value: sliderFrame.upperValue

            minimumValue: slider.__handlePosList[1].minimumValue

            maximumValue: slider.__handlePosList[1].maximumValue

            stepSize: slider.stepSize

            onEditingFinished: {
                setValueHelper(sliderFrame, "value", value);
            }

            onValueChanged: {
                sliderFrame.upperValue = value
            }


            Binding {
                target: sliderUpperValue
                property: "value"
                value: sliderFrame.upperValue
            }
        }
    }
}
