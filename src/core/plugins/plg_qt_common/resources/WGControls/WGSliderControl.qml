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
    property real value

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

    /*! TODO: Make timeObjects work
    */
    property bool timeObject: false

    /*! This property defines the number of decimal places displayed in the textbox
        The default value is \c 1
    */
    property int decimals: 1

    /*! This property is used to define the buttons label when used in a WGFormLayout
        The default value is an empty string
    */
    //TODO: This should be renamed, it does not require "_"
    property string label_: ""

    /*! \internal */
    property int valueBoxWidth: sliderValue.implicitWidth

    /*! \internal */
    property bool showValue: true

    /*! \internal */
    property bool fakeLowerValue: false

    implicitHeight: parent.rowHeight_ ? parent.rowHeight_ : 22

    property alias b_Target: dataBinding.target
    property alias b_Property: dataBinding.property
    property alias b_Value: dataBinding.value

    Binding {
        id: dataBinding
    }

    onValueChanged: {
        setValueHelper(slider, "value", sliderFrame.value);
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


        Rectangle {
            id: fakeValue
            color: "transparent"
            Layout.preferredWidth: fakeLowerValue ? valueBoxWidth : 0
            Layout.preferredHeight: defaultSpacing.minimumRowHeight
            visible: fakeLowerValue ? true : false
        }


        WGSlider {
            id: slider

            property bool showValue: true

            stepSize: 1.0

            Layout.fillWidth: true

            activeFocusOnPress: true

            Layout.preferredHeight: Math.round(sliderFrame.height)

            WGSliderHandle {
                id: sliderHandle
                minimumValue: slider.minimumValue
                maximumValue: slider.maximumValue
                showBar: true

                value: sliderFrame.value

                onValueChanged: {
                    sliderFrame.value = value
                }

                Binding {
                    target: sliderHandle
                    property: "value"
                    value: sliderFrame.value
                }
            }

            style : WGSliderStyle{

            }
        }

        WGNumberBox {
            id: sliderValue

            Layout.preferredHeight: defaultSpacing.minimumRowHeight
            visible: showValue
            decimals: sliderFrame.decimals
            Layout.preferredWidth: visible ? valueBoxWidth : 0

            prefix: sliderFrame.prefix
            suffix: sliderFrame.suffix

            value: sliderFrame.value

            minimumValue: sliderFrame.minimumValue

            maximumValue: sliderFrame.maximumValue

            stepSize: slider.stepSize

            onEditingFinished: {
                setValueHelper(sliderFrame, "value", value);
            }

            onValueChanged: {
                sliderFrame.value = value
            }


            Binding {
                target: sliderValue
                property: "value"
                value: sliderFrame.value
            }
        }
    }
}
