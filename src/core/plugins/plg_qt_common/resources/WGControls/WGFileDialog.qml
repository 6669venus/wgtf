import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import BWControls 1.0

Item {
    id: mainDialog

    property url folder

    property var modality

    property var nameFilters

    property string selectedNameFilter

    property string title

    signal open()

    signal close()

    signal accepted(url selectedFile)

    signal rejected()

    onAccepted: {
        //TODO: Make this set the data
        dialogClosed(selectedFile, true)
    }

    onRejected: {
        dialogClosed("", false)
    }
}
