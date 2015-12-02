import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import BWControls 1.0

WGFileDialog {
    id: mainDialog

    onOpen: {
        abInstance.open()
    }

    onClose: {
        abInstance.close()
    }

    Dialog {
        id: abInstance
        modality: mainDialog.modality
        title: mainDialog.title

        width: 800
        height: 600

        //TODO: make this point to the currently selected AB instance file
        property url fileUrl: "file:///sample_file"

        contentItem: Rectangle {
            width: 800
            height: 600
            color: palette.MainWindowColor
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: defaultSpacing.standardMargin

                WGAssetBrowser {
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    //TODO: Make this load a proper file system and AB stuff.
                    viewModel: view
                }

                WGExpandingRowLayout {
                    Layout.preferredHeight: defaultSpacing.minimumRowHeight
                    Layout.fillWidth: true

                    WGLabel {
                        text: "Selected File: "
                    }

                    WGTextBox {
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight
                        Layout.fillWidth: true
                        readOnly: true
                        text: abInstance.fileUrl
                    }

                    WGPushButton {
                        text: "Open"
                        onClicked: {
                            abInstance.accepted()
                        }
                    }

                    WGPushButton {
                        text: "Cancel"
                        onClicked: {
                            abInstance.rejected()
                        }
                    }
                }
            }
        }

        onAccepted: {
            mainDialog.accepted(fileUrl)
        }

        onRejected: {
            mainDialog.rejected()
        }
    }
}
