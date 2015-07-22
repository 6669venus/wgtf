import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3

ToolBar {
    id: toolbar

    Layout.fillHeight: false
    Layout.fillWidth: true

    property alias search: search
    property alias reset: reset
    property alias back: back
    property alias forward: forward
    property alias filter: filter
    property alias wordComplete: wordComplete
    property alias save: save
    property alias showSaved: showSaved

    RowLayout {
        Layout.fillWidth: false

        ToolButton {
            id: search
            tooltip: qsTr("Search")
            iconSource: "qrc:/icons/Search.png"
        }

        ToolButton {
            id: reset
            tooltip: "Reset"
            iconSource: "qrc:/icons/Reset.png"
        }

        ToolButton {
            id: back
            tooltip: qsTr("Back")
            iconSource: "qrc:/icons/Back.png"
        }

        ToolButton {
            id: forward
            tooltip: qsTr("Forward")
            iconSource: "qrc:/icons/Forward.png"
        }

        ResourceTypeDropDown{
            id: filter
        }

        ToolBarSeparator{}

        ToolButton {
            id: wordComplete
            checkable: true
            checked: true
            tooltip: qsTr("Word Complete")
            iconSource: "qrc:/icons/WordComplete.png"
        }

        ToolBarSeparator{}

        ToolButton {
            id: save
            tooltip: qsTr("Save")
            iconSource: "qrc:/icons/Save.png"
        }

        ToolButton {
            id: showSaved
            checkable: true
            checked: false
            tooltip: qsTr("Show Saved")
            iconSource: "qrc:/icons/ShowSaved.png"
        }
    }
}
