import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Private 1.0

/*!
 \A Slider handle intended for the WGSlider Control

Example:
\code{.js}
WGSlider {
    Layout.fillWidth: true
    minimumValue: 0
    maximumValue: 100
    stepSize: 1.0

    WGSliderHandle {
        value: 50

        handleStyle: Rectangle {
            color: "grey"
            implicitHeight: 20
            implicitWidth: 20
        }
    }
}
\endcode
*/

Item {
    id: sliderHandle
    objectName: "SliderHandle"

    /*!
        The parent slider object
    */
    property QtObject parentSlider

    property alias range: range

    /*!
        The internal color of the handle
    */
    property color handleColor: palette.lightPanelColor

    /*!
        The color of the bar attached to the handle
    */
    property color barColor: parentSlider.barColor

    /*!
        Allows a slider bar's minimum point to be attached to another position (usually another handle for range sliders).
    */
    property int barMinPos: __horizontal ? 0 : range.position

    /*!
        This property determines whether the coloured bar will be shown or not.
        The default value is \c true
    */

    property bool showBar: true

    /*!
        This property holds the minimum value of the handle.
        The default value is \c{0.0}.
    */

    property alias minimumValue: range.minimumValue

    /*!
        This property holds the maximum value of the handle.
        The default value is \c{1.0}.
    */
    property alias maximumValue: range.maximumValue

    /*!
        This property holds the current value of the handle.
        The default value is \c{0.0}.
    */
    property alias value: range.value

    /*!
        This is the Component for the handle style.

        This can be any Item based component.
    */
    property Component handleStyle: WGButtonFrame{
        color: parentSlider.enabled ? handleColor : palette.mainWindowColor
        borderColor: parentSlider.enabled ? palette.darkerShade : palette.darkShade
        highlightColor: parentSlider.__hoveredHandle === handleIndex ? palette.lighterShade : "transparent"
        innerBorderColor: parentSlider.__activeHandle === handleIndex && parentSlider.activeFocus ? palette.highlightShade : "transparent"
        implicitWidth: defaultSpacing.minimumRowHeight - defaultSpacing.rowSpacing * 2
        implicitHeight: defaultSpacing.minimumRowHeight - defaultSpacing.rowSpacing * 2
    }

    /*! \internal */
    property bool __horizontal: parentSlider.__horizontal

    function updatePos() {
        if (parentSlider.__handleMoving)
        {
            var newValue = range.valueForPosition(__horizontal ? sliderHandle.x : sliderHandle.y, range.positionAtMinimum, range.positionAtMaximum)
            setValueHelper(sliderHandle, "value", newValue);
        }
    }

    activeFocusOnTab: true

    RangeModel {
        objectName: "SliderRange"
        id: range
        stepSize: parentSlider.stepSize
        value: parentSlider.value

        minimumValue: parentSlider.minimumValue
        maximumValue: parentSlider.maximumValue

        inverted: __horizontal ? false : true

        property int sliderLength: __horizontal ? parentSlider.internalWidth : parentSlider.internalHeight

        //The handle offset makes the handles fit inside the bar at the edges instead of overlapping the outside.
        property int handleOffset: {
            if(parentSlider.handleClamp)
            {
                 __horizontal ? parentSlider.__handleWidth / 2 : parentSlider.__handleHeight / 2
            }
            else
            {
                0
            }
        }

        positionAtMinimum:  {
            if(sliderHandle.minimumValue == parentSlider.minimumValue)
            {
                0 + handleOffset
            }
            else if (sliderHandle.minimumValue > parentSlider.minimumValue)
            {
                (sliderHandle.minimumValue * (sliderLength / (parentSlider.maximumValue - parentSlider.minimumValue))) + handleOffset
            }
        }

        positionAtMaximum: {
            if(sliderHandle.maximumValue == parentSlider.maximumValue)
            {
                sliderLength + handleOffset - defaultSpacing.standardBorderSize
            }
            else if (sliderHandle.maximumValue < parentSlider.maximumValue)
            {
                (sliderHandle.maximumValue * (sliderLength / (parentSlider.maximumValue - parentSlider.minimumValue))) + handleOffset
            }
        }
    }
}

