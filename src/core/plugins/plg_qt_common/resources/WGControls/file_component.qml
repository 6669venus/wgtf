import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import BWControls 1.0

WGExpandingRowLayout {
    id: fileComponent
    anchors.left: parent.left
    anchors.right: parent.right

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

    property alias modality: defaultFileDialog.modality

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

    /*! This property determines if multiple files can be selected or not
      The default value is false
    */
    property bool selectMultiple: false

    /*! This property determines if folders can be selected or not
      The default value is false
    */
    property bool selectFolder: false

    /*! This property determines if the control should open an Asset Browser instance
      The default value is false (Setting this to true is NOT implemented yet)
    */
    property bool useAssetBrowser: false

    /*! This title of the File Dialog
      The default value is based on selectFolder and selectMultiple
    */
    property string title: {
        if (selectMultiple)
        {
            "Select Files"
        }
        else if (selectFolder)
        {
            "Select a Folder"
        }
        else
        {
            "Select a File"
        }
    }

    /*! This function opens the desired dialog box depending on whether useAssetBrowser == true or not.
    */
    function openDialog() {
        //TODO: Set the folder to a useful initial location.

        if (useAssetBrowser)
        {
            //TODO: Create Asset Browser instance
            console.log ("file_component.qml: Asset Browser instance not implemented. Set useAssetBrowser: false")
        }
        else
        {
            defaultFileDialog.open()
        }
    }

    WGTextBox {
        id: textField
        Layout.fillWidth: true
        Layout.preferredHeight: defaultSpacing.minimumRowHeight

        //TODO: Make this point to the data
        text: defaultFileDialog.fileUrl
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
        iconSource: "icons/open_16x16.png"
        Layout.preferredHeight: defaultSpacing.minimumRowHeight
        Layout.preferredWidth: defaultSpacing.minimumRowHeight

        onClicked: {
            openDialog()
        }

        FileDialog {
            id: defaultFileDialog

            visible: false

            title: fileComponent.title

            nameFilters: fileComponent.nameFilters
            selectedNameFilter: fileComponent.selectedNameFilter
            selectMultiple: fileComponent.selectMultiple
            selectFolder: fileComponent.selectFolder

            onAccepted: {
                //TODO: Make this set the data
                fileComponent.fileUrl = defaultFileDialog.fileUrl
                defaultFileDialog.close()
            }
            onRejected: {
                defaultFileDialog.close()
            }
        }
    }
}

