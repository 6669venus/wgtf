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

Item {
    id: sliderHandle

    width: parentSlider.__handleWidth
    height: parentSlider.__handleHeight

    property QtObject parentSlider: parent.parent

    anchors.verticalCenter: parent.verticalCenter

    property alias range: range

    /*!
        \qmlproperty bool Slider::pressed

        This property indicates whether the slider handle is being pressed.
    */
    //readonly property alias pressed: mouseArea.pressed

    /*!
        \qmlproperty bool Slider::hovered

        This property indicates whether the slider handle is being hovered.
    */
    //readonly property alias hovered: mouseArea.handleHovered

    property color handleColor: palette.LightPanelColor

    property color barColor: palette.HighlightColor

    property bool showBar: true

    property int barMinPos: 0

    property bool __horizontal: true

    /*!
        \qmlproperty real Slider::minimumValue

        This property holds the minimum value of the slider.
        The default value is \c{0.0}.
    */

    property real minimumValue: 0

    /*!
        \qmlproperty real Slider::maximumValue

        This property holds the maximum value of the slider.
        The default value is \c{1.0}.
    */
    property real maximumValue: 100

    /*!
        \qmlproperty real Slider::value

        This property holds the current value of the slider.
        The default value is \c{0.0}.
    */
    property real value: 0

    property int handleIndex: -1

    function updatePos() {
        if (parentSlider.updateValueWhileDragging)
        {
            sliderHandle.value = range.valueForPosition(__horizontal ? sliderHandle.x : sliderHandle.y, range.positionAtMinimum, range.positionAtMaximum)
            //range.position = __horizontal ? x : y
        }
    }

    onXChanged: updatePos();
    onYChanged: updatePos();

    /*! \internal
        The extra arguments positionAtMinimum and positionAtMaximum are there to force
        re-evaluation of the handle position when the constraints change (QTBUG-41255).
    */

    property real __handlePos: range.valueForPosition(__horizontal ? sliderHandle.x : sliderHandle.y, range.positionAtMinimum, range.positionAtMaximum)

    Connections {
        target: parentSlider
        onWidthChanged: {
            if(__horizontal)
            {
                sliderHandle.x = range.positionForValue(value)
            }
        }
        onHeightChanged: {
            if(!__horizontal)
            {
                sliderHandle.y = range.positionForValue(value)
            }
        }
    }

    //property bool handleHovered: false

    MouseArea {
        hoverEnabled: true
        anchors.verticalCenter: parent.verticalCenter
        width: parent.width
        height: parent.height
        x: -(parent.width / 2)
        propagateComposedEvents: true

        onEntered: {
           parentSlider.__hoveredHandle = handleIndex
        }

        onExited: {
            if (parentSlider.__hoveredHandle == handleIndex)
            {
               parentSlider.__hoveredHandle = -1
            }
        }

        onPressed: {
            parentSlider.__activeHandle = handleIndex
            mouse.accepted = false
        }
    }

    activeFocusOnTab: true

    RangeModel {
        id: range
        stepSize: parentSlider.stepSize
        inverted: __horizontal ? false : true
        value: sliderHandle.value
        minimumValue: sliderHandle.minimumValue
        maximumValue: sliderHandle.maximumValue

        property int handleMinOffset: sliderHandle.minimumValue == parentSlider.minimumValue ? sliderHandle.width / 2 : 0
        property int handleMaxOffset: sliderHandle.maximumValue == parentSlider.maximumValue ? sliderHandle.width / 2 : 0

        positionAtMinimum: Math.round(((sliderHandle.minimumValue) / (parentSlider.maximumValue - parentSlider.minimumValue) * parentSlider.width) + handleMinOffset)
        positionAtMaximum: Math.round(((sliderHandle.maximumValue) / (parentSlider.maximumValue - parentSlider.minimumValue) * parentSlider.width) - handleMaxOffset)
    }
    /*
    Rectangle {
        color: "#66FF0000"
        x: range.positionAtMinimum
        width: range.positionAtMaximum - range.positionAtMinimum
        height: parentSlider.height
        parent: parentSlider
    }
    Rectangle {
        anchors.verticalCenter: parent.verticalCenter
        height: 10
        width: 10
        color: "#6600FFFF"
        x: -5
    }
    */

    /*

    MouseArea {
        id: mouseArea

        anchors.fill: parent
        hoverEnabled: Settings.hoverEnabled
        property int clickOffset: 0
        property real pressX: 0
        property real pressY: 0
        property bool handleHovered: false

        function clamp ( val ) {
            return Math.max(range.positionAtMinimum, Math.min(range.positionAtMaximum, val))
        }

        function updateHandlePosition(mouse, force) {
            var pos, overThreshold
            if (__horizontal) {
                pos = clamp (mouse.x + clickOffset - fakeHandle.width/2)
                overThreshold = Math.abs(mouse.x - pressX) >= Settings.dragThreshold
                if (overThreshold)
                    preventStealing = true
                if (overThreshold || force)
                    fakeHandle.x = pos
            } else if (!__horizontal) {
                pos = clamp (mouse.y + clickOffset- fakeHandle.height/2)
                overThreshold = Math.abs(mouse.y - pressY) >= Settings.dragThreshold
                if (overThreshold)
                    preventStealing = true
                if (overThreshold || force)
                    fakeHandle.y = pos
            }
        }

        onPositionChanged: {
            if (pressed)
                updateHandlePosition(mouse, preventStealing)

            var point = mouseArea.mapToItem(fakeHandle, mouse.x, mouse.y)
            handleHovered = fakeHandle.contains(Qt.point(point.x, point.y))
        }

        onPressed: {
            if (parentSlider.activeFocusOnPress)
                parentSlider.forceActiveFocus();

            if (handleHovered) {
                parentSlider.__activeHandle = index
                var point = mouseArea.mapToItem(fakeHandle, mouse.x, mouse.y)
                clickOffset = __horizontal ? fakeHandle.width/2 - point.x : fakeHandle.height/2 - point.y
            }
            pressX = mouse.x
            pressY = mouse.y
            updateHandlePosition(mouse, !Settings.hasTouchScreen)
        }

        onReleased: {
            updateHandlePosition(mouse, Settings.hasTouchScreen)
            // If we don't update while dragging, this is the only
            // moment that the range is updated.
            if (!slider.updateValueWhileDragging)
                range.position = __horizontal ? fakeHandle.x : fakeHandle.y;
            clickOffset = 0
            preventStealing = false
        }

        onExited: handleHovered = false
    }


    // During the drag, we simply ignore the position set from the range, this
    // means that setting a value while dragging will not "interrupt" the
    // dragging activity.
    Binding {
        when: !mouseArea.drag.active
        target: fakeHandle
        property: __horizontal ? "x" : "y"
        value: range.position
    }
    */
}
