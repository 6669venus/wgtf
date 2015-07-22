import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import BWControls 1.0
import WGControls 1.0



ColumnLayout {
	property var title: "Viewport"
	property var layoutHints: { 'viewport': 1.0, 'right': 0.5, 'test':0.5 }
    spacing: 2
    Layout.fillHeight: true

    property QtObject panelProps: WGPanelProperties{}

    Rectangle {
        id: viewport_tools
        color: palette.DarkColor
        height: 24
        Layout.fillWidth: true

        RowLayout {
            WGToolButton {
                iconSource: "qrc:///icons/close_16x16"
            }
            WGSliderControl {
                width: 200
                Layout.fillWidth: true
                minimumValue: 0
                maximumValue: 100
                stepSize: 1
                value: 40
                showValue_: false
                label_: "Snapping Slider:"
            }

            WGToolButton {
                iconSource: "qrc:///icons/open_16x16"
            }
            WGToolButton {
                iconSource: "qrc:///icons/show_16x16"
            }
            WGToolButton {
                iconSource: "qrc:///icons/show_16x16"
            }
            WGToolButton {
                iconSource: "qrc:///icons/show_16x16"
            }
            Rectangle {
                width: 1
                Layout.fillHeight: true
                color: palette.LighterShade
            }

            WGToolButton {
                iconSource: "qrc:///icons/show_16x16"
            }

            WGSliderControl {
                width: 200
                Layout.fillWidth: true
                minimumValue: 0
                maximumValue: 100
                stepSize: 1
                value: 40
                showValue_: false
                label_: "Snapping Slider:"
            }
        }
    }
    Rectangle {
        id: viewport
        color: palette.DarkColor
        Layout.fillWidth: true
        Layout.fillHeight: true
    }
    Rectangle {
        id: viewport_transform
        color: palette.DarkColor
        height: 24
        Layout.fillWidth: true

        RowLayout {
            anchors.left: parent.left
            anchors.right: parent.right
            WGLabel { text: "Position:" }
            WGLabel { text: "X" }
            WGNumberBox { Layout.fillWidth: true; noArrows_: true }
            WGLabel { text: "Y" }
            WGNumberBox { Layout.fillWidth: true; noArrows_: true }
            WGLabel { text: "Z" }
            WGNumberBox { Layout.fillWidth: true; noArrows_: true }

            Rectangle {
                width: 1
                Layout.fillHeight: true
                color: palette.LighterShade
            }

            WGLabel { text: "Orientation:" }
            WGLabel { text: "X" }
            WGNumberBox { Layout.fillWidth: true; noArrows_: true }
            WGLabel { text: "Y" }
            WGNumberBox { Layout.fillWidth: true; noArrows_: true }
            WGLabel { text: "Z" }
            WGNumberBox { Layout.fillWidth: true; noArrows_: true }
            WGPushButton { text: "Reset" }
        }
    }
}