import QtQuick 2.3
import QtQuick.Layouts 1.1
import BWControls 1.0

WGExpandingRowLayout {
    id: fileComponent
    anchors.left: parent.left
    anchors.right: parent.right

    /*! Determines whether text in the url can be edited manually.
      The default value is false.
    */
    property alias readOnly: textField.readOnly

    /*! The folder the fileDialog will open to by default.
      The default value is empty
    */
    property url folder

    /*! The path of the file which was selected by the user.

        Note: This property is set only if exactly one file was selected. In all other cases, it will be empty.
    */
    property url fileUrl

    /*! The list of file paths which were selected by the user.
    */
    property var fileUrls

    /*! Whether the dialog should be shown modal with respect to the window containing the dialog's parent Item,
        modal with respect to the whole application, or non-modal.
        The default value is Qt.WindowModal
    */

    property var modality: Qt.WindowModal

    /*! This array contains a list of filename Filters in the format:
      \code{.js}
      description (*.extension1 *.extension2 ...)
      \endcode
      All files can be selected using (*)
      Default value includes the following filters:

      \code{.js}
      "All files (*)",
      "Image files (*.jpg *.png *.bmp *.dds)",
      "Model files (*.model *.primitives *.visual)",
      "Script files (*.txt *.xml *.py)",
      "Audio files (*.fsb *.fev)",
      \endcode
    */
    property var nameFilters: {
        [
            "All files (*)",
            "Image files (*.jpg *.png *.bmp *.dds)",
            "Model files (*.model *.primitives *.visual)",
            "Script files (*.txt *.xml *.py)",
            "Audio files (*.fsb *.fev)",
        ]
    }

    /*! This currently selected filename filter
      The default value is All Files (*)
    */
    property string selectedNameFilter: nameFilters[0]

    property string title: "Select a File"

    /*! The dialog QML file to open.
      The default value is WGFileDialog
    */
    property Component dialog: WGFileDialog {}

    /*! internal */
    property var __dialogInstance

    /*! This function opens the desired dialog box depending on whether useAssetBrowser == true or not.
    */
    function openDialog() {
        __dialogInstance.nameFilters = fileComponent.nameFilters
        __dialogInstance.selectedNameFilter = fileComponent.selectedNameFilter
        __dialogInstance.folder = fileComponent.folder
        __dialogInstance.title = fileComponent.title
        __dialogInstance.modality = fileComponent.modality
        __dialogInstance.open()
    }

    signal dialogClosed(string url, bool accepted)

    onDialogClosed: {
        console.log("Closed")
        if (accepted)
        {

            console.log("Accepted: " + url)
            //TODO: Set the data
            textField.text = url
        }
        __dialogInstance.close()
    }

    WGTextBox {
        id: textField
        Layout.fillWidth: true
        Layout.preferredHeight: defaultSpacing.minimumRowHeight

        //TODO: Make this point to the data
        text: "file:///c:/testlocation/"
        readOnly: true

        MouseArea {
            anchors.fill: parent
            propagateComposedEvents: true
            hoverEnabled: true

            cursorShape: Qt.IBeamCursor

            //Difficult to make discrete text selection and open via double-click work at the same time.
            //This is a compromise so that the filename can still be copied to clipboard
            onClicked: {
                textField.selectAll()
            }

            onDoubleClicked: {
                openDialog()
            }
        }
    }

    WGPushButton {
        id: openButton
        iconSource: "icons/open_16x16.png"
        Layout.preferredHeight: defaultSpacing.minimumRowHeight
        Layout.preferredWidth: defaultSpacing.minimumRowHeight

        enabled: false

        onClicked: {
            openDialog()
        }
    }

    Component.onCompleted: {
        __dialogInstance = dialog.createObject(fileComponent)
        openButton.enabled = true
    }
}

