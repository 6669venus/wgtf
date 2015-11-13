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
    maximumValue: linkColorsToHandles ? 100 : 255

    stepSize: linkColorsToHandles ? 0.1 : 1

    property var colorData: ["#FF0000","#FFFF00","#00FF00","#0000FF"]

    property var positionData: linkColorsToHandles ? [20,40,60,80] : [0,100,150,255]

    property int handles: linkColorsToHandles ? positionData.length : 1

    value: linkColorsToHandles ? 0 : 150

    implicitHeight: defaultSpacing.minimumRowHeight

    style: WGColorSliderStyle{}

    property var colorBarModel: ListModel {}

    signal updateColorBars()

    property bool barLoaded: false

    onUpdateColorBars: {
        if (barLoaded)
        {
            if(linkColorsToHandles)
            {
                colorBarModel.set(0,{"minValue": minimumValue,
                                         "maxValue": positionData[0],
                                         "minColor": colorData[0],
                                         "maxColor": colorData[0]
                                     });
                colorBarModel.set(colorBarModel.count - 1,{"minValue": positionData[positionData.length - 1],
                                      "maxValue": maximumValue,
                                      "minColor": colorData[colorData.length - 1],
                                      "maxColor": colorData[colorData.length - 1]
                                     });
            }
            var startBar = linkColorsToHandles ? 1 : 0
            var endBar = colorBarModel.count - 1
            for (startBar; startBar < endBar; startBar++)
            {
                colorBarModel.set(startBar,{"minValue": positionData[startBar - 1],
                                         "maxValue": positionData[startBar],
                                         "minColor": colorData[startBar - 1],
                                         "maxColor": colorData[startBar]
                                     });
            }
        }
    }

    Component.onCompleted: {
        for (var i = 0; i < handles; i++)
        {
            var newHandle = Qt.createComponent("WGColorSliderHandle.qml");
            if (newHandle.status === Component.Ready)
            {
                var newObject = newHandle.createObject(__handlePosList, {
                                           "minimumValue": sliderFrame.minimumValue,
                                           "maximumValue": sliderFrame.maximumValue,
                                           "value": linkColorsToHandles ? positionData[i] : sliderFrame.value,
                                           "showBar": false
                                       });
            }
        }
        var ticker = 0
        if (linkColorsToHandles)
        {
            colorBarModel.append({"minValue": minimumValue,
                                     "maxValue": positionData[0],
                                     "minColor": colorData[0],
                                     "maxColor": colorData[0]
                                 });
            console.log("Creating " + ticker)
            ticker++
        }
        for (var j = 0; j < colorData.length - 1; j++)
        {
            colorBarModel.append({"minValue": positionData[j],
                                     "maxValue": positionData[j+1],
                                     "minColor": colorData[j],
                                     "maxColor": colorData[j+1]
                                 });
            console.log("Creating " + ticker)
            ticker++
        }
        if (linkColorsToHandles)
        {
            colorBarModel.append({"minValue": positionData[positionData.length - 1],
                                     "maxValue": maximumValue,
                                     "minColor": colorData[colorData.length - 1],
                                     "maxColor": colorData[colorData.length - 1]
                                 });
            console.log("Creating " + ticker)
        }
        barLoaded = true
    }
}
