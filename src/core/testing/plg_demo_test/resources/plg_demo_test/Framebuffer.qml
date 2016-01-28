import QtQuick 2.0
import QtCanvas3D 1.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import BWControls 1.0
import WGControls 1.0

import "js/frameBuffer.js" as GLCode
import "js/gl-matrix.js" as GLMatrix

Rectangle {
	id: viewport
	color: palette.MainWindowColor
	property var title: "Viewport"
	property var layoutHints: { 'viewport': 0.1 }

	Item {
		anchors.fill: parent
		visible: true

		Canvas3D {
			id: canvas3d
			anchors.fill: parent
			property double xRotSlider: 0
			property double yRotSlider: 0
			property double zRotSlider: 0
			property double xRotAnim: 0
			property double yRotAnim: 0
			property double zRotAnim: 0
			property bool isRunning: true
			property var positions: []

			// Emitted when one time initializations should happen
			onInitializeGL: {
				GLCode.initializeGL(canvas3d);
			}

			// Emitted each time Canvas3D is ready for a new frame
			onPaintGL: {
				automationUpdate();
				var positions = [];
				var count = getObjectCount();
				for(var i = 0; i < count; i++)
				{
					var objectPosition = getObjectPosition(i);
					var position = GLMatrix.mat3.create();

					position[0] = objectPosition.x;
					position[1] = objectPosition.y;
					position[2] = objectPosition.z;

					positions.push(position);
				}
				var index = rootObjectIndex();
				//console.log("===selected object: " + index + " =====");
				GLCode.setSelectedObject(index);
				GLCode.paintGL(canvas3d, positions);
			}

			onResizeGL: {
				GLCode.resizeGL(canvas3d);
			}

			MouseArea {
				anchors.fill: parent
				onClicked: {
					var index = GLCode.getSelectedObject();
					//console.log("===selected object: " + index + " =====");
					updateRootObject(index);
				}
				onPressed: {
					GLCode.onMouseDown( mouseX, mouseY );
				}
			}
		}
	}
}
