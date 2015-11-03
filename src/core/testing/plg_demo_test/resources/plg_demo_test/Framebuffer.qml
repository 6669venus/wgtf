/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtCanvas3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.0
import QtCanvas3D 1.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import BWControls 1.0
import WGControls 1.

import "js/frameBuffer.js" as GLCode
import "js/gl-matrix.js" as GLMatrix

Item {
    id: mainview
    width: 800
    height: 600
    visible: true

    property var title: "QML Controls Test Window"

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
		property int count: 0
		property var positions: []

        // Emitted when one time initializations should happen
        onInitializeGL: {
			count = getObjectCount();
            GLCode.initializeGL(canvas3d);
        }

        // Emitted each time Canvas3D is ready for a new frame
        onPaintGL: {
			var positions = [];
			for(var i = 0; i < count; i++)
			{
				var objectPosition = getObjectPosition(i);
				var position = GLMatrix.mat3.create();

				position[0] = objectPosition.x;
				position[1] = objectPosition.y;
				position[2] = objectPosition.z;

				positions.push(position);
			}
            GLCode.paintGL(canvas3d, positions);
        }

        onResizeGL: {
            GLCode.resizeGL(canvas3d);
        }

		MouseArea {
			 anchors.fill: parent
			 onClicked: {
				var index = GLCode.getSelectedObject();
				console.log("===selected object: " + index + " =====");
			 }
			 onPressed: {
				GLCode.onMouseDown( mouseX, mouseY );
			 }
		}
    }
}