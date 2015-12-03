import QtQuick 2.3
import QtQuick.Layouts 1.1
import BWControls 1.0

WGFileSelectBox {
    id: fileComponent
    anchors.left: parent.left
    anchors.right: parent.right

    readOnly: true

    //TODO: Make this point to data
    text: "file://filelocation/sample.file"

    //TODO: Make this choose the right dialog to open based on metadata
    dialog: WGNativeFileDialog {}

    onFileChosen: {
        //TODO: Make this set the data
        text = selectedFile
    }

    onFileRejected: {
        closeDialog()
    }
}
