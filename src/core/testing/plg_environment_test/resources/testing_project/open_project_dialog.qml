import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2

Item{
    id: root
    Component.onCompleted: {
        fileDialogInstance.open();
    }

    FileDialog {
        objectName: "ProjectFileDialog"
        id: fileDialogInstance
        folder: ""
        modality: Qt.WindowModal
        nameFilters: "NGT Project files(*.ngtprj)"
        selectedNameFilter: "NGT Project files(*.ngtprj)"
        title: "Selected Project File"

        onAccepted: {
            openProject(fileUrl);
        }

        onRejected: {
        }
    }
}
