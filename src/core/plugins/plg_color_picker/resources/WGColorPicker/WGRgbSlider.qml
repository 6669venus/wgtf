import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

import BWControls 1.0
import WGControls 1.0
import WGColorPicker 1.0

ColumnLayout {
    id: rgbSlider

    property real rVal
    property real gVal
    property real bVal

    onRValChanged: {
        rSlider.changeValue(rgbSlider.rVal, 0)
        rBox.value = rgbSlider.rVal
    }
    onGValChanged: {
        gSlider.changeValue(rgbSlider.gVal, 0)
        gBox.value = rgbSlider.gVal
    }
    onBValChanged: {
        bSlider.changeValue(rgbSlider.bVal, 0)
        bBox.value = rgbSlider.bVal
    }

    RowLayout {
        Layout.fillWidth: true
        Layout.preferredHeight: defaultSpacing.minimumRowHeight

        WGLabel {
            text: "R:"
            horizontalAlignment: Text.AlignRight
            Layout.preferredWidth: defaultSpacing.doubleMargin
        }

        WGColorSlider {
            id: rSlider
            Layout.fillWidth: true
            Layout.preferredHeight: defaultSpacing.minimumRowHeight
            minimumValue: 0
            maximumValue: 1.0
            stepSize: 0.001
            colorData: [Qt.rgba(0,rgbSlider.gVal,rgbSlider.bVal,1), Qt.rgba(1,rgbSlider.gVal,rgbSlider.bVal,1)]
            positionData: [0, 1]
            value: rgbSlider.rVal
            linkColorsToHandles: false

            onValueChanged: {
                if (value != rgbSlider.rVal)
                {
                    rgbSlider.rVal = value
                }
            }
        }

        WGNumberBox {
            id: rBox
            Layout.preferredWidth: 105
            minimumValue: 0
            maximumValue: 1.0
            stepSize: 0.001
            decimals: 10
            value: rgbSlider.rVal
            onValueChanged: {
                if (value != rgbSlider.rVal)
                {
                    rgbSlider.rVal = value
                }
            }
        }
    }

    RowLayout {
        Layout.preferredWidth: Math.round(parent.width)
        Layout.preferredHeight: defaultSpacing.minimumRowHeight

        WGLabel {
            text: "G:"
            horizontalAlignment: Text.AlignRight
            Layout.preferredWidth: defaultSpacing.doubleMargin
        }

        WGColorSlider {
            id: gSlider
            Layout.fillWidth: true
            Layout.preferredHeight: defaultSpacing.minimumRowHeight
            minimumValue: 0
            maximumValue: 1.0
            stepSize: 0.001
            colorData: [Qt.rgba(rgbSlider.rVal,0,rgbSlider.bVal,1), Qt.rgba(rgbSlider.rVal,1,rgbSlider.bVal,1)]
            positionData: [0, 1]
            value: rgbSlider.gVal
            linkColorsToHandles: false

            onValueChanged: {
                if (value != rgbSlider.gVal)
                {
                    rgbSlider.gVal = value
                }
            }
        }

        WGNumberBox {
            id: gBox
            Layout.preferredWidth: 105
            minimumValue: 0
            maximumValue: 1.0
            stepSize: 0.001
            decimals: 10
            value: rgbSlider.gVal
            onValueChanged: {
                if (value != rgbSlider.gVal)
                {
                    rgbSlider.gVal = value
                }
            }
        }
    }

    RowLayout {
        Layout.preferredWidth: Math.round(parent.width)
        Layout.preferredHeight: defaultSpacing.minimumRowHeight

        WGLabel {
            text: "B:"
            horizontalAlignment: Text.AlignRight
            Layout.preferredWidth: defaultSpacing.doubleMargin
        }

        WGColorSlider {
            id: bSlider
            Layout.fillWidth: true
            Layout.preferredHeight: defaultSpacing.minimumRowHeight
            minimumValue: 0
            maximumValue: 1.0
            stepSize: 0.001
            colorData: [Qt.rgba(rgbSlider.rVal,rgbSlider.gVal,0,1), Qt.rgba(rgbSlider.rVal,rgbSlider.gVal,1,1)]
            positionData: [0, 1]
            value: rgbSlider.bVal
            linkColorsToHandles: false

            onValueChanged: {
                if (value != rgbSlider.bVal)
                {
                    rgbSlider.bVal = value
                }
            }
        }

        WGNumberBox {
            id: bBox
            Layout.preferredWidth: 105
            minimumValue: 0
            maximumValue: 1.0
            stepSize: 0.001
            decimals: 10
            value: rgbSlider.bVal
            onValueChanged: {
                if (value != rgbSlider.bVal)
                {
                    rgbSlider.bVal = value
                }
            }
        }
    }

}
