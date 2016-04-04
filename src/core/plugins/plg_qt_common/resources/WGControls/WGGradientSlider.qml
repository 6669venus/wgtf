import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Private 1.0
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import WGControls 2.0

//TODO: Test orientation = vertical. Create vertical slider. Remove option here

/*!
 \brief A multi-handle Slider with a color gradient background.
 Purpose: Allow the user to edit a linear gradient.

WGGradientSlider is a gradient or ramp slider where moving the handles changes the gradient
positions and individual colors in the gradient can be edited by the user. It is intended that
this slider could have any number of handles.

Handles can be added with Shift+left click anywhere in the slider.
Handles can be deleted by Ctrl+left clicking on a handle
Handles colors can be changed by double clicking them.

There are multiple methods of creating a gradient slider. Both of the following example create a
gradient from red to yellow to white with three handles. The gradient slider needs some kind of
color data to work with and so will not create a default handle.

Example:
\code{.js}
    WGGradientSlider {
        Layout.fillWidth: true

        Component.onCompleted: {
            createColorHandle(.25,handleStyle,__handlePosList.length, "red")
            createColorHandle(.5,handleStyle,__handlePosList.length, "yellow")
            createColorHandle(.75,handleStyle,__handlePosList.length, "white")
        }
    }
\endcode

Example:
\code{.js}
    WGGradientSlider {
        Layout.fillWidth: true

        WGGradientSliderHandle {
            value: 0.25
            color: "red"
        }
        WGGradientSliderHandle {
            value: 0.5
            color: "yellow"
        }
        WGGradientSliderHandle {
            value: 0.75
            color: "white"
        }
    }
\endcode
*/

WGSlider {
    id: slider
    objectName: "WGColorSlider"

    /*!
        This value determines whether double clicking a handle should display a color picker.
    */
    property bool useColorPicker: true

    /*!
        This value determines whether the alpha value will appear when changing a color handle via a color picker.
    */
    property bool showAlphaChannel: true

    /*!
        This value determines whether the user can add and delete handles from the slider.
    */
    property bool addDeleteHandles: true

    /*!
        This value determines both the vertical offset of the handles AND the additional margin
        below the slider groove. This works well with handles that look like arrows to make them
        sit below the groove.

        The default value is 0.
    */
    property int handleVerticalOffset: 0

    minimumValue: 0

    maximumValue: 1

    stepSize: .001

    handleType: WGGradientSliderHandle{}

    implicitHeight: defaultSpacing.minimumRowHeight

    handleClamp: false

    grooveClickable: false

    createInitialHandle: false

    style: WGGradientSliderStyle{}

    property var colorPicker: ColorDialog {
        id: colorPicker
        title: "Please choose a color"
        showAlphaChannel: slider.showAlphaChannel

        property int currentColorIndex: -1

        onAccepted: {
            if(currentColorIndex >= 0)
            {
                colorModified(currentColorIndex, Qt.rgba(colorPicker.color.r,colorPicker.color.g,colorPicker.color.b,colorPicker.color.a))
                currentColorIndex = -1
                //updateColorBars()
            }
        }

        onRejected: {
            currentColorIndex = -1
        }
    }

    // the repeated column of gradient bars loaded in WGGradientSliderStyle
    property Component gradientBars: Item {
        id: gradientFrame
        /*
        Connections {
            target: slider
            onChangeValue: {
                canvas.requestPaint()
            }
        }

        Canvas {
            id: canvas
            property var ctx
            anchors.fill: parent

            onPaint: {
                ctx = canvas.getContext('2d');
                var grad = ctx.createLinearGradient(0,0,0,gradientFrame.height);
                var previousStop = 0
                var currentStop
                grad.addColorStop(0, __handlePosList[0].color);
                for (var i = 0; i < __handleCount; i++)
                {
                    //currentStop = ((__handlePosList[i].range.position) / __sliderLength) + (__visualMinPos / __sliderLength)
                    //currentStop = __handlePosList[i].value

                    currentStop = ((__handlePosList[i].value / maximumValue))


                    if(currentStop == previousStop)
                    {
                        currentStop += 0.000000001
                    }
                    /*
                    if(currentStop == previousStop)
                    {
                        if (currentStop <= 1.0)
                        {
                            currentStop += 0.001
                        }
                        else
                        {
                            currentStop = 1.0
                        }
                    }
                    */
                    /*
                    grad.addColorStop(currentStop, __handlePosList[i].color);
                    previousStop = currentStop;
                }
                grad.addColorStop(1.0, __handlePosList[__handlePosList.length - 1].color);
                ctx.fillStyle = grad;
                ctx.fillRect(0,0,gradientFrame.width,gradientFrame.height)
            }
        }
        */

        Item {
        id: gradientBars
        anchors.fill: parent
            Repeater {
                //adds an extra bar at the end that has no corresponding handle
                model: __handleCount + 1

                Rectangle {
                    id: colorBar

                    width: gradientBars.width
                    height: maxPos - minPos
                    y: minPos

                    gradient: {
                        if (slider.__handleCount > 0)
                        {
                            if (index == slider.__handleCount)
                            {
                                //if it's the last bar, just uses a solid color
                                colorBar.color = slider.__handlePosList[index - 1].color
                                return null
                            }
                            else
                            {
                                slider.__handlePosList[index].gradient
                            }
                        }
                    }

                    property real minPos: {
                        if (index == 0)
                        {
                            0
                        }
                        else
                        {
                            slider.__handlePosList[index - 1].range.position
                        }
                    }

                    property real maxPos: {
                        if (index == slider.__handleCount)
                        {
                            gradientBars.height
                        }
                        else
                        {
                            slider.__handlePosList[index].range.position
                        }
                    }

                    // Mouse area for adding new handles
                    MouseArea
                    {
                        anchors.fill: parent
                        propagateComposedEvents: true

                        // Workaround for crash during Shift+Click, this colorBar may no longer be associated with the parent
                        // Without this when the event is propagated an attempt to access the null window crashes the application
                        onPressAndHold: { mouse.accepted = true}

                        onPressed: {
                            //adds handles when bar is Shift Clicked
                            if ((mouse.button == Qt.LeftButton) && (mouse.modifiers & Qt.ShiftModifier) && slider.addDeleteHandles)
                            {
                                //get the position of the mouse inside the entire slider
                                var barPoint = mouseY / colorBar.height
                                if (index == slider.__handleCount)
                                {
                                    var newColor = slider.__handlePosList[index - 1].color
                                }
                                else
                                {
                                    var newColor = slider.__handlePosList[index].getInternalColor(barPoint)
                                }
                                var mousePos = mapToItem(gradientBars, mouseX, mouseY)
                                var newPos = mousePos.y / (gradientBars.height / (slider.maximumValue - slider.minimumValue))

                                slider.createColorHandle(newPos, slider.handleType, index, newColor)
                            }
                            else if ((mouse.button == Qt.LeftButton) && (mouse.modifiers & Qt.sliderModifier) && slider.addDeleteHandles)
                            {
                                mouse.accepted = false
                            }
                            else
                            {
                                mouse.accepted = false
                            }
                        }
                        onReleased: {
                            slider.__draggable = true
                        }
                    }
                }
            }
        }
    }

    /*!
        This signal is fired when a point's color is changed via the color picker

    */
    signal colorModified(int index, color col)

    /*!
        creates a new color handle with value (val), handleType (handle), color (col) and gradient (grad)

        Only a value is actually needed to create a handle. handleType will default to the slider handleType.
        Color will default to the default color in the handle.
        The gradient can usually be ignored unless you wish to create a non-linear gradient
    */
    function createColorHandle(val, handle, index, col, grad)
    {
        if (typeof handle !== "undefined")
        {
            var newHandle = handle.createObject(slider, {
                                        "value": val,
                                        "parentSlider": slider
                                    });
        }
        else
        {
            var newHandle = slider.handleType.createObject(slider, {
                                        "value": val,
                                        "parentSlider": slider
                                    });
        }

        if (typeof index != "undefined")
        {
            addHandle(newHandle, index)
        }
        else
        {
            addHandle(newHandle, __handlePosList.length)
        }

        if (typeof col != "undefined")
        {
            newHandle.color = col
        }

        if (typeof grad != "undefined")
        {
            newHandle.gradient = grad
        }

        updateHandles()

        return newHandle
    }

    /*!
        This can be used to assign a new gradient to a WGGradientSliderHandle.__horizontal which can be useful
        if you need to apply a non-linear gradient.

        TODO: Fix odd color bars be created with non-linear gradients and Shift+Click
    */
    function updateGradient(index, grad)
    {
        if (typeof grad != "undefined")
        {
            __handlePosList[index].gradient = grad
        }
        else
        {
            __handlePosList[index].gradient.stops[0].color = __handlePosList[index].minColor
            __handlePosList[index].gradient.stops[1].color = __handlePosList[index].color
        }
    }

    /*!
        This updates the color, mininmum and maximum values for the slider handles
    */
    function updateHandles()
    {
        for (var i = 0; i < __handlePosList.length; i++)
        {
            var iHandle = __handlePosList[i]
            if (iHandle.objectName = "SliderHandle")
            {

                if (i === 0)
                {
                    iHandle.minimumValue = Qt.binding(function() {return slider.minimumValue})
                    iHandle.minColor = iHandle.color
                }
                else
                {
                    iHandle.minimumValue = Qt.binding(function() {return slider.__handlePosList[i - 1].value})
                    iHandle.minColor = slider.__handlePosList[i - 1].color
                }
                if (i === slider.__handlePosList.length - 1)
                {
                    iHandle.maximumValue = Qt.binding(function() {return slider.maximumValue})
                }
                else
                {
                    iHandle.maximumValue = Qt.binding(function() {return slider.__handlePosList[i + 1].value})
                }
            }
            else
            {
                i--
                console.log("WARNING WGGradientSlider: Child object found in __handlePosList that isn't a valid slider handle")
            }
        }
    }

    onColorModified: {
        __handlePosList[index].color = col
        updateHandles()
    }

    onChangeValue: {
        updateHandles()
    }


    //delete a handle
    onHandleCtrlClicked: {
        if (addDeleteHandles)
        {
            removeHandle(index)
            updateHandles()
        }
    }

    //pick a color using ColorDialog
    onSliderDoubleClicked: {
        if (useColorPicker)
        {
            colorPicker.color = __handlePosList[index].color
            colorPicker.currentColorIndex = index
            colorPicker.open()
        }
    }

    Component.onCompleted: {
        updateHandles()
    }
}
