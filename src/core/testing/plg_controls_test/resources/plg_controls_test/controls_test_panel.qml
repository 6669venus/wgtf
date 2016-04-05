import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls 1.0 as WGOne
import WGControls 2.0

/**
 *	Sample panel with all the available controls and styles.
 *	Used for control testing
 */
Rectangle {
    id: mainWindow

    visible: true
    property var title: qsTr("QML Controls Test Window")

    color: palette.mainWindowColor

    WGOne.WGFormLayout {
        anchors.fill: parent
        anchors.leftMargin: 5
        anchors.rightMargin: 5

        WGSlider {
            property string label: "WGSlider no handle"
            Layout.fillWidth: true
            Layout.preferredHeight: defaultSpacing.minimumRowHeight
            minimumValue: 0
            maximumValue: 100
            stepSize: 1.0
            value: 25
        }

        WGSlider {
            property string label: "WGSlider no clamp"
            Layout.fillWidth: true
            Layout.preferredHeight: defaultSpacing.minimumRowHeight
            minimumValue: 0
            maximumValue: 100
            stepSize: 1.0
            value: 25
            handleClamp: false
        }

        WGSliderControl {
            id: buttonSlider
            label: "WGSliderControl"
            Layout.fillWidth: true
            Layout.preferredHeight: defaultSpacing.minimumRowHeight
            minimumValue: 0
            maximumValue: 100
            stepSize: 1.0
            value: 25
        }
        WGOne.WGPushButton {
            text: "Set to 75"
            onClicked: {
                buttonSlider.setHandleValue(75)
            }
        }

        WGOne.WGPushButton {
            text: "No handle"
            onClicked: {
                buttonSlider.setHandleValue(7,20)
            }
        }

        WGOne.WGPushButton {
            text: "Out of range"
            onClicked: {
                buttonSlider.setHandleValue(500,0)
            }
        }

        WGSliderControl {
            label: "WGSliderControl"
            Layout.fillWidth: true
            Layout.preferredHeight: defaultSpacing.minimumRowHeight
            minimumValue: 0
            maximumValue: 100
            stepSize: 1.0
            value: 25
            handleClamp: false
        }

        WGGradientSlider {
            id: gradSlider
            property string label: "WGGradientSlider"
            Layout.fillWidth: true
            Component.onCompleted: {
                createColorHandle(0.15,handleStyle,__handlePosList.length, "red")
                createColorHandle(0.30,handleStyle,__handlePosList.length, "yellow")
                createColorHandle(0.45,handleStyle,__handlePosList.length, "green")
                createColorHandle(0.60,handleStyle,__handlePosList.length, "blue")
                createColorHandle(0.75,handleStyle,__handlePosList.length, "purple")
            }
        }

        RowLayout {
            Layout.preferredHeight: defaultSpacing.minimumRowHeight
            Layout.fillWidth: true

            WGOne.WGLabel {
                text: "Handles: "
            }

            WGOne.WGTextBox {
                id: handlesText

                Layout.preferredHeight: defaultSpacing.minimumRowHeight
                Layout.fillWidth: true

                text: "1,2,3"
            }

            WGOne.WGLabel {
                text: "Values: "
            }

            WGOne.WGTextBox {
                id: valuesText

                Layout.preferredHeight: defaultSpacing.minimumRowHeight
                Layout.fillWidth: true
                text: "0.40,0.50,0.60"
            }
        }

        WGOne.WGPushButton {
            text: "Move Handles!"
            onClicked: {
                var strHandles = handlesText.text.split(",")
                var strValues = valuesText.text.split(",")

                gradSlider.setHandleValue(strValues, strHandles)
            }
        }

        WGOne.WGPushButton {
            text: "Set 1 to .20"
            onClicked: {
                gradSlider.setHandleValue(.20, 1)
            }
        }

        WGGradientSlider {
            id: gradExtremeSlider
            property string label: "WGGradientSlider"
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
                color: "green"
            }
        }

        WGOne.WGPushButton {
            text: "Make point at 600"
            onClicked: {
                gradExtremeSlider.createHandleAtPixelPosition(600)
            }
        }
        WGOne.WGPushButton {
            text: "Make point at 0.66"
            onClicked: {
                gradExtremeSlider.createHandleAtPosition(0.66)
            }
        }

        WGSlider {
            property string label: "WGSlider one handle"
            Layout.fillWidth: true
            Layout.preferredHeight: defaultSpacing.minimumRowHeight
            minimumValue: 0
            maximumValue: 100
            stepSize: 1.0

            WGSliderHandle {
                value: 75
            }
        }

        WGSlider {
            property string label: "WGSlider multi-handle"
            Layout.fillWidth: true
            Layout.preferredHeight: defaultSpacing.minimumRowHeight
            minimumValue: 0
            maximumValue: 100
            stepSize: 1.0

            WGSliderHandle {
                value: 25
                showBar: false
            }

            WGSliderHandle {
                value: 75
                showBar: false
            }
        }

        WGRangeSlider {
            label: "WGRangeSlider"
            Layout.fillWidth: true
            Layout.preferredHeight: defaultSpacing.minimumRowHeight
            minimumValue: 0
            maximumValue: 100
            stepSize: 1.0
            lowerValue: 25
            upperValue: 75
        }

        WGRangeSlider {
            label: "WGRangeSlider no clamp"
            Layout.fillWidth: true
            Layout.preferredHeight: defaultSpacing.minimumRowHeight
            minimumValue: 0
            maximumValue: 100
            stepSize: 1.0
            lowerValue: 25
            upperValue: 75
            handleClamp: false
        }

        WGColorSlider {
            Layout.fillWidth: true
            minimumValue: 0
            maximumValue: 255
            stepSize: 1
            value: 128
            gradient: Gradient {
                GradientStop {
                    position: 0
                    color: "#FF0000"
                }
                GradientStop {
                    position: 0.5
                    color: "#FFFF00"
                }
                GradientStop {
                    position: 1
                    color: "#FFFFFF"
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 300

            WGSlider {
                Layout.preferredHeight: 300
                Layout.preferredWidth: defaultSpacing.minimumRowHeight
                minimumValue: 0
                maximumValue: 100
                stepSize: 1.0
                value: 25
                orientation: Qt.Vertical
            }

            WGSlider {
                Layout.preferredHeight: 300
                Layout.preferredWidth: 80
                minimumValue: 0
                maximumValue: 100
                stepSize: 1.0
                orientation: Qt.Vertical

                WGSliderHandle {
                    value: 75
                }
            }

            WGSlider {
                Layout.preferredHeight: 300
                Layout.preferredWidth: 80
                minimumValue: 0
                maximumValue: 100
                stepSize: 1.0
                orientation: Qt.Vertical

                WGSliderHandle {
                    value: 25
                    showBar: false
                }

                WGSliderHandle {
                    value: 75
                    showBar: false
                }
            }
            WGSliderControl {
                Layout.preferredHeight: 300
                Layout.preferredWidth: valueBoxWidth
                minimumValue: 0
                maximumValue: 100
                stepSize: 1.0
                value: 25
                orientation: Qt.Vertical
                valueBoxWidth: 50
            }

            WGRangeSlider {
                Layout.preferredHeight: 300
                Layout.preferredWidth: valueBoxWidth
                minimumValue: 0
                maximumValue: 100
                stepSize: 1.0
                lowerValue: 25
                upperValue: 75
                orientation: Qt.Vertical
                valueBoxWidth: 50
            }

            WGColorSlider {
                Layout.preferredHeight: 300
                Layout.preferredWidth: defaultSpacing.minimumRowHeight
                minimumValue: 0
                maximumValue: 255
                stepSize: 1
                value: 128
                orientation: Qt.Vertical
                gradient: Gradient {
                    GradientStop {
                        position: 0
                        color: "#FF0000"
                    }
                    GradientStop {
                        position: 0.5
                        color: "#FFFF00"
                    }
                    GradientStop {
                        position: 1
                        color: "#FFFFFF"
                    }
                }
            }

            WGGradientSlider {
                Layout.preferredHeight: 300
                Layout.preferredWidth: defaultSpacing.minimumRowHeight
                Component.onCompleted: {
                    createColorHandle(.25,handleStyle,__handlePosList.length, "red")
                    createColorHandle(.5,handleStyle,__handlePosList.length, "yellow")
                    createColorHandle(.75,handleStyle,__handlePosList.length, "white")
                }
                orientation: Qt.Vertical
            }

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: 300
            }
        }
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }

}

