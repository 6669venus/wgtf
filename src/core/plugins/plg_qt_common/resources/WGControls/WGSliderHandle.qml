import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Private 1.0
import BWControls 1.0

/*!
 \A Slider handle intended for the WGSlider Control

Example:
\code{.js}
WGSlider {
    Layout.fillWidth: true
    minimumValue: 0
    maximumValue: 10
    stepSize: 1.0

    WGSliderHandle {
        minimumValue: 0
        maximumValue: 100
        value: 50
    }
}
\endcode
*/

Item {
    id: sliderHandle

    /*!
        The parent slider object
    */
    property QtObject parentSlider: parent.parent

    property alias range: range

    property color handleColor: palette.LightPanelColor

    property color barColor: parentSlider.barColor

    /*!
        This property determines whether the coloured bar will be shown or not.
        The default value is \c true
    */

    property bool showBar: true

    /*!
        A paired handle that handles the max value in a range slider.
    */

    property QtObject rangePartnerHandle: sliderHandle

    /*!
        True if the handle is the maximum value in a range slider
    */

    property bool maxHandle: false

    /*!
        Allows a slider bar to be attached to another handle for range sliders.
    */

    property int barMinPos: __horizontal ? 0 : range.position

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


    /*! \internal */
    property bool __horizontal: parentSlider.__horizontal

    /*! \internal */
    property int handleIndex: -1

    /*! \internal */
    property bool __overlapping: {
        if(rangePartnerHandle != sliderHandle)
        {
            if((sliderHandle.range.position >= rangePartnerHandle.range.position - (__horizontal ? parentSlider.__handleWidth/2 : parentSlider.__handleHeight/2)) && (sliderHandle.range.position <= rangePartnerHandle.range.position + (__horizontal ? parentSlider.__handleWidth/2 : parentSlider.__handleHeight/2)))
            {
                return true
            }
            else
            {
                return false
            }
        }
        else
        {
            return false
        }
    }

    function updatePos() {
        if (parentSlider.__handleMoving)
        {
            sliderHandle.value = range.valueForPosition(__horizontal ? sliderHandle.x : sliderHandle.y, range.positionAtMinimum, range.positionAtMaximum)
        }
    }

    width: parentSlider.__handleWidth

    height: parentSlider.__handleHeight

    anchors.verticalCenter: __horizontal ? parent.verticalCenter : undefined
    anchors.horizontalCenter: !__horizontal ? parent.horizontalCenter : undefined

    onXChanged: updatePos();
    onYChanged: updatePos();

    //Update handle position if the parent slider is changed in size.

    Connections {
        target: parentSlider
        onWidthChanged: {
            if(__horizontal)
            {
                sliderHandle.x = range.position
            }
        }
        onHeightChanged: {
            if(!__horizontal)
            {
                sliderHandle.y = range.position
            }
        }
    }

    activeFocusOnTab: true

    RangeModel {
        id: range
        stepSize: parentSlider.stepSize
        value: parentSlider.value
        minimumValue: 0
        maximumValue: 10

        inverted: __horizontal ? false : true

        onValueChanged: {
            if(__horizontal)
            {
                sliderHandle.x = range.positionForValue(value)
            }
            else
            {
                sliderHandle.y = range.positionForValue(value)
            }
        }

        property int sliderLength: __horizontal ? parentSlider.internalWidth : parentSlider.internalHeight

        //The handle offset makes the handles fit inside the bar at the edges instead of overlapping the outside.
        property int handleOffset: __horizontal ? parentSlider.__handleWidth / 2 : parentSlider.__handleHeight / 2

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
                sliderLength + handleOffset
            }
            else if (sliderHandle.maximumValue < parentSlider.maximumValue)
            {
                (sliderHandle.maximumValue * (sliderLength / (parentSlider.maximumValue - parentSlider.minimumValue))) + handleOffset
            }
        }
    }
}
