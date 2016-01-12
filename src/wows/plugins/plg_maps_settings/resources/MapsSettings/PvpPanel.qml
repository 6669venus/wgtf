import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls 1.0

// WGPanel triggers auto-reloading when QML file is saved
WGPanel {
    color: palette.MainWindowColor
    property var title: "PvP Settings"
    property var layoutHints: { 'test': 0.1 }
    property var topControlsHeight: 20

    ListModel {
        id: mapsModel
        ListElement {
            name: "(33) spaces/00_CO_ocean"
            mapThumbnail: "icons/Ocean_Standart_second_5_1.JPG"
            t1: 0
            t2: 0
            t3: 0
            t4: 0
            t5: 0
            t6: 0
            t7: 0
            t8: 0
            t9: 0
            t10: 0
        }
    }

    TableView {
        anchors.fill: parent
        model: mapsModel
        TableViewColumn {
            role: "name"
            title: "Map/Scenario"
            width: 160
        }
        TableViewColumn {
            role: "t1"
            title: "1"
            width: 20
        }
        TableViewColumn {
            role: "t2"
            title: "2"
            width: 20
        }
        TableViewColumn {
            role: "t3"
            title: "3"
            width: 20
        }
        TableViewColumn {
            role: "t4"
            title: "4"
            width: 20
        }
        TableViewColumn {
            role: "t5"
            title: "5"
            width: 20
        }
        TableViewColumn {
            role: "t6"
            title: "6"
            width: 20
        }
        TableViewColumn {
            role: "t7"
            title: "7"
            width: 20
        }
        TableViewColumn {
            role: "t8"
            title: "8"
            width: 20
        }
        TableViewColumn {
            role: "t9"
            title: "9"
            width: 20
        }
        TableViewColumn {
            role: "t10"
            title: "10"
            width: 20
        }
    }
}
