import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Private 1.0
import BWControls 1.0

/*!
 \A Slider handle intended for the WGOverlaySlider Control

Example:
\code{.js}
WGOverlaySlider {
    Layout.fillWidth: true
    minimumValue: 0
    maximumValue: 10
    stepSize: 1.0

    WGOverlaySliderHandle {
        minimumValue: 0
        maximumValue: 100
        value: 50
    }
}
\endcode
*/

WGSliderHandle {
    id: sliderHandle
    handleStyle: WGButtonFrame {
        implicitWidth: defaultSpacing.minimumRowHeight - defaultSpacing.rowSpacing * 2
        implicitHeight: defaultSpacing.minimumRowHeight - defaultSpacing.rowSpacing * 2
        color:  parentSlider.__hoveredHandle ? palette.OverlayLighterShade : palette.OverlayLightShade
        borderColor: palette.OverlayDarkerShade
        innerBorderColor: parentSlider.__activeHandle === handleIndex && parentSlider.activeFocus ? palette.HighlightShade : "transparent"
    }
}
