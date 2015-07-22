import QtQuick 2.4
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.3

ColumnLayout {
    id: tagSearch
    anchors.fill: parent
    spacing: 0

    property alias toolbar: toolbar
    property alias searchEdit : searchEdit
    property bool showSaved: true
    property real savedHeight: 200

    onShowSavedChanged: {
        if(showSaved){
            saved.height = savedHeight
        } else if(saved.height != 0) {
            savedHeight = saved.height
            saved.height = 0
        }
        toolbar.showSaved.checked = showSaved
    }

    TagSearchToolBar {
        id:toolbar
        showSaved.checked: tagSearch.showSaved;
        showSaved.onClicked: {
            tagSearch.showSaved = !tagSearch.showSaved
        }
    }

    TextField{
        id: searchEdit
        placeholderText: qsTr("Search...")
        Layout.fillWidth: true
    }

    SplitView {
        id: hsplitter
        Layout.fillHeight: true
        Layout.fillWidth: true
        orientation: Qt.Vertical
        SplitView {
            id: vsplitter
            Layout.fillHeight: true
            Layout.fillWidth: true
            orientation: Qt.Horizontal
            Rectangle{
                color: "red";
                Text{text: "todo"}
                width: parent.width * .5
            }
            Rectangle{
                color: "blue";
                Text{text: "todo"}
            }
        }
        TableView{
            id: saved
            TableViewColumn {
                role: "name"
                title: "Name"
                width: 100
            }
            TableViewColumn {
                role: "tags"
                title: "Tags"
                width: 300
            }
            onHeightChanged: {
                if(height == 0 && tagSearch.showSaved){
                    tagSearch.showSaved = false
                } else if (height != 0 && !tagSearch.showSaved){
                    tagSearch.savedHeight = height
                    tagSearch.showSaved = true
                }
            }
        }
    }
}

