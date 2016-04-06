import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import QtQuick.Window 2.1
import BWControls 1.0
import WGControls 1.0

/*!
    \brief Timeline Panel WIP.
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

    // Awaiting dummy model
    WGTreeModel {
        id: treeModel
        source: timelinePanel.sourceModel
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

    //The GridCanvas will draw behind the TreeView
    WGGridCanvas {
        id: keyframeGrid
        objectName: "keyframeGrid"
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        useAxis: xGrid
        showYGridLines: false
        //x: //The position of the grids left should be ~2xtreeview.columnWidth + 1stcolumnWidth + secondcolumnWidth

        //Current Time Handle
        Rectangle {
            id: currentTimeHandle
            objectName: "currentTimeHandle"
            property real currentTime: 0
            x: currentTime
            width: 7
            y: 0
            height: parent.height
            color: "yellow"

            MouseArea {
                id: currentTimeHandleMouseArea
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width
                height: parent.height
                cursorShape: Qt.SplitHCursor

                drag.target: currentTimeHandle
                drag.threshold: 0
                drag.axis: Drag.XAxis
                //drag.minimumX: index === 0 ? firstColumnIndentation + minimumColumnSize : minimumColumnSize
                //drag.maximumX: resizable ? maximumColumnSize : columns.calculateMaxColumnSize(column.x, index)

                onPositionChanged: {
                    console.log("timelineHandle dragged")
                }
            }

            Rectangle {
                id: innerShade
                color: "white"
                width: 1
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.centerIn: parent
            }
        }
    }

    WGTreeView {
        id: timelineView
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        spacing: 1
        // Set the columnsFrame invisible to allow the gridView visibility
        showColumnsFrame: false //true
        showColumnHeaders: false
        showColumnFooters: false
        model: treeModel
        selectionExtension: treeModelExtension
        //columnSpacing: 4
        //backgroundColourMode: incrementalGroupBackgroundColours

        columnDelegates: [defaultColumnDelegate, propertyDelegate, keyframeColumnDelegate]

        property Component propertyDelegate: Loader {
            clip: true
            sourceComponent: itemData != null ? itemData.Component : null
        }

        Component {
            id: keyframeColumnDelegate

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: timelineView.minimumRowHeight

                //to be replaced by WGKeyframeControl
                WGSliderControl {
                    anchors.fill: parent
                }
            }
        }
    }
}
