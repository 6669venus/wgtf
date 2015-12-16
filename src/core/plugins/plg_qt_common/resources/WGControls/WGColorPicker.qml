import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

import BWControls 1.0
import WGControls 1.0

// WIP Color Picker


Rectangle {
    id: basePanel
    objectName: "WGColorPicker"

    property color currentColor: Qt.hsla(hValue,sValue,lValue,aValue)

    property double hValue: 0
    property double sValue: 0
    property double lValue: 0
    property double aValue: 1

    color: palette.MainWindowColor

    Item {
        anchors.fill: parent
        anchors.margins: defaultSpacing.standardMargin
        Item {
            id: topBar
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right

            height: defaultSpacing.minimumRowHeight
        }

        Item {
            id: leftColumn
            anchors.top: topBar.bottom
            anchors.left: parent.left
            anchors.bottom: parent.bottom

            width: parent.width * 0.5


            ColumnLayout {
                anchors.fill: parent

                Item {
                    Layout.fillWidth: true
                    Layout.preferredHeight: width

                    WGColorWheel {
                        id: colorWheel
                        anchors.fill: parent
                        anchors.margins: defaultSpacing.standardMargin

                        onCurrentColorChanged: {
                            basePanel.hValue = chroma
                            basePanel.sValue = saturation
                            basePanel.lValue = lightness
                        }

                        Connections {
                            target: basePanel
                            onCurrentColorChanged: {
                                colorWheel.updateHSL(hValue,sValue,lValue)
                            }
                        }
                    }
                }

                Rectangle {
                    id: colorBox
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: currentColor
                }
            }
        }
        Item {
            id: rightColumn
            anchors.top: topBar.bottom
            anchors.left: leftColumn.right
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.leftMargin: defaultSpacing.standardMargin

            ColumnLayout {
                anchors.fill: parent

                Connections {
                    target: basePanel
                    onHValueChanged: {
                        hSlider.changeValue(basePanel.hValue, 0)
                        hBox.value = basePanel.hValue
                    }
                    onSValueChanged: {
                        sSlider.changeValue(basePanel.sValue, 0)
                        sBox.value = basePanel.sValue
                    }
                    onLValueChanged: {
                        lSlider.changeValue(basePanel.lValue, 0)
                        lBox.value = basePanel.lValue
                    }
                    onCurrentColorChanged: {
                        rSlider.changeValue(basePanel.currentColor.r, 0)
                        rBox.value = basePanel.currentColor.r
                        gSlider.changeValue(basePanel.currentColor.g, 0)
                        gBox.value = basePanel.currentColor.g
                        bSlider.changeValue(basePanel.currentColor.b, 0)
                        bBox.value = basePanel.currentColor.b
                    }
                }

                WGColorSlider {
                    id: hSlider
                    Layout.fillWidth: true
                    Layout.preferredHeight: defaultSpacing.minimumRowHeight
                    minimumValue: 0
                    maximumValue: 1.0
                    stepSize: 0.001
                    colorData: [Qt.rgba(1,0,0,1), Qt.rgba(1,1,0,1), Qt.rgba(0,1,0,1), Qt.rgba(0,1,1,1), Qt.rgba(0,0,1,1), Qt.rgba(1,0,1,1), Qt.rgba(1,0,0,1)]
                    positionData: [0, 0.167,0.333,0.5,0.667,0.833,1]
                    value: basePanel.hValue
                    linkColorsToHandles: false

                    onValueChanged: {
                        if (value != basePanel.hValue)
                        {
                            basePanel.hValue = value
                        }
                    }
                }
                RowLayout {
                    Layout.fillWidth: true
                    Layout.preferredHeight: defaultSpacing.minimumRowHeight

                    WGLabel {
                        text: "H: "
                        horizontalAlignment: Text.AlignRight
                    }

                    WGNumberBox {
                        id: hBox
                        Layout.fillWidth: true
                        minimumValue: 0
                        maximumValue: 1.0
                        stepSize: 0.001
                        decimals: 10
                        value: basePanel.hValue
                        onValueChanged: {
                            basePanel.hValue = value
                        }
                    }
                }

                WGColorSlider {
                    id: sSlider
                    Layout.fillWidth: true
                    Layout.preferredHeight: defaultSpacing.minimumRowHeight
                    minimumValue: 0
                    maximumValue: 1.0
                    stepSize: 0.001
                    colorData: [Qt.hsla(hValue,0,lValue,1), Qt.hsla(hValue,1,lValue,1)]
                    positionData: [0, 1]
                    value: basePanel.sValue
                    linkColorsToHandles: false

                    onValueChanged: {
                        if (value != basePanel.sValue)
                        {
                            basePanel.sValue = value
                        }
                    }
                }
                RowLayout {
                    Layout.fillWidth: true
                    Layout.preferredHeight: defaultSpacing.minimumRowHeight

                    WGLabel {
                        text: "S: "
                        horizontalAlignment: Text.AlignRight
                    }

                    WGNumberBox {
                        id: sBox
                        Layout.fillWidth: true
                        minimumValue: 0
                        maximumValue: 1.0
                        stepSize: 0.001
                        decimals: 10
                        value: basePanel.sValue
                        onValueChanged: {
                            basePanel.sValue = value
                        }
                    }
                }

                WGColorSlider {
                    id: lSlider
                    Layout.fillWidth: true
                    Layout.preferredHeight: defaultSpacing.minimumRowHeight
                    minimumValue: 0
                    maximumValue: 1.0
                    stepSize: 0.001
                    colorData: [Qt.hsla(hValue,sValue,0,1), Qt.hsla(hValue,sValue,0.5,1),Qt.hsla(hValue,sValue,1,1)]
                    positionData: [0,0.5, 1]
                    value: basePanel.lValue
                    linkColorsToHandles: false

                    onValueChanged: {
                        if (value != basePanel.lValue)
                        {
                            basePanel.lValue = value
                        }
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    Layout.preferredHeight: defaultSpacing.minimumRowHeight

                    WGLabel {
                        text: "L: "
                        horizontalAlignment: Text.AlignRight
                    }

                    WGNumberBox {
                        id: lBox
                        Layout.fillWidth: true
                        minimumValue: 0
                        maximumValue: 1.0
                        stepSize: 0.001
                        decimals: 10
                        value: basePanel.lValue
                        onValueChanged: {
                            basePanel.lValue = value
                        }
                    }
                }

                WGSeparator {
                    Layout.fillWidth: true
                }

                WGColorSlider {
                    id: rSlider
                    Layout.fillWidth: true
                    Layout.preferredHeight: defaultSpacing.minimumRowHeight
                    minimumValue: 0
                    maximumValue: 1.0
                    stepSize: 0.001
                    colorData: [Qt.rgba(0,basePanel.currentColor.g,basePanel.currentColor.b,1), Qt.rgba(1,basePanel.currentColor.g,basePanel.currentColor.b,1)]
                    positionData: [0, 1]
                    value: basePanel.currentColor.r
                    linkColorsToHandles: false
                }
                RowLayout {
                    Layout.fillWidth: true
                    Layout.preferredHeight: defaultSpacing.minimumRowHeight

                    WGLabel {
                        text: "R: "
                        horizontalAlignment: Text.AlignRight
                    }

                    WGNumberBox {
                        id: rBox
                        Layout.fillWidth: true
                        minimumValue: 0
                        maximumValue: 1.0
                        stepSize: 0.001
                        decimals: 10
                        value: basePanel.currentColor.r
                    }
                }

                WGColorSlider {
                    id: gSlider
                    Layout.fillWidth: true
                    Layout.preferredHeight: defaultSpacing.minimumRowHeight
                    minimumValue: 0
                    maximumValue: 1.0
                    stepSize: 0.001
                    colorData: [Qt.rgba(basePanel.currentColor.r,0,basePanel.currentColor.b,1), Qt.rgba(basePanel.currentColor.r,1,basePanel.currentColor.b,1)]
                    positionData: [0, 1]
                    value: basePanel.currentColor.g
                    linkColorsToHandles: false
                }
                RowLayout {
                    Layout.fillWidth: true
                    Layout.preferredHeight: defaultSpacing.minimumRowHeight

                    WGLabel {
                        text: "G: "
                        horizontalAlignment: Text.AlignRight
                    }

                    WGNumberBox {
                        id: gBox
                        Layout.fillWidth: true
                        minimumValue: 0
                        maximumValue: 1.0
                        stepSize: 0.001
                        decimals: 10
                        value: basePanel.currentColor.g
                    }
                }

                WGColorSlider {
                    id: bSlider
                    Layout.fillWidth: true
                    Layout.preferredHeight: defaultSpacing.minimumRowHeight
                    minimumValue: 0
                    maximumValue: 1.0
                    stepSize: 0.001
                    colorData: [Qt.rgba(basePanel.currentColor.r,basePanel.currentColor.g,0,1), Qt.rgba(basePanel.currentColor.r,basePanel.currentColor.g,1,1)]
                    positionData: [0, 1]
                    value: basePanel.currentColor.b
                    linkColorsToHandles: false
                }
                RowLayout {
                    Layout.fillWidth: true
                    Layout.preferredHeight: defaultSpacing.minimumRowHeight

                    WGLabel {
                        text: "B: "
                        horizontalAlignment: Text.AlignRight
                    }

                    WGNumberBox {
                        id: bBox
                        Layout.fillWidth: true
                        minimumValue: 0
                        maximumValue: 1.0
                        stepSize: 0.001
                        decimals: 10
                        value: basePanel.currentColor.b
                    }
                }

                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
            }
        }
    }
}
