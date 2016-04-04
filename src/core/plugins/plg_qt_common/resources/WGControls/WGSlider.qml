/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Quick Controls module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Private 1.0
import WGControls 2.0

/*!
 \brief A rewrite of the default QML Slider control that allows 'x'
 handles and can be setup as a range slider easily.

 Generally if you want to use this slider in an NGT context you
 would use WGSliderControl or WGRangeSlider which includes the required
 number boxes.

 The WGSliderHandle range will default to the WGSlider range unless explicitely
 assigned


Example:
\code{.js}
WGSlider {
    Layout.fillWidth: true
    minimumValue: 0
    maximumValue: 100
    stepSize: 1.0

    WGSliderHandle {
        minimumValue: 20
        maximumValue: 80
        value: 50
    }
}
\endcode
*/

Control {
    objectName: "WGSlider"
    id: slider

    /*!
        This property holds the layout orientation of the slider.
        The default value is \c Qt.Horizontal.

        TODO: Make range sliders work if orientation: Qt.Vertical
    */
    property int orientation: Qt.Horizontal

    /*!
        This property indicates whether the slider should receive active focus when
        pressed.
    */
    property bool activeFocusOnPress: false

    /*!
        This property indicates whether the slider should display tickmarks
        at step intervals. Tick mark spacing is calculated based on the
        \l stepSize property.

        The default value is \c false.
    */
    property bool tickmarksEnabled: false

    /*!
        This property holds the minimum value of the slider.
        The default value is \c{0.0}.

        This is not always the same as the minimumValue of the slider handle.
    */

    property real minimumValue: 0
    /*!
        This property holds the maximum value of the slider.
        The default value is \c{100.0}.

        This is not always the same as the maximumValue of the slider handle.
    */

    property real maximumValue: 100

    /*!
        This is a helper property that sets the value of the slider IF there is only one handle
        and it is NOT otherwise set itself.
        The default value is \c{0.0}.
    */

    property real value: 0

    /*!
        This property determines if the handles should be constrained within the length of the slider.

        This is useful to make sure the handles don't move outside the control boundaries but means the control values
        don't exactly follow the control height/width in a linear fashion. (the value is always accurate)

        The default value is \ctrue
    */

    property bool handleClamp: true

    /*!
        This property determines the default color of the bar that 'fills' the slider if is enabled

        The default value is \c palette.highlightColor
    */

    property color barColor: palette.highlightColor

    /*! This property indicates the slider step size.

        A value of 0 indicates that the value of the slider operates in a
        continuous range between \l minimumValue and \l maximumValue.

        Any non 0 value indicates a discrete stepSize. The following example
        will generate a slider with integer values in the range [0-5].

        \code{.js}
        Slider {
            maximumValue: 5.0
            stepSize: 1.0
        }
        \endcode

        The default value is \c{0.0}.
    */
    property real stepSize: 1

    /*!
        This property determines if the user can click the bar itself to
        move the handle to that position. Setting this to false gives better control
        and makes it harder to move the handles by accident.

        The default value is \c true
    */
    property bool grooveClickable: true

    /*!
        This property determines if the slider will create a handle if none are initialised.

        The default value is \c true
    */
    property bool createInitialHandle: true

    /*!
        This string determines the the component for the slider handle.

        The default value is WGColorSliderHandle.
        It can be set to any Item based component.
    */
    property Component handleType: WGSliderHandle{}

    /*! \internal */
    property bool __draggable: true

    /*! \internal */
    property bool __horizontal: orientation === Qt.Horizontal

    /*! \internal */
    property int __activeHandle: 0

    /*! \internal */
    property int __hoveredHandle: -1

    /*! \internal */
    property int __handleHeight: defaultSpacing.minimumRowHeight

    /*! \internal */
    property int __handleWidth: defaultSpacing.minimumRowHeight

    property alias pressed: mouseArea.pressed

    /*!
        This property holds the handle objects.
    */
    property var __handlePosList: []

    /*!
        This property holds the number of handle objects.
    */
    property int __handleCount: 0

    activeFocusOnTab: true

    /*! \internal */
    property bool __handleMoving: false

    /*! \internal */
    property real __grabbedValue: 0

    /*! \internal */
    property real __sliderLength: mouseArea.width

    /*! \internal */
    property real __clampedLength: __visualMaxPos - __visualMinPos

    /*! \internal */
    property real __visualMinPos: handleClamp ? __handleWidth / 2 : 0

    /*! \internal */
    property real __visualMaxPos: handleClamp ? __sliderLength - __handleWidth / 2 : __sliderLength

    //Accessible.role: Accessible.Slider
    /*! \internal */
    function accessibleIncreaseAction() {
        __handlePosList.children[__activeHandle].range.increaseSingleStep()
    }
    /*! \internal */
    function accessibleDecreaseAction() {
        __handlePosList.children[__activeHandle].range.decreaseSingleStep()
    }

    style: Qt.createComponent("WGSliderStyle.qml", slider)

    Keys.onRightPressed: {
        if (__horizontal) __handlePosList[__activeHandle].range.increaseSingleStep()
    }

    Keys.onLeftPressed: {
        if (__horizontal) __handlePosList[__activeHandle].range.decreaseSingleStep()
    }

    Keys.onUpPressed: {
        __handlePosList[__activeHandle].range.increaseSingleStep()
    }

    Keys.onDownPressed: {
        __handlePosList[__activeHandle].range.decreaseSingleStep()
    }

    /*!
        This signal is fired when the bar is double clicked
    */
    signal sliderDoubleClicked(int index)

    /*!
        This signal is fired when a handle (index) is left pressed when holding the Ctrl key
    */
    signal handleCtrlClicked(int index)

    /*!
        This signal is fired when a handle (index) is added
    */
    signal handleAdded(int index)

    /*!
        This signal is fired when a handle (index) is removed
    */
    signal handleRemoved(int index)

    /*!
        This signal is fired when a handle (index) value (val) is changed
    */
    signal changeValue (real val, int index)

    implicitHeight: defaultSpacing.minimumRowHeight
    implicitWidth: defaultSpacing.standardMargin

    /*!
        This function creates a new WGSliderHandle object (handle) with value (val) at (index)
        and automatically adds it to __handlePosList.

        Only the value is actually required.
    */
    function createHandle(val, handle, index)
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
        return newHandle
    }

    /*!
        Removes the handle (index) from __handlePosList and destroys it.
    */
    function removeHandle(index) {
        var handleToRemove = __handlePosList[index]
        if(__handleCount > 0)
        {
            __handlePosList.splice(index,1)
            __handleCount = __handlePosList.length
            handleRemoved(index)
        }
        handleToRemove.destroy()
    }

    /*! Internal */
    function addHandle(handle, index) {
        handle.parentSlider = slider
        __handlePosList.splice(index,0,handle)
        __handleCount = __handlePosList.length
        handleAdded(index)
    }

    function setHandleValue(val, index) {
        if (typeof index == "undefined")
        {
            index = 0
        }
        if(index <= __handleCount - 1)
        {
            if (val >= __handlePosList[index].minimumValue && val <= __handlePosList[index].maximumValue)
            {
                __handlePosList[index].value = val
                changeValue (val, index)
                return 1
            }
            else
            {
                console.log("WARNING WGSlider: Tried to set the value of a handle outside of minimum and maxium value")
                return -1
            }
        }
        else
        {
            console.log("WARNING WGSlider: Tried to change the value of a handle that does not exist")
            return -1
        }
    }

    Component.onCompleted: {

        //collect any child handles and put them in __handlePosList
        for (var i = 0; i < slider.children.length; i++)
        {
            if (slider.children[i].objectName == "SliderHandle")
            {
                addHandle(slider.children[i],__handlePosList.length)
            }
        }

        //create a handle if none were collected
        if(__handlePosList.length == 0 && createInitialHandle)
        {
            var newHandle = createHandle(slider.value)
            slider.value = Qt.binding(function() {return newHandle.value})
        }
    }

    MouseArea {
        id: mouseArea
        objectName: "sliderDragArea"

        z:-1

        anchors.centerIn: parent

        height: __horizontal ? parent.height : parent.width
        width: __horizontal ? parent.width : parent.height

        rotation: __horizontal ? 0 : -90
        transformOrigin: Item.Center

        hoverEnabled: true

        property int clickOffset: 0
        property real pressX: 0
        property real pressY: 0

        property bool dragStarted: false

        function clamp ( val ) {
            return Math.max(__handlePosList[__activeHandle].range.positionAtMinimum, Math.min(__handlePosList[__activeHandle].range.positionAtMaximum, val))
        }

        function updateHandlePosition(mouse, force) {

            if (__draggable)
                {
                var pos, overThreshold

                pos = clamp (mouse.x + clickOffset)
                overThreshold = Math.abs(mouse.x - pressX) >= Settings.dragThreshold
                if (overThreshold)
                    preventStealing = true
                if (overThreshold || force)
                    __handlePosList[__activeHandle].range.position = pos
            }
        }

        onPositionChanged: {
            if (pressed)
                updateHandlePosition(mouse, preventStealing)
        }

        onPressed: {
            if (__draggable)
            {
                beginUndoFrame();
                __handleMoving = true
                __grabbedValue = value;
                if (slider.activeFocusOnPress)
                    slider.forceActiveFocus();

                if(!grooveClickable)
                {
                    if(dragStarted) {
                        pressX = mouse.x
                        pressY = mouse.y

                        updateHandlePosition(mouse, !Settings.hasTouchScreen)
                    }
                }
                else
                {
                    pressX = mouse.x
                    pressY = mouse.y

                    updateHandlePosition(mouse, !Settings.hasTouchScreen)
                }
            }
        }

        onReleased: {

            if (!grooveClickable)
            {
                if(dragStarted) {
                    updateHandlePosition(mouse, Settings.hasTouchScreen)
                    // If we don't update while dragging, this is the only
                    // moment that the range is updated.
                }
            }
            else
            {
                updateHandlePosition(mouse, Settings.hasTouchScreen)
            }

            endUndoFrame();

            clickOffset = 0
            preventStealing = false

            __handleMoving = false

            dragStarted = false

            __draggable = true
        }

        //signal when bar is double clicked.
        //can be used for double clicking a handle, but if this is in the handle object, dragging won't work.
        onDoubleClicked: {
            sliderDoubleClicked(__activeHandle)
        }

        onWheel: {
            if(slider.activeFocus){
                if (wheel.angleDelta.y > 0)
                {
                    __handlePosList[__activeHandle].range.increaseSingleStep()
                }
                else
                {
                    __handlePosList[__activeHandle].range.decreaseSingleStep()
                }
            }
        }
    }

    /* Deprecated */
    property alias handleStyle: slider.handleType;
}
