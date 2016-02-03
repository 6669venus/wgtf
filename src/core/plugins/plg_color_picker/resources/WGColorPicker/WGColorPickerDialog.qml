import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2

import BWControls 1.0
import WGControls 1.0
import WGColorPicker 1.0

WGFileDialog {
    id: mainDialog

    property bool showAlphaChannel: true

    onOpen: {
        cpInstance.width = dWidth
        cpInstance.height = dHeight
        colorPicker.initialColor = curValue
        cpInstance.open()
    }

    onClose: {
        cpInstance.close()
    }


    Dialog {
        id: cpInstance
        modality: mainDialog.modality
        title: mainDialog.title

        property color initialColor: "#FFFFFF"
        property color currentColor: initialColor

        contentItem: Rectangle {
            color: palette.MainWindowColor
            anchors.margins: defaultSpacing.standardMargin

            WGColorPicker {
                id: colorPicker
                anchors.fill: parent

                showAlpha: showAlphaChannel

                onOkClicked: {
                    mainDialog.accepted(colorPicker.currentColor)
                }

                onCancelClicked: {
                    mainDialog.rejected()
                }
            }
        }
    }
}
