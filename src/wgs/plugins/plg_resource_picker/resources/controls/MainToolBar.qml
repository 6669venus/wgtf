import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3


ToolBar {
    id: toolbar

    Layout.fillHeight: false
    Layout.fillWidth: true

    property alias refresh: refresh
    property alias sync: sync
    property alias add: add
    property alias checkin: checkin
    property alias checkout: checkout
    property alias undoCheckout: undoCheckout
    property alias revisionHistory: revisionHistory
    property alias togglePreview: togglePreview
    property alias reportView: reportView
    property alias iconView: iconView
    property alias refreshCurrent: refreshCurrent
    property alias autoRefresh: autoRefresh
    property alias showHidden: showHidden
    property alias searchField: searchField

    RowLayout {
        Layout.fillWidth: false

        ToolButton {
            id: refresh
            text: qsTr("")
            tooltip: "Refresh"
            iconSource: "qrc:/icons/Refresh.png"
        }

        ToolBarSeparator{}

        ToolButton {
            id: sync
            tooltip: qsTr("Sync")
            iconSource: "qrc:/icons/Sync.png"
        }

        ToolBarSeparator{}

        ToolButton {
            id: add
            tooltip: qsTr("Add to Source Control")
            iconSource: "qrc:/icons/AddSourceControl.png"
        }

        ToolButton {
            id: checkin
            tooltip: qsTr("Check In")
            iconSource: "qrc:/icons/CheckIn.png"
        }

        ToolButton {
            id: checkout
            tooltip: qsTr("Check Out")
            iconSource: "qrc:/icons/CheckOut.png"
        }

        ToolButton {
            id: undoCheckout
            tooltip: qsTr("Undo Check Out")
            iconSource: "qrc:/icons/UndoCheckout.png"
        }

        ToolButton {
            id: revisionHistory
            tooltip: qsTr("Revision History")
            iconSource: "qrc:/icons/RevisionHistory.png"
        }

        ToolBarSeparator{}

        ToolButton {
            id: togglePreview
            iconSource: "qrc:/icons/TogglePreview.png"
            checked: true
            checkable: true
            tooltip: qsTr("Toggle Preview")
        }

        ToolBarSeparator{}

        ToolButton {
            id: reportView
            iconSource: "qrc:/icons/ReportView.png"
            checked: true
            checkable: true
            tooltip: qsTr("Report View")
        }

        ToolButton {
            id: iconView
            iconSource: "qrc:/icons/IconView.png"
            checkable: true
            tooltip: qsTr("Icon View")
        }

        ToolBarSeparator{}

        ToolButton {
            id: refreshCurrent
            iconSource: "qrc:/icons/RefreshCurrent.png"
            tooltip: qsTr("Refresh Current Selection with Source Control")
        }

        ToolButton {
            id: autoRefresh
            iconSource: "qrc:/icons/AutoRefresh.png"
            checked: true
            checkable: true
            tooltip: qsTr("Auto Source Control Refresh on Folder Change")
        }

        ToolBarSeparator{}

        ToolButton {
            id: showHidden
            iconSource: "qrc:/icons/ShowHidden.png"
            checked: false
            checkable: true
            tooltip: qsTr("Show Hidden")
        }

        TextField{
            id: searchField
            placeholderText: qsTr("Search...")
            width: filter.width * 2
        }

        ResourceTypeDropDown{
            id: filter
        }
    }
}
