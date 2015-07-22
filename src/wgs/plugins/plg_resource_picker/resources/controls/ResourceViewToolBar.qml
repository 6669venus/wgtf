import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3
import WGControls 1.0

ToolBar {
    id: toolbar

    Layout.fillHeight: false
    Layout.fillWidth: true

    property alias togglePreview: togglePreview
    property alias refresh: refresh
    property alias refreshCurrent: refreshCurrent
    property alias autoRefresh: autoRefresh
    property alias searchField: searchField
    property alias filter: filter

    RowLayout {
        Layout.fillWidth: false

        ToolButton {
            id: togglePreview
            iconSource: "icons/TogglePreview.png"
            iconName: qsTr("")
            checked: true
            checkable: true
            tooltip: qsTr("Toggle Preview")
        }

        ToolBarSeparator{}

        ToolButton {
            id: refresh
            text: qsTr("")
            tooltip: "Refresh"
            iconSource: "icons/Refresh.png"
        }

        ToolButton {
            id: refreshCurrent
            tooltip: qsTr("Current Folder Refresh")
            iconSource: "icons/RefreshCurrent.png"
        }

        ToolButton {
            id: autoRefresh
            tooltip: qsTr("Current Folder Refresh")
            iconSource: "icons/AutoRefresh.png"
        }

        ToolBarSeparator{}

        WGDropDownBox{

            model: ListModel {
                id: model
                ListElement { source: "icons/ReportView.png"; text: "Details" }
                ListElement { source: "icons/IconView.png"; text: "Icons" }
                ListElement { source: "icons/LargeIconView.png"; text: "Large Icons"; }
                ListElement { source: "icons/ExtraLargeIconView.png"; text: "Extra Large Icons"; }
            }
            // Would be nice to have images next to icons in the drop down but not supported by default
            // There may be a way to do this using a custom list view but it isn't straight forward
//            ListView
//            {
//                model: model;
//                delegate: RowLayout{
//                    Image{ source: model.source }
//                    Text{ text: model.text }
//                }
//            }
        }

        ToolBarSeparator{}

        TextField{
            id: searchField
            placeholderText: qsTr("Search...")
            width: 300
        }

        ResourceTypeDropDown{
            id: filter
        }
    }
}
