import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import BWControls 1.0
import WGControls 1.0



ColumnLayout {
	property var title: "Curve Editor"
	property var layoutHints: { 'curveeditor': 1.0, 'bottom': 0.5 }
    spacing: 0
    Rectangle {
        id: bottom
        color: palette.MainWindowColor
        Layout.fillHeight: true
        Layout.fillWidth: true


        Canvas {
			id: canvas
            anchors.fill: parent
            contextType: "2d"

            Path {
                id: red
                startX: 0; startY: canvas.height

                PathCurve { x: 0; y: canvas.height }
                PathCurve { x: canvas.width * 0.4; y: canvas.height - (canvas.height * 0.8) }
                PathCurve { x: canvas.width; y: 2 }
            }

            Path {
                id: green
                startX: 0; startY: canvas.height

                PathCurve { x: 0; y: canvas.height }
                PathCurve { x: canvas.width; y: 2 }
            }

            Path {
                id: blue
                startX: 0; startY: canvas.height

                PathCurve { x: 0; y: canvas.height }
                PathCurve { x: canvas.width * 0.4; y: canvas.height - (canvas.height * 0.2) }
                PathCurve { x: canvas.width; y: 2 }
            }

            onPaint: {
				if(available)
				{
					context.clearRect(0, 2, canvas.width, canvas.height);

					context.fillStyle = "#000000";
					context.fillRect(0, 0, canvas.width, canvas.height);

					context.lineWidth = 4.0;
					context.strokeStyle = Qt.rgba(1.0, 0.41, 0.36, 0.3);
					context.path = red;
					context.stroke();

					context.lineWidth = 1.0;
					context.strokeStyle = Qt.rgba(1.0, 0.41, 0.36, 1.0);
					context.path = red;
					context.stroke();

					context.lineWidth = 4.0;
					context.strokeStyle = Qt.rgba(0.49, 0.72, 0.38, 0.3);
					context.path = green;
					context.stroke();

					context.lineWidth = 1.0;
					context.strokeStyle = Qt.rgba(0.49, 0.72, 0.38, 1.0);
					context.path = green;
					context.stroke();

					context.lineWidth = 4.0;
					context.strokeStyle = Qt.rgba(0.54, 0.78, 1.0, 0.3);
					context.path = blue;
					context.stroke();

					context.lineWidth = 1.0;
					context.strokeStyle = Qt.rgba(0.54, 0.78, 1.0, 1.0);
					context.path = blue;
					context.stroke();
				}
            }
        }
    }
}
