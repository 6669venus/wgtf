import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import QtQuick.Window 2.1
import BWControls 1.0
import WGControls 1.0

/*!
    \brief Timeline Panel

*/
WGPanel {
    id: timelinePanel
    objectName: "TimelinePanel"
    layoutHints: { 'test': 0.1 }
    color: palette.mainWindowColor
    Layout.fillWidth: true
    Layout.fillHeight: true

    property var title: qsTr( "Timeline Panel" )

    property var sourceModel: source

    //get model from SB
    WGTreeModel {
        id: treeModel
        source: root.testTreeModel
        objectName: "TreeExpansionTest"
        ValueExtension {}
        ColumnExtension {}
        ComponentExtension {}
        TreeExtension {
            id: treeModelExtension
            selectionExtension: treeModelSelection
        }
        ThumbnailExtension {}
        SelectionExtension {
            id: treeModelSelection
            multiSelect: true
        }
    }

    WGTreeView {
        id: timelineView
        anchors.top: searchBox.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        spacing: 1
        showColumnsFrame: true
        showColumnHeaders: false
        showColumnFooters: false
        model: treeModel
        selectionExtension: treeModelExtension
        //columnSpacing: 4
        //backgroundColourMode: incrementalGroupBackgroundColours

        columnDelegates: [defaultColumnDelegate, columnDelegate, timelineDelegate]

        Component {
            id: columnDelegate

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: timelineView.minimumRowHeight

                Rectangle {
                    anchors.fill: parent
                    anchors.margins: 1
                    color: {
                        if (typeof itemData.Value === "string")
                        {
                            return "transparent";
                        }

                        var colour = itemData.Value;
                        var r = colour > 9999 ? (colour / 10000) % 100 + 156 : 0;
                        var g = colour > 99 ? (colour / 100) % 100 + 156 : 0;
                        var b = colour % 100 + 156;

                        return Qt.rgba(r / 255, g / 255, b / 255, 1);
                    }
                }

                Text {
                    clip: true
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.margins: 4
                    verticalAlignment: Text.AlignVCenter
                    visible: typeof itemData.Value === "string"
                    text: typeof itemData.Value === "string" ? itemData.Value : ""
                    color: palette.textColor
                }
            }
        }

        Component {
            id: timelineDelegate

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: timelineView.minimumRowHeight

                WGSliderControl {
                    anchors.fill: parent
                }
            }
        }
    }
}
