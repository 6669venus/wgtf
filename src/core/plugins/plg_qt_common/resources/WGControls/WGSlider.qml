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
import BWControls 1.0


Control {
    id: slider

    /*!
        \qmlproperty enumeration Slider::orientation

        This property holds the layout orientation of the slider.
        The default value is \c Qt.Horizontal.
    */
    property int orientation: Qt.Horizontal

    /*!
        \qmlproperty bool Slider::updateValueWhileDragging

        This property indicates whether the current \l value should be updated while
        the user is moving the slider handle, or only when the button has been released.
        This property could for instance be modified if changing the slider value would turn
        out to be too time consuming.

        The default value is \c true.
    */
    property bool handleMoving: false

    /*!
        \qmlproperty bool Slider::activeFocusOnPress

        This property indicates whether the slider should receive active focus when
        pressed.
    */
    property bool activeFocusOnPress: false

    /*!
        \qmlproperty bool Slider::tickmarksEnabled

        This property indicates whether the slider should display tickmarks
        at step intervals. Tick mark spacing is calculated based on the
        \l stepSize property.

        The default value is \c false.

        \note This property may be ignored on some platforms when using the native style (e.g. Android).
    */
    property bool tickmarksEnabled: false


    /*!
        \qmlproperty real Slider::minimumValue

        This property holds the minimum value of the slider.
        The default value is \c{0.0}.
    */


    property real minimumValue: 0
    /*!
        \qmlproperty real Slider::minimumValue

        This property holds the minimum value of the slider.
        The default value is \c{0.0}.
    */

    property real maximumValue: 100

    property real value: 0

    property color barColor: palette.HighlightColor

    /*!
        \qmlproperty real Slider::stepSize

        This property indicates the slider step size.

        A value of 0 indicates that the value of the slider operates in a
        continuous range between \l minimumValue and \l maximumValue.

        Any non 0 value indicates a discrete stepSize. The following example
        will generate a slider with integer values in the range [0-5].

        \qml
        Slider {
            maximumValue: 5.0
            stepSize: 1.0
        }
        \endqml

        The default value is \c{0.0}.
    */
    property int stepSize: 1

    /*! \internal */
    property bool __horizontal: orientation === Qt.Horizontal

    /*!
        This property indicates the last clicked handle. Never changes in sliders with only one handle.
    */

    property int __activeHandle: 0

    /*!
        This property indicates which handle is mouseovered or none (-1).
    */

    property int __hoveredHandle: -1

    /*!
        Property stores the height of the handle from WGSliderStyle
    */
    property int __handleHeight

    /*!
        Property stores the width of the handle from WGSliderStyle
    */

    property int __handleWidth

    property alias pressed: mouseArea.pressed

    default property alias __handlePosList: __handlePosList.children

    Item {
        id: __handlePosList
        anchors.fill: parent
    }

    activeFocusOnTab: true

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

    Keys.onRightPressed: if (__horizontal) __handlePosList.children[__activeHandle].range.increaseSingleStep()
    Keys.onLeftPressed: if (__horizontal) __handlePosList.children[__activeHandle].range.decreaseSingleStep()
    Keys.onUpPressed: if (!__horizontal) __handlePosList.children[__activeHandle].range.increaseSingleStep()
    Keys.onDownPressed: if (!__horizontal) __handlePosList.children[__activeHandle].range.decreaseSingleStep()

    property int internalWidth: width - __handleWidth
    property int internalHeight: height - __handleHeight

    x: __horizontal ? __handleWidth / 2 : 0

    y: !__horizontal ? __handleHeight / 2 : 0

    MouseArea {
        id: mouseArea

        z:-1

        anchors.centerIn: parent
        height: __horizontal ? __handleHeight : parent.height
        width: !__horizontal ? __handleWidth : parent.width

        hoverEnabled: Settings.hoverEnabled
        property int clickOffset: 0
        property real pressX: 0
        property real pressY: 0
        property bool handleHovered: false

        function clamp ( val ) {
            return Math.max(__handlePosList.children[__activeHandle].range.positionAtMinimum, Math.min(__handlePosList.children[__activeHandle].range.positionAtMaximum, val))
        }

        function updateHandlePosition(mouse, force) {
            var pos, overThreshold
            if (__horizontal) {
                pos = clamp (mouse.x + clickOffset)
                overThreshold = Math.abs(mouse.x - pressX) >= Settings.dragThreshold
                if (overThreshold)
                    preventStealing = true
                if (overThreshold || force)
                    __handlePosList.children[__activeHandle].x = pos
            } else if (!__horizontal) {
                pos = clamp (mouse.y + clickOffset)
                overThreshold = Math.abs(mouse.y - pressY) >= Settings.dragThreshold
                if (overThreshold)
                    preventStealing = true
                if (overThreshold || force)
                    __handlePosList.children[__activeHandle].y = pos
            }
        }

        onPositionChanged: {
            if (pressed)
                updateHandlePosition(mouse, preventStealing)

            var point = mouseArea.mapToItem(__handlePosList.children[__activeHandle], mouse.x, mouse.y)
        }

        onPressed: {
            handleMoving = true
            if (slider.activeFocusOnPress)
                slider.forceActiveFocus();

            pressX = mouse.x
            pressY = mouse.y
            updateHandlePosition(mouse, !Settings.hasTouchScreen)
        }

        onReleased: {
            updateHandlePosition(mouse, Settings.hasTouchScreen)
            // If we don't update while dragging, this is the only
            // moment that the range is updated.
            clickOffset = 0
            preventStealing = false

            handleMoving = false
        }

        onWheel: {
            if (wheel.angleDelta.y > 0)
            {
                __horizontal ? __handlePosList.children[__activeHandle].range.increaseSingleStep() : __handlePosList.children[__activeHandle].range.decreaseSingleStep()
            }
            else
            {
                !__horizontal ? __handlePosList.children[__activeHandle].range.increaseSingleStep() : __handlePosList.children[__activeHandle].range.decreaseSingleStep()
            }
        }

        onExited: handleHovered = false
    }
}
