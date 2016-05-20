import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import BWControls 1.0

Item {
    id: itemRow
    objectName: "WGHeaderRow"

    width: childrenRect.width != 0 ? childrenRect.width : 1024
    height: childrenRect.height != 0 ? childrenRect.height : 1024

    property var columnDelegates: []
    property var columnSequence: []
    property var columnWidths: []
    property var headerData: []
    property alias columnSpacing: row.spacing

    signal itemPressed(var mouse, var itemIndex)
    signal itemClicked(var mouse, var itemIndex)
    signal itemDoubleClicked(var mouse, var itemIndex)

    /* MOVE INTO STYLE*/
    Rectangle {
        id: backgroundArea
        anchors.fill: row
        color: palette.midDarkColor
        opacity: 1
        visible: true
    }
    /**/

    Row {
        id: row
        Repeater {
            model: columnCount()

            Item {
                width: columnWidths[index]
                height: childrenRect.height
                clip: true
                MouseArea {
                    width: columnWidths[index]
                    height: row.height
                    acceptedButtons: Qt.RightButton | Qt.LeftButton;

                    onPressed: itemPressed(mouse, index)
                    onClicked: itemClicked(mouse, index)
                    onDoubleClicked: itemDoubleClicked(mouse, index)
                }

                Row {
                    id: columnLayoutRow
                    Loader {
                        id: columnDelegateLoader
                        property var headerData: itemRow.headerData[index]
                        property var headerWidth: columnWidths[index] - x
                        sourceComponent: itemRow.columnDelegates[index]
                    }
                }
            }
        }
    }
}

