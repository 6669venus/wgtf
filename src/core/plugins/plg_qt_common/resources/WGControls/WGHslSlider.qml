import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

import BWControls 1.0
import WGControls 1.0

ColumnLayout {
    id: hslSlider

    property real hVal
    property real sVal
    property real lVal

    onHValChanged: {
        hSlider.changeValue(hslSlider.hVal, 0)
        hBox.value = hslSlider.hVal
    }
    onSValChanged: {
        sSlider.changeValue(hslSlider.sVal, 0)
        sBox.value = hslSlider.sVal
    }
    onLValChanged: {
        lSlider.changeValue(hslSlider.lVal, 0)
        lBox.value = hslSlider.lVal
    }

    RowLayout {
        Layout.preferredWidth: Math.round(parent.width)
        Layout.preferredHeight: defaultSpacing.minimumRowHeight

        WGLabel {
            text: "H:"
            horizontalAlignment: Text.AlignRight
            Layout.preferredWidth: defaultSpacing.doubleMargin
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
            value: hslSlider.hVal
            linkColorsToHandles: false

            onValueChanged: {
                if (value != hslSlider.hVal)
                {
                    hslSlider.hVal = value
                }
            }
        }

        WGNumberBox {
            id: hBox
            Layout.preferredWidth: 105
            minimumValue: 0
            maximumValue: 1.0
            stepSize: 0.001
            decimals: 10
            value: hslSlider.hVal
            onValueChanged: {
                if (value != hslSlider.hVal)
                {
                    hslSlider.hVal = value
                }
            }
        }
    }

    RowLayout {
        Layout.preferredWidth: Math.round(parent.width)
        Layout.preferredHeight: defaultSpacing.minimumRowHeight

        WGLabel {
            text: "S:"
            horizontalAlignment: Text.AlignRight
            Layout.preferredWidth: defaultSpacing.doubleMargin
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
            value: hslSlider.sVal
            linkColorsToHandles: false

            onValueChanged: {
                if (value != hslSlider.sVal)
                {
                    hslSlider.sVal = value
                }
            }
        }

        WGNumberBox {
            id: sBox
            Layout.preferredWidth: 105
            minimumValue: 0
            maximumValue: 1.0
            stepSize: 0.001
            decimals: 10
            value: hslSlider.sVal
            onValueChanged: {
                if (value != hslSlider.sVal)
                {
                    hslSlider.sVal = value
                }
            }
        }
    }

    RowLayout {
        Layout.preferredWidth: Math.round(parent.width)
        Layout.preferredHeight: defaultSpacing.minimumRowHeight

        WGLabel {
            text: "L:"
            horizontalAlignment: Text.AlignRight
            Layout.preferredWidth: defaultSpacing.doubleMargin
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
            value: hslSlider.lVal
            linkColorsToHandles: false

            onValueChanged: {
                if (value != hslSlider.lVal)
                {
                    hslSlider.lVal = value
                }
            }
        }

        WGNumberBox {
            id: lBox
            Layout.preferredWidth: 105
            minimumValue: 0
            maximumValue: 1.0
            stepSize: 0.001
            decimals: 10
            value: hslSlider.lVal
            onValueChanged: {
                if (value != hslSlider.lVal)
                {
                    hslSlider.lVal = value
                }
            }
        }
    }

}
