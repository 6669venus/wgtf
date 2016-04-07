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

    ListModel {
        id: barModel

        ListElement {
            startTime: 0
            endTime: 0.2
            barColor: "red"
            rowSpan: 1
        }
        ListElement {
            startTime: 0.2
            endTime: 0.4
            barColor: "blue"
            rowSpan: 1
        }
        ListElement {
            startTime: 0.2
            endTime: 0.6
            barColor: "green"
            rowSpan: 1
        }
        ListElement {
            startTime: 0.6
            endTime: 0.8
            barColor: "yellow"
            rowSpan: 1
        }
        ListElement {
            startTime: 0.4
            endTime: 0.6
            barColor: "aqua"
            rowSpan: 1
        }
        ListElement {
            startTime: 0.2
            endTime: 0.4
            barColor: "orange"
            rowSpan: 1
        }
        ListElement {
            startTime: 0.4
            endTime: 1.0
            barColor: "purple"
            rowSpan: 1
        }
    }

    WGGridCanvas {
        id: gridCanvas
        anchors.fill: parent
        focus: true
        useAxis: xGrid
        showMouseLine: true
        showXGridLines: true
        showYGridLines: false

        timeScale: 100

        property real totalWidth: Math.abs(gridCanvas.viewTransform.transformX(0) - gridCanvas.viewTransform.transformX(1))
        property real totalHeight: Math.abs(gridCanvas.viewTransform.transformY(0) - gridCanvas.viewTransform.transformY(1))

        ListView {
            model: barModel

            width: gridCanvas.totalWidth
            height: gridCanvas.totalHeight
            x: gridCanvas.viewTransform.transformX(0)
            y: gridCanvas.viewTransform.transformY(1)

            delegate: Item {
                height: defaultSpacing.minimumRowHeight
                width: gridCanvas.totalWidth
                Rectangle {
                    color: barColor
                    x: gridCanvas.totalWidth * startTime
                    width: (gridCanvas.totalWidth * endTime) - (gridCanvas.totalWidth * startTime)
                    height: defaultSpacing.minimumRowHeight
                }
            }
        }
   }
}
