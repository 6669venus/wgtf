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

    /*! This property determines if the sidebar in the dialog containing shortcuts and bookmarks is visible.
      The default value is false
    */
    property bool sidebarVisible: false

    /*! This property determines if the control should open an Asset Browser instance
      The default value is false (Setting this to true is NOT implemented yet)
    */
    property bool useAssetBrowser: false

    /*! This title of the File Dialog
      The default value is based on selectFolder and selectMultiple
    */
    property string title: {
        if (selectMultiple && selectFolder)
        {
            "Select Files and/or Folders"
        }
        else if (selectMultiple)
        {
            "Select File(s)"
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

    WGTextBox {
        id: textField
        Layout.fillWidth: true
        Layout.preferredHeight: defaultSpacing.minimumRowHeight
        text: itemData.Value
        readOnly: true

        MouseArea {
            anchors.fill: parent
            onDoubleClicked: {
                defaultFileDialog.open()
            }
        }
    }

    WGPushButton {
        iconSource: "icons/open_16x16.png"
        Layout.preferredHeight: defaultSpacing.minimumRowHeight
        Layout.preferredWidth: defaultSpacing.minimumRowHeight

        onClicked: {
            defaultFileDialog.open()
        }

        FileDialog {
            id: defaultFileDialog

            visible: false

            title: fileComponent.title

            nameFilters: fileComponent.nameFilters
            selectedNameFilter: fileComponent.selectedNameFilter
            selectMultiple: fileComponent.selectMultiple
            selectFolder: fileComponent.selectFolder
            sidebarVisible: fileComponent.sidebarVisible

            onAccepted: {
                defaultFileDialog.close()
                textField.text = fileDialog.fileUrls
            }
            onRejected: {
                defaultFileDialog.close()
            }
        }
    }
}

