/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
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
    \qmltype SliderStyle
    \inqmlmodule QtQuick.Controls.Styles
    \since 5.1
    \ingroup controlsstyling
    \brief Provides custom styling for Slider

    The slider style allows you to create a custom appearance for
    a \l Slider control.

    The implicit size of the slider is calculated based on the
    maximum implicit size of the \c background and \c handle
    delegates combined.

    Example:
    \qml
    Slider {
        anchors.centerIn: parent
        style: SliderStyle {
            groove: Rectangle {
                implicitWidth: 200
                implicitHeight: 8
                color: "gray"
                radius: 8
            }
            handle: Rectangle {
                anchors.centerIn: parent
                color: control.pressed ? "white" : "lightgray"
                border.color: "gray"
                border.width: 2
                implicitWidth: 34
                implicitHeight: 34
                radius: 12
            }
        }
    }
    \endqml
*/
Style {
    id: styleitem

    /*! The \l Slider this style is attached to. */
    readonly property WGSlider control: __control

    padding { top: 0 ; left: 0 ; right: 0 ; bottom: 0 }

    /*! This property holds the item for the slider handle.
        You can access the slider through the \c control property
    */
    property Component handle:

        WGButtonFrame {
            id: handleFrame
            height: 14
            width: 14
            color: control.__handlePosList[buttonid].handleColor
            borderColor: control.__activeHandle == buttonid && control.activeFocus ? palette.HighlightShade : palette.DarkerShade
            highlightColor: control.__hoveredHandle == buttonid ? palette.LighterShade : "transparent"

    }
    /*! This property holds the background groove of the slider.

        You can access the handle position through the \c styleData.handlePosition property.
    */
    property Component groove: Item {
        anchors.verticalCenter: parent.verticalCenter
        implicitWidth: Math.round(TextSingleton.implicitHeight * 4.5)
        implicitHeight: Math.max(6, Math.round(TextSingleton.implicitHeight * 0.3))
        Rectangle {
            radius: height/2
            anchors.fill: parent
            border.width: 1
            border.color: "#888"

            gradient: Gradient {
                GradientStop { color: "#bbb" ; position: 0 }
                GradientStop { color: "#ccc" ; position: 0.6 }
                GradientStop { color: "#ccc" ; position: 1 }
            }
        }
    }

    property Component bar: Item {
        property color fillColor: control.__handlePosList[barid].barColor
        clip: true
        Rectangle {
            anchors.fill: parent
            border.color: Qt.darker(fillColor, 1.2)
            radius: height/2
            gradient: Gradient {
                GradientStop {color: Qt.lighter(fillColor, 1.3)  ; position: 0}
                GradientStop {color: fillColor ; position: 1.4}
            }
        }
    }

    /*! This property holds the tick mark labels
        \since QtQuick.Controls.Styles 1.1

        You can access the handle width through the \c styleData.handleWidth property.
    */

    property Component tickmarks: Repeater {
        id: repeater
        model: control.stepSize > 0 ? 1 + (control.maximumValue - control.minimumValue) / control.stepSize : 0
        Rectangle {
            color: "#777"
            width: 1 ; height: 3
            y: repeater.height
            x: control.__handleWidth / 2 + index * ((repeater.width - control.__handleWidth) / (repeater.count-1))
        }
    }

    /*! This property holds the slider style panel.

        Note that it is generally not recommended to override this.
    */
    property Component panel: Item {
        id: root

        property bool horizontal : control.orientation === Qt.Horizontal
        property int horizontalSize: grooveLoader.implicitWidth + padding.left + padding.right
        property int verticalSize: grooveLoader.implicitHeight + padding.top + padding.bottom//Math.max(handleLoader.implicitHeight, grooveLoader.implicitHeight) + padding.top + padding.bottom

        implicitWidth: horizontal ? horizontalSize : verticalSize
        implicitHeight: horizontal ? verticalSize : horizontalSize

        y: horizontal ? 0 : height
        rotation: horizontal ? 0 : -90
        transformOrigin: Item.TopLeft

        Item {

            anchors.fill: parent

            Loader {
                id: grooveLoader
                x: padding.left
                sourceComponent: groove
                width: (horizontal ? parent.width : parent.height) - padding.left - padding.right
                y:  Math.round(padding.top + (Math.round(horizontal ? parent.height : parent.width - padding.top - padding.bottom) - grooveLoader.item.height)/2)

                Repeater {
                model: control.__handlePosList
                    Loader {
                        id: barLoader
                        sourceComponent: bar
                        property int barid: index
                        visible: control.__handlePosList[index].showBar
                        anchors.verticalCenter: grooveLoader.verticalCenter
                        height: grooveLoader.height
                        z: 1
                        x: control.__handlePosList[index].barMinPos
                        width: control.__handlePosList[index].x - control.__handlePosList[index].barMinPos
                    }
                }
            }

            Loader {
                id: tickMarkLoader
                anchors.fill: parent
                sourceComponent: control.tickmarksEnabled ? tickmarks : null
            }

            Repeater {
            model: control.__handlePosList
                Loader {
                    id: handleLoader
                    sourceComponent: handle
                    property int buttonid: index
                    anchors.verticalCenter: grooveLoader.verticalCenter
                    x: control.__handlePosList[index].x - (control.__handlePosList[index].width / 2)//Math.round((control.__handlePosList[index].__handlePos - control.__handlePosList[index].minimumValue) / (control.__handlePosList[index].maximumValue - control.__handlePosList[index].minimumValue) * ((horizontal ? root.width : root.height) - item.width))

                    onLoaded: {
                        control.__handlePosList[index].handleIndex = index
                        control.__handleHeight = handleLoader.height
                        control.__handleWidth = handleLoader.width
                    }
                }
            }
        }
    }
}
