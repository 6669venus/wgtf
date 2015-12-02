import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import BWControls 1.0

FileDialog {
    id: mainDialog

    onOpen: {
        console.log("Opened")
    }

    onAccepted: {
        //TODO: Make this set the data
        fileComponent.fileUrl = defaultFileDialog.fileUrl
        parent.dialogClosed(defaultFileDialog.fileUrl, true)
    }
    onRejected: {
        parent.dialogClosed("", false)
    }
}
