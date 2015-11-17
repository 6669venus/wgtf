import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Private 1.0
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import BWControls 1.0

//TODO: Test orientation = vertical. Create vertical slider. Remove option here

/*!
 \brief Slider with a color gradient background.
 Purpose: Allow the user to select a color visually

 This slider has two main variations:

 A color value slider where the gradient background is visual only. This is intended to
 be used for RGB sliders, HSL sliders etc. where changing the handle position selects
 a color on that gradient. The gradient itself is not changed by the handles position,
 (but could be changed by other means.) It is generally intended that these sliders
 would only have one handle.

 The following example creates a grayscale or brightness slider with one handle.

Example:
\code{.js}
    WGColorSlider {
        Layout.fillWidth: true
        minimumValue: 0
        maximumValue: 255
        stepSize: 1
        colorData: ["#000000", "#FFFFFF"]
        positionData: [0, 255]
        value: 128
        linkColorsToHandles: false
    }
\endcode

The second variation is a gradient or ramp slider where moving the handles changes the gradient
positions and individual colors in the gradient can be edited by the user. It is intended that
this slider could have any number of handles.

The following example creates a gradient from red to yellow to white with three handles.

Example:
\code{.js}
    WGColorSlider {
        Layout.fillWidth: true
        minimumValue: 0
        maximumValue: 100
        stepSize: 0.1
        colorData: ["#FF0000", "#FFFF00", "#FFFFFF"]
        positionData: [25, 50, 75]
        linkColorsToHandles: true
    }
\endcode

TODO: Hook up to proper C++ data
TODO: Make multi handle slider with linkColorsToHandles: true work in vertical orientation
TODO: Make safer with bad data, colorData.length != posData.length, bad hex colors etc.

*/

WGSlider {
    id: sliderFrame
    objectName: "WGColorSlider"

    minimumValue: 0
    maximumValue: linkColorsToHandles ? 100 : 255

    stepSize: linkColorsToHandles ? 0.1 : 1

    /*!
        This property determines whether the handleValues will be based off the positionData or off
        a general value. It is the main property that determines whether the slider will behave like
        a standard color slider or a gradient/ramp slider.

        If true, the handles will change the gradient positions when they are moved.

        The default value is \c false
    */

    property bool linkColorsToHandles: false

    /*!
        This array contains the values for the colors in the gradient as set contained in colorData.concat:
        These values are in slider value units, not QML gradient values.

        If linkColorsToHandles: true, the first and last colors will extend to
        the edges of the slider. In this case do not add colors/positions that correspond to the slider
        minimumValue and maximumValue.
    */

    //TODO: Data should probably be in C++

    property var positionData: []

    /*!
        This array contains the gradient color values for the points in positionData.

        If linkColorsToHandles: true, the first and last colors will extend to
        the edges of the slider. In this case do not add colors/positions that correspond to the slider
        minimumValue and maximumValue.
    */

    //TODO: Data should probably be in C++

    property var colorData: []

    /*!
        This value determines the number of handles in the slider. The default value
        is based on linkColorsToHandles.

        If true it will create handles for each color/position value.

        If false it will create one handle.

        If you need more handles, set this value to 0 and create WGColorHandles manually as with WGSlider.

    */
    /*!
        This value determines whether double clicking a handle should display a color picker.

        This is intended mostly to be used with linkColorsToHandles = true.constructor
    */
    property bool useColorPicker: linkColorsToHandles

    /*!
        This value determines the number of handles generated for the slider

        It is not intended that this be changed unless you need multiple handles
        with linkColorsToHandles = false. If linkColorsToHandles = true,
        the control will create handles based off the color and position data.
    */
    property int handles: linkColorsToHandles ? positionData.length : 1

    /*!
        This value determines whether the colorPicker should show alpha values or not.
    */
    property bool useAlpha: false

    /*!
        This value determines whether the user can add and delete handles from the slider.
        This is intended to be used with \c linkColorsToHandles

        The default value is true if linkColorsToHandles is true
    */
    property bool addDeleteHandles: linkColorsToHandles

    implicitHeight: defaultSpacing.minimumRowHeight

    style: WGColorSliderStyle{}

    /*! \internal */
    property bool __barLoaded: false

    /*! \internal */
    property var __colorBarModel: ListModel {}

    grooveClickable: false

    property var colorPicker: ColorDialog {
        id: colorPicker
        title: "Please choose a color"
        showAlphaChannel: useAlpha

        property int currentColorIndex: -1

        onAccepted: {
            if(currentColorIndex >= 0)
            {
                colorData[currentColorIndex] = colorPicker.color.toString()
                currentColorIndex = -1
                updateColorBars()
            }
        }

        onRejected: {
            currentColorIndex = -1
        }
    }

    signal updateColorBars()

    /*!
        This signal is fired when a point is added to the data with addData()
    */
    signal pointAdded(int index)

    /*!
        This signal is fired when a point is removed from the data with deleteData()
    */
    signal pointRemoved(int index)

    function addData (index, pos, col)
    {
        //TODO: Data should be changed via C++
        __barLoaded = false
        positionData.splice(index,0,pos)
        colorData.splice(index,0,col)
        pointAdded(index)
    }

    function deleteData (index)
    {
        //TODO: Data should be changed via C++
        __barLoaded = false
        positionData.splice(index,1)
        colorData.splice(index,1)
        pointRemoved(index)
    }


    onPointAdded:
    {
        //Turn off updating values, create a new handle and update everything
        __barLoaded = false

        var newHandle = Qt.createComponent("WGColorSliderHandle.qml");

        if (newHandle.status === Component.Ready)
        {
            var newObject = newHandle.createObject(__handlePosList, {
                                        "showBar": false
                                   });
        }
        updateHandles()


        //easier to wipe the color bars and re-create them
        __colorBarModel.clear()

        createBars()

        __barLoaded = true
    }

    onPointRemoved:
    {
        //Turn off updating values, destroy the unneeded handle and update everything
        __barLoaded = false

        for (var i = 0; i < __handlePosList.children.length; i++)
        {
            if (__handlePosList.children[i].handleIndex == index)
            {
                var handleToDestroy = __handlePosList.children[i]
            }
        }

        // This parenting change is needed as the object seems to take
        // a while to destroy and the handles won't update properly
        handleToDestroy.parent = sliderFrame
        handleToDestroy.visible = false
        handleToDestroy.destroy()

        updateHandles()

        //easier to wipe the color bars and re-create them
        __colorBarModel.clear()

        createBars()

        __barLoaded = true
    }

    function updateHandles()
    {
        //update the value and index of the handles
        for (var i = 0; i < __handlePosList.children.length; i++)
        {
            __handlePosList.children[i].handleIndex = i
            __handlePosList.children[i].updateValueBinding()

        }

        //update the min max values to reflect the new order and values
        for (var j = 0; j < __handlePosList.children.length; j++)
        {
            __handlePosList.children[j].updateMinMaxBinding()
        }
    }

    function createBars()
    {
        //create the first bar if linkColorsToHandles
        if (linkColorsToHandles)
        {
            __colorBarModel.append({"minValue": minimumValue,
                                     "maxValue": positionData[0],
                                     "minColor": colorData[0],
                                     "maxColor": colorData[0]
                                 });
        }
        //create the middle bars if linkColorsToHandles
        //or create all the bars if !linkColorsToHandles
        for (var j = 0; j < colorData.length - 1; j++)
        {
            __colorBarModel.append({"minValue": positionData[j],
                                     "maxValue": positionData[j+1],
                                     "minColor": colorData[j],
                                     "maxColor": colorData[j+1]
                                 });
        }
        //create the last bar if linkColorsToHandles
        if (linkColorsToHandles)
        {
            __colorBarModel.append({"minValue": positionData[positionData.length - 1],
                                     "maxValue": maximumValue,
                                     "minColor": colorData[colorData.length - 1],
                                     "maxColor": colorData[colorData.length - 1]
                                 });
        }
    }

    //converts #AARRGGBB or #RRGGBB to (0-1,0-1,0-1,0-1)
    function hexToRgb(hex) {
        if (hex.length = 7)
        {
            var result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);

            var r = parseInt(result[1], 16) / 255
            var g = parseInt(result[2], 16) / 255
            var b = parseInt(result[3], 16) / 255

            return Qt.rgba(r,g,b,1)
        }
        else if (hex.length = 9)
        {
            var result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);

            var a = parseInt(result[0], 16) / 255
            var r = parseInt(result[1], 16) / 255
            var g = parseInt(result[2], 16) / 255
            var b = parseInt(result[3], 16) / 255

            return Qt.rgba(r,g,b,a)
        }
        else
        {
            console.log ("hexToRgb: Invalid hex color. Requires '#RRGGBB' or '#AARRGGBB'")
            return null
        }
    }

    //converts (0-1,0-1,0-1,0-1) to #AARRGGBB or #RRGGBB
    function rgbToHex(r,g,b,a)
    {
        r = Math.round(r * 255).toString(16)
        g = Math.round(g * 255).toString(16)
        b = Math.round(b * 255).toString(16)
        a = Math.round(a * 255).toString(16)

        if (a.length == 1) {a = "0" + a}
        if (r.length == 1) {r = "0" + r}
        if (g.length == 1) {g = "0" + g}
        if (b.length == 1) {b = "0" + b}

        if (a < 255)
        {
            return "#" + a + r + g + b
        }
        else
        {
            return "#" + r + g + b
        }
    }

    onUpdateColorBars: {
        if (__barLoaded)
        {
            if(linkColorsToHandles)
            {
                __colorBarModel.set(0,{"minValue": minimumValue,
                                         "maxValue": positionData[0],
                                         "minColor": colorData[0],
                                         "maxColor": colorData[0]
                                     });
            }

            var startBar = linkColorsToHandles ? 1 : 0
            var endBar = linkColorsToHandles ? __colorBarModel.count - 2 : __colorBarModel.count - 1

            for (startBar; startBar <= endBar; startBar++)
            {
                __colorBarModel.set(startBar,{"minValue": positionData[startBar - 1],
                                         "maxValue": positionData[startBar],
                                         "minColor": colorData[startBar - 1],
                                         "maxColor": colorData[startBar]
                                     });
            }

            if(linkColorsToHandles)
            {
                __colorBarModel.set(__colorBarModel.count - 1,{"minValue": positionData[positionData.length - 1],
                                      "maxValue": maximumValue,
                                      "minColor": colorData[colorData.length - 1],
                                      "maxColor": colorData[colorData.length - 1]
                                     });
            }
        }
    }

    //delete a handle
    onHandleCtrlClicked: {
        if (addDeleteHandles)
        {
            deleteData(index)
        }
    }

    //pick a color using ColorDialog
    onSliderDoubleClicked: {
        if (useColorPicker)
        {
            colorPicker.color = colorData[__activeHandle]
            colorPicker.currentColorIndex = __activeHandle
            colorPicker.open()
        }
    }

    Component.onCompleted: {
        //generate handles
        if(handles > 0)
        {
            for (var i = 0; i < handles; i++)
            {
                var newHandle = Qt.createComponent("WGColorSliderHandle.qml");
                if (newHandle.status === Component.Ready)
                {
                    var newObject = newHandle.createObject(__handlePosList, {
                                               "value": linkColorsToHandles ? positionData[i] : sliderFrame.value,
                                               "showBar": false
                                           });
                }
            }
        }

        //create colored bars
        createBars()

        __barLoaded = true
    }
}
