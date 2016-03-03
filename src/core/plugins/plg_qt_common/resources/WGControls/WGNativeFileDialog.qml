import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import BWControls 1.0

WGFileDialog {
    objectName: "WGNativeFileDialog"
    id: mainDialog

    onOpen: {
        fileDialogInstance.folder = curValue
        fileDialogInstance.open()
    }

    onClose: {
        fileDialogInstance.close()
    }

    FileDialog {
        objectName: "FileDialog"
        id: fileDialogInstance
        folder: mainDialog.folder
        modality: mainDialog.modality
        nameFilters: mainDialog.nameFilters
        selectedNameFilter: mainDialog.selectedNameFilter
        title: mainDialog.title

        onAccepted: {
            mainDialog.accepted(fileUrl)
        }

        onRejected: {
            mainDialog.rejected()
        }
    }
}
