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
            barColor: "red"
            rowSpan: 1
        }
        ListElement {
            startTime: 2
            endTime: 4
            barColor: "blue"
            rowSpan: 1
        }
        ListElement {
            startTime: 2
            endTime: 6
            barColor: "green"
            rowSpan: 1
        }
        ListElement {
            startTime: 6
            endTime: 8
            barColor: "yellow"
            rowSpan: 1
        }
        ListElement {
            startTime: 4
            endTime: 6
            barColor: "aqua"
            rowSpan: 1
        }
        ListElement {
            startTime: 2
            endTime: 4
            barColor: "orange"
            rowSpan: 1
        }
        ListElement {
            startTime: 4
            endTime: 10
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

        timeScale: 10

        horizontalPixelGap: (gridCanvas.width / totalFrames) * framesPerSecond

        onWidthChanged: {
            console.log(gridCanvas.width + " / " + totalFrames + " * " + framesPerSecond)
            console.log(horizontalPixelGap)
        }

        property int framesPerSecond: 30
        property int totalFrames: (framesPerSecond * timeScale)
        property real frameWidth: canvasWidth / totalFrames

        function pixelsToFrames(pixels) {
            return Math.round(pixels / frameWidth)
        }

        function framesToPixels(frames) {
            return frames * frameWidth
        }


        ListView {
            id: timelineView
            model: barModel

            width: gridCanvas.canvasWidth
            height: gridCanvas.canvasHeight

            x: gridCanvas.viewTransform.transformX(0)
            y: gridCanvas.viewTransform.transformY(1)

            interactive: false


            delegate: WGTimelineBarSlider {
                id: slider

                height: defaultSpacing.minimumRowHeight
                anchors.left: parent.left
                anchors.right: parent.right
                minimumValue: 0
                maximumValue: gridCanvas.totalFrames
                stepSize: 1
                startFrame: startTime * gridCanvas.framesPerSecond
                endFrame: endTime * gridCanvas.framesPerSecond
                barColor: model.barColor
            }
        }
   }
}
