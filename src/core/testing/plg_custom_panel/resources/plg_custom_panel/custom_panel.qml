import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

import BWControls 1.0
import WGControls 1.0

WGPanel {

    title: qsTr("Custom Testing Panel")
    layoutHints: { 'test': 0.1 }

    color: palette.MainWindowColor

    Label {
        id: labelHello
        visible: true
        x: 0
        y: 48
        width: 200
        height: 13
        text: "Press the button."
    }

    Button {
        id: buttonSayHello
        visible: true
        x: 48
        y: 19
        text: "Be Polite: Say Hello!"

        onClicked: {
            labelHello.text = "Hello World!"
        }
    }
}
