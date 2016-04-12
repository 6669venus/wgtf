import QtQuick 2.5
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import QtQuick.Window 2.1
import BWControls 1.0
import WGControls 1.0 as WGOne
import WGControls 2.0

/*!
    \brief Timeline Panel WIP.
*/

WGOne.WGPanel {
    id: timelinePanel
    objectName: "TimelinePanel"
    layoutHints: { 'test': 0.1 }
    color: palette.mainWindowColor
    Layout.fillWidth: true
    Layout.fillHeight: true

    property var title: qsTr( "Timeline Panel" )

    ListModel {
        id: barModel

        ListElement {
            name: "Parent Object 1"
            type: "fullBar"
            barColor: "#666666"
            rowSpan: 1
        }
        ListElement {
            name: "Condition 1"
            text: "if (something == 0)"
            type: "textBox"
            rowSpan: 1
        }
        ListElement {
            name: "Property 1"
            type: "barSlider"
            startTime: 0
            endTime: 2
            barColor: "#3FA9F5"
            rowSpan: 1
        }
        ListElement {
            name: "Property 2"
            type: "barSlider"
            startTime: 2
            endTime: 4
            barColor: "#7AC943"
            rowSpan: 1
        }
        ListElement {
            name: "Property 3"
            type: "barSlider"
            startTime: 2
            endTime: 6
            barColor: "#FF931E"
            rowSpan: 1
        }
        ListElement {
            name: "Property 4"
            type: "barSlider"
            startTime: 6
            endTime: 8
            barColor: "#3FA9F5"
            rowSpan: 1
        }
        ListElement {
            name: "Property 5"
            type: "barSlider"
            startTime: 4
            endTime: 6
            barColor: "#7AC943"
            rowSpan: 1
        }
        ListElement {
            name: "Property 6"
            type: "barSlider"
            startTime: 2
            endTime: 4
            barColor: "#FF931E"
            rowSpan: 1
        }
        ListElement {
            name: "Property 7"
            type: "barSlider"
            startTime: 4
            endTime: 10
            barColor: "#3FA9F5"
            rowSpan: 1
        }
    }

    RowLayout {
        anchors.fill: parent

        // This is just a placeholder tree to show that one can be made to match the TimelineView
        Item {
            Layout.preferredWidth: 160
            Layout.fillHeight: true

            Text {
                anchors.bottom: tempListView.top
                anchors.bottomMargin: defaultSpacing.standardMargin
                text: "Placeholder Property Tree"
                color: "white"
                font.bold: true
            }

            ListView {
                id: tempListView
                anchors.fill: parent
                anchors.topMargin: gridCanvas.topMargin
                model: barModel

                spacing: 1

                onContentYChanged: {
                    gridCanvas.setViewYPosition(tempListView.contentY)
                }

                Connections {
                    target: gridCanvas
                    onYPositionChanged: {
                        if (tempListView.contentY != yPos)
                        {
                            tempListView.contentY = yPos
                        }
                    }
                }

                delegate: RowLayout {
                    height: defaultSpacing.minimumRowHeight
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.leftMargin: defaultSpacing.standardMargin
                    anchors.rightMargin: defaultSpacing.standardMargin

                    Rectangle {
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight - defaultSpacing.doubleMargin
                        Layout.preferredWidth: defaultSpacing.minimumRowHeight - defaultSpacing.doubleMargin

                        color: (typeof barColor != "undefined") ? barColor : "transparent"
                        border.width: 1
                        border.color: (typeof barColor != "undefined") ? "white" : "transparent"
                    }

                    Text {
                        Layout.fillWidth: true
                        color: "white"
                        text: name
                    }
                }
            }
        }

        WGTimelineControl {
            id: gridCanvas
            Layout.fillHeight: true
            Layout.fillWidth: true
            focus: true

            model: barModel

            timeScale: 10
        }
    }
}
