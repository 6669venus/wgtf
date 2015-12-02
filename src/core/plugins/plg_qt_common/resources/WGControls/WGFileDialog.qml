import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import BWControls 1.0

FileDialog {
    id: mainDialog

    onAccepted: {
        //TODO: Make this set the data
        dialogClosed(mainDialog.fileUrl, true)
    }

    onRejected: {
        dialogClosed("", false)
    }

}
