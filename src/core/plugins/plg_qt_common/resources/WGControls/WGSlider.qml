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

/*!
    \qmltype Slider
    \inqmlmodule QtQuick.Controls
    \since 5.1
    \ingroup controls
    \brief Provides a vertical or horizontal slider control.

    \image slider.png

    The slider is the classic control for providing a bounded value. It lets
    the user move a slider handle along a horizontal or vertical groove
    and translates the handle's position into a value within the legal range.

    \code
    Slider {
        value: 0.5
    }
    \endcode

    The slider value is by default in the range [0, 1]. If integer values are
    needed, you can set the \l stepSize.

    You can create a custom appearance for a Slider by
    assigning a \l {QtQuick.Controls.Styles::SliderStyle}{SliderStyle}.
*/

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
    property bool updateValueWhileDragging: true

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
    //property bool tickmarksEnabled: false

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

    property int __activeHandle: 0

    //default property alias handles: __handlePosList

    property list <QtObject> __handlePosList

    activeFocusOnTab: true

    //Accessible.role: Accessible.Slider
    /*! \internal
    function accessibleIncreaseAction() {
        __handlePosList[__activeHandle].range.increaseSingleStep()
    }
    /*! \internal
    function accessibleDecreaseAction() {
        __handlePosList[__activeHandle].range.decreaseSingleStep()
    }*/

    style: Qt.createComponent("WGSliderStyle.qml", slider)

    /*
    Keys.onRightPressed: if (__horizontal) __handlePosList[__activeHandle].range.increaseSingleStep()
    Keys.onLeftPressed: if (__horizontal) __handlePosList[__activeHandle].range.decreaseSingleStep()
    Keys.onUpPressed: if (!__horizontal) __handlePosList[__activeHandle].range.increaseSingleStep()
    Keys.onDownPressed: if (!__horizontal) __handlePosList[__activeHandle].range.decreaseSingleStep()
    */

    MouseArea {
        id: mouseArea

        anchors.fill: parent
        hoverEnabled: Settings.hoverEnabled
        property int clickOffset: 0
        property real pressX: 0
        property real pressY: 0
        property bool handleHovered: false

        function clamp ( val ) {
            return Math.max(__handlePosList[__activeHandle].range.positionAtMinimum, Math.min(__handlePosList[__activeHandle].range.positionAtMaximum, val))
        }

        function updateHandlePosition(mouse, force) {
            var pos, overThreshold
            if (__horizontal) {
                pos = clamp (mouse.x + clickOffset - __handlePosList[__activeHandle].width/2)
                overThreshold = Math.abs(mouse.x - pressX) >= Settings.dragThreshold
                if (overThreshold)
                    preventStealing = true
                if (overThreshold || force)
                    __handlePosList[__activeHandle].range.value = pos
            } else if (!__horizontal) {
                pos = clamp (mouse.y + clickOffset - __handlePosList[__activeHandle].height/2)
                overThreshold = Math.abs(mouse.y - pressY) >= Settings.dragThreshold
                if (overThreshold)
                    preventStealing = true
                if (overThreshold || force)
                    __handlePosList[__activeHandle].range.value = pos
            }
        }

        onPositionChanged: {
            if (pressed)
                updateHandlePosition(mouse, preventStealing)

            var point = mouseArea.mapToItem(__handlePosList[__activeHandle], mouse.x, mouse.y)
        }

        onPressed: {
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
        }

        onExited: handleHovered = false
    }

    /*
    WheelArea {
        id: wheelarea
        anchors.fill: parent
        horizontalMinimumValue: slider.minimumValue
        horizontalMaximumValue: slider.maximumValue
        verticalMinimumValue: slider.minimumValue
        verticalMaximumValue: slider.maximumValue
        property real step: (slider.maximumValue - slider.minimumValue)/(range.positionAtMaximum - range.positionAtMinimum)

        onVerticalWheelMoved: {
            if (verticalDelta !== 0) {
                var delta = Math.abs(verticalDelta)*step > stepSize ? verticalDelta*step : verticalDelta/Math.abs(verticalDelta)*stepSize
                value += delta
            }
        }

        onHorizontalWheelMoved: {
            if (horizontalDelta !== 0) {
                var delta = Math.abs(horizontalDelta)*step > stepSize ? horizontalDelta*step : horizontalDelta/Math.abs(horizontalDelta)*stepSize
                value += delta
            }
        }
    }
    */
}
