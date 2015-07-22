import QtQuick 2.4
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.3

ColumnLayout {
    anchors.fill: parent
    spacing: 0

    ScanToolBar{
        id: toolbar
        Layout.fillWidth: true
    }

    TableView{
        id:table
        Layout.fillHeight: true
        Layout.fillWidth: true
        TableViewColumn{
            role: "script"
            title: "Script"
            width:200
        }
    }
}
