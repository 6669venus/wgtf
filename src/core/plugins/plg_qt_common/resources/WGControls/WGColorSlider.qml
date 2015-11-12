import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Private 1.0
import QtQuick.Layouts 1.1
import BWControls 1.0

//TODO: Test orientation = vertical. Create vertical slider. Remove option here

/*!
 \brief Slider with a color gradient background.
 Purpose: Allow the user to select a color visually

Example:
\code{.js}
\endcode
*/

WGSlider {
    id: sliderFrame
    objectName: "WGColorSlider"

    property bool linkColorsToHandles: true

    minimumValue: 0
    maximumValue: 255

    stepSize: linkColorsToHandles ? 0.1 : 1

    property var colorData: ["#FF0000","#FFFF00","#00FF00","#0000FF"]

    property var positionData: [0,100,150,255]

    property int handles: 3

    implicitHeight: defaultSpacing.minimumRowHeight

    style: WGColorSliderStyle{}

    Component.onCompleted: {
        for (var i = 0; i < handles; i++)
        {
            var newHandle = Qt.createComponent("WGSliderHandle.qml");
            if (newHandle.status === Component.Ready)
            {
                var newObject = newHandle.createObject(__handlePosList, {
                                           "minimumValue": sliderFrame.minimumValue,
                                           "maximumValue": sliderFrame.maximumValue,
                                           "value": positionData[i],
                                           "showBar": false
                                       });
            }
        }
    }
}
