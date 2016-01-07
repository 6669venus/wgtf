import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Private 1.0
import BWControls 1.0

/*!
 \A Slider handle intended for the WGColorSlider Control

 Generated dynamically by WGColorSlider. Don't use separately.
*/

WGSliderHandle {
    id: sliderHandle

    minimumValue: handleIndex == 0 ? parentSlider.minimumValue : parentSlider.__handlePosList.children[handleIndex - 1].value

    maximumValue: {
        if (parentSlider.linkColorsToHandles)
        {
            if (handleIndex == parentSlider.positionData.length - 1)
            {
                parentSlider.maximumValue
            }
            else
            {
                parentSlider.__handlePosList.children[handleIndex + 1].value
            }

        }
        else
        {
            parentSlider.maximumValue
        }
    }

    onValueChanged: {
        if (parentSlider.linkColorsToHandles && parentSlider.__barLoaded)
        {
            parentSlider.positionData[handleIndex] = value
            parentSlider.updateColorBars()
        }
        if (!parentSlider.linkColorsToHandles && parentSlider.__barLoaded)
        {
            if (handleIndex == 0)
            {
                parentSlider.value = sliderHandle.value
            }
        }
    }

    function updateValueBinding()
    {
        minimumValue = parentSlider.minimumValue
        maximumValue = parentSlider.maximumValue

        value = Qt.binding(function() {return parentSlider.positionData[handleIndex]})
    }

    function updateMinMaxBinding()
    {
        if (handleIndex == 0)
        {
            minimumValue = Qt.binding(function() {return parentSlider.minimumValue})
        }
        else
        {
            minimumValue = Qt.binding(function() {return parentSlider.__handlePosList.children[handleIndex - 1].value})
        }

        if (handleIndex == parentSlider.positionData.length - 1)
        {
            maximumValue = Qt.binding(function() {return parentSlider.maximumValue})
        }
        else
        {
            maximumValue = Qt.binding(function() {return parentSlider.__handlePosList.children[handleIndex + 1].value})
        }
    }
}
