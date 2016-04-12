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
            startTime: 0
            endTime: 2
            barColor: "#3FA9F5"
            rowSpan: 1
        }
        ListElement {
            startTime: 2
            endTime: 4
            barColor: "#7AC943"
            rowSpan: 1
        }
        ListElement {
            startTime: 2
            endTime: 6
            barColor: "#FF931E"
            rowSpan: 1
        }
        ListElement {
            startTime: 6
            endTime: 8
            barColor: "#3FA9F5"
            rowSpan: 1
        }
        ListElement {
            startTime: 4
            endTime: 6
            barColor: "#7AC943"
            rowSpan: 1
        }
        ListElement {
            startTime: 2
            endTime: 4
            barColor: "#FF931E"
            rowSpan: 1
        }
        ListElement {
            startTime: 4
            endTime: 10
            barColor: "#3FA9F5"
            rowSpan: 1
        }
    }

    WGTimelineControl {
        id: gridCanvas
        anchors.fill: parent
        focus: true

        model: barModel
    }
}
