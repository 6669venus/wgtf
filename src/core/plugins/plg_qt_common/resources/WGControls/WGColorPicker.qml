import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

import BWControls 1.0
import WGControls 1.0

// WIP Color Picker


Rectangle {
    id: basePanel
    objectName: "WGColorPicker"

    property color testColor: Qt.hsla(0,1,1,1)

    color: palette.MainWindowColor

    WGColorWheel {
        id: colorWheel
        height: 360
        width: 360

        anchors.horizontalCenter: parent.horizontalCenter

        onCurrentColorChanged: {
            testColor = currentColor
        }
    }

    Rectangle {
        id: colorBox
        anchors.top: colorWheel.bottom
        height: 200
        width: 200
        color: testColor
    }
    ColumnLayout {
        anchors.left: colorBox.right
        anchors.leftMargin: defaultSpacing.standardMargin
        anchors.top: colorWheel.bottom
        height: 200
        width: 200

        Text {
            text: "H: " + colorWheel.chroma
            color: "white"
        }
        Text {
            text: "S: " + colorWheel.saturation
            color: "white"
        }
        Text {
            text: "L: " + colorWheel.lightness
            color: "white"
        }
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}
