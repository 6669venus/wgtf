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

    padding { top: 0 ; left: 1 ; right: 1 ; bottom: 0 }

    /*! This property holds the item for the slider handle.
        You can access the slider through the \c control property
    */
    property Component handle:

        WGButtonFrame {
            id: handleFrame
            implicitWidth: defaultSpacing.minimumRowHeight - defaultSpacing.rowSpacing * 2
            implicitHeight: defaultSpacing.minimumRowHeight - defaultSpacing.rowSpacing * 2
            color: control.enabled ? control.__handlePosList[buttonid].handleColor : palette.MainWindowColor
            borderColor: control.__activeHandle == buttonid && control.activeFocus ? palette.HighlightShade : palette.DarkerShade
            highlightColor: control.__hoveredHandle == buttonid ? palette.LighterShade : "transparent"
            innerBorderColor: control.enabled ? palette.LightShade : "transparent"

    }
    /*! This property holds the background groove of the slider.

        You can access the handle position through the \c styleData.handlePosition property.
    */
    property Component groove: Item {
        anchors.verticalCenter: parent.verticalCenter
        implicitWidth: Math.round(TextSingleton.implicitHeight * 4.5)
        implicitHeight: Math.max(6, Math.round(TextSingleton.implicitHeight * 0.3))
        WGTextBoxFrame {
            radius: defaultSpacing.standardRadius
            anchors.fill: parent
            color: control.enabled ? palette.TextBoxColor : "transparent"
        }
    }

    property Component bar: Item {
        property color fillColor: control.__handlePosList[barid].barColor
        clip: true
        Rectangle {
            clip: true
            anchors.fill: parent
            anchors.margins: defaultSpacing.standardBorderSize
            border.color: control.enabled ? Qt.darker(fillColor, 1.2) : palette.LighterShade
            radius: defaultSpacing.halfRadius
            color: control.enabled ? fillColor : palette.LightShade
        }
    }

    /*! This property holds the tick mark labels
        \since QtQuick.Controls.Styles 1.1

        You can access the handle width through the \c styleData.handleWidth property.
    */

    property Component tickmarks: Repeater {
        id: repeater
        model: control.stepSize > 0 ? 1 + (control.maximumValue - control.minimumValue) / control.stepSize : 0
        WGSeparator {
            vertical_: !control.__horizontal
            color: palette.LightPanelColor
            width: defaultSpacing.separatorWidth
            height: defaultSpacing.standardMargin
            y: defaultSpacing.doubleMargin
            x: control.__handleWidth / 2 + index * ((repeater.width - control.__handleWidth) / (repeater.count-1)) - (defaultSpacing.separatorWidth / 2)
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
            id: sliderFrame
            anchors.centerIn: parent
            height: control.__horizontal ? control.__handleHeight : control.height
            width: control.__horizontal ? control.width : control.__handleWidth

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
                        width: control.__handlePosList[index].range.position - control.__handlePosList[index].barMinPos - padding.left - padding.right
                    }
                }
            }

            Loader {
                id: tickMarkLoader
                anchors.centerIn: parent
                height: control.height
                width: control.width
                sourceComponent: control.tickmarksEnabled ? tickmarks : null
            }

            Repeater {
            model: control.__handlePosList
                Loader {
                    id: handleLoader
                    sourceComponent: handle
                    property int buttonid: index

                    property bool shrinkingHandle: control.__handlePosList[index].rangePartnerHandle != control.__handlePosList[index]

                    anchors.verticalCenter: shrinkingHandle ? undefined : grooveLoader.verticalCenter

                    anchors.top: shrinkingHandle && !control.__handlePosList[index].maxHandle ? sliderFrame.top : undefined
                    anchors.bottom: shrinkingHandle && control.__handlePosList[index].maxHandle ? sliderFrame.bottom : undefined

                    height: control.__handlePosList[index].__overlapping ? parentSlider.__handleHeight / 2 : parentSlider.__handleHeight

                    Behavior on height{
                        NumberAnimation {
                            duration: 120
                            easing {
                                type: Easing.OutCirc
                                amplitude: 1.0
                                period: 0.5
                            }
                        }
                    }


                    x: control.__handlePosList[index].range.position - (control.__handleWidth / 2)
                    onLoaded: {
                        control.__handlePosList[index].handleIndex = index
                        control.__handleHeight = handleLoader.implicitHeight
                        control.__handleWidth = handleLoader.implicitWidth
                    }

                    MouseArea {
                        hoverEnabled: true
                        anchors.fill: parent
                        propagateComposedEvents: true

                        onEntered: {
                            control.__hoveredHandle = handleIndex
                        }
                        onExited: {
                            if (control.__hoveredHandle == handleIndex)
                            {
                               control.__hoveredHandle = -1
                            }
                        }

                        onPressed: {
                            control.__activeHandle = handleIndex
                            control.forceActiveFocus()
                            mouse.accepted = false
                        }

                    }
                }
            }
        }
    }
}
