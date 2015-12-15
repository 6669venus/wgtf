import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import BWControls 1.0

WGFileDialog {
    id: mainDialog

    onOpen: {
        cpInstance.width = dWidth
        cpInstance.height = dHeight
        cpInstance.initialColor = curValue
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
            width: parent.width
            height: parent.height
            color: palette.MainWindowColor
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: defaultSpacing.standardMargin

                WGColorPicker {
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                }

                WGExpandingRowLayout {
                    Layout.preferredHeight: defaultSpacing.minimumRowHeight
                    Layout.fillWidth: true

                    Item {
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight
                        Layout.fillWidth: true
                    }

                    WGPushButton {
                        text: "Select"
                        onClicked: {
                            cpInstance.accepted()
                        }
                    }

                    WGPushButton {
                        text: "Cancel"
                        onClicked: {
                            cpInstance.rejected()
                        }
                    }
                }
            }
        }

        onAccepted: {
            mainDialog.accepted(currentColor)
        }

        onRejected: {
            mainDialog.rejected()
        }
    }
}
