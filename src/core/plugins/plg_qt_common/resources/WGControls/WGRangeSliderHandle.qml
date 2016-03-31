import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Private 1.0

/*!
 \A Slider handle intended for the WGRangeSlider Control. It has animation properties to handle overlapping it's partner handle.

 Shouldn't really be needed to be used on it's own but may be useful in some other kind of slider.
*/

WGSliderHandle {
    id: sliderHandle
    objectName: "SliderHandle"

    /*!
        A paired handle that handles the max or min value in a range slider.
    */
    property QtObject rangePartnerHandle: sliderHandle

    /*!
        True if the handle is the maximum value in a range slider
    */
    property bool maxHandle: false

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

        Behavior on height{
            enabled: __horizontal
            NumberAnimation {
                duration: 120
                easing {
                    type: Easing.OutCirc
                    amplitude: 1.0
                    period: 0.5
                }
            }
        }

        Behavior on width{
            enabled: !__horizontal
            NumberAnimation {
                duration: 120
                easing {
                    type: Easing.OutCirc
                    amplitude: 1.0
                    period: 0.5
                }
            }
        }

        anchors.top: {
            if(__horizontal)
            {
                !maxHandle ? parent.top : undefined
            }
            else
            {
                undefined
            }
        }
        anchors.bottom: {
            if(__horizontal)
            {
                maxHandle ? parent.bottom : undefined
            }
            else
            {
                undefined
            }
        }
        anchors.left: {
            if(!__horizontal)
            {
                !maxHandle ? parent.left : undefined
            }
            else
            {
                undefined
            }
        }
        anchors.right: {
            if(!__horizontal)
            {
                maxHandle ? parent.right : undefined
            }
            else
            {
                undefined
            }
        }

        height:{
            if(__horizontal)
            {
                __overlapping ? parent.height / 2 : parent.height
            }
            else
            {
                parent.implicitHeight
            }
        }

        width: {
            if(!__horizontal)
            {
                __overlapping ? parent.width / 2 : parent.width
            }
            else
            {
                parent.implicitWidth
            }
        }
    }

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
}

