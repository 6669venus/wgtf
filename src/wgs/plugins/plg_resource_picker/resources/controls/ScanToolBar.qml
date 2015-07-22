import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3

ToolBar {
    id: toolbar

    Layout.fillHeight: false
    Layout.fillWidth: true

    property alias runScan: runScan
    property alias refreshScripts: refreshScripts

    RowLayout {
        Layout.fillWidth: false

        ToolButton {
            id: runScan
            tooltip: qsTr("Run Scan")
            iconSource: "qrc:/icons/RunScan.png"
        }

        ToolButton {
            id: refreshScripts
            tooltip: "Refresh Scripts"
            iconSource: "qrc:/icons/RefreshScripts.png"
        }
    }
}
