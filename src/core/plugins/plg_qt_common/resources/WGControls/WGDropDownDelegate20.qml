/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Labs Controls module of the Qt Toolkit.
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

import QtQuick 2.6
import Qt.labs.templates 1.0 as T

/*!
 \  brief a reimplementaion of the ItemDelegate in Qt.Labs with a highlight item and updated styling.

*/

T.ItemDelegate {
    id: control

    property QtObject parentControl

    implicitWidth: parentControl.labelMaxWidth + (parentControl.imageRole ? control.height : 0)
                   + (parentControl.showRowIndicator ? indicator.implicitWidth : 0)
                   + control.leftPadding + control.rightPadding
                   + defaultSpacing.doubleMargin
    implicitHeight: Math.max(background ? background.implicitHeight : 0,
                             Math.max(label ? label.implicitHeight : 0,
                                      indicator ? indicator.implicitHeight : 0) + topPadding + bottomPadding)
    baselineOffset: label ? label.y + label.baselineOffset : 0

    topPadding: defaultSpacing.doubleBorderWidth
    leftPadding: defaultSpacing.standardMargin
    rightPadding: defaultSpacing.standardMargin
    bottomPadding: defaultSpacing.doubleBorderWidth

    spacing: defaultSpacing.rowSpacing

    //! [label]
    label: Item {
        id: label
        width: parentControl.labelMaxWidth + (parentControl.imageRole ? delegateImage.width : 0) + (parentControl.showRowIndicator ? indicator.width : 0)
        height: control.availableHeight
        x: control.mirrored ? control.width - width - control.rightPadding : control.leftPadding
        y: control.topPadding

        Image {
            id: delegateImage
            anchors.verticalCenter: parent.verticalCenter
            source: control.image
            visible: control.image
            height: Math.min(sourceSize.height, control.availableHeight)
            width: height
        }

        WGLabel {
            id: delegateLabel
            anchors.left: delegateImage.visible ? delegateImage.right : parent.left
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter

            anchors.leftMargin: delegateImage.visible ? defaultSpacing.standardMargin : 0

            text: control.text
            elide: Text.ElideRight
            visible: control.text
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
        }
    }
    //! [label]

    //! [indicator]
    indicator: Image {
        id: indicator
        x: control.mirrored ? control.leftPadding : control.width - width - control.rightPadding
        y: control.topPadding + (control.availableHeight - height) / 2

        visible: control.checked && parentControl.showRowIndicator
        source: control.checkable ? "qrc:/qt-project.org/imports/Qt/labs/controls/images/check.png" : ""
    }
    //! [indicator]

    //! [background]
    background: Rectangle {
        implicitWidth: parent.width
        implicitHeight: defaultSpacing.minimumRowHeight
        visible: control.pressed || control.highlighted
        color: control.pressed ? palette.mainWindowColor : palette.darkHeaderColor
    }

    //! [background]

    WGHighlightFrame {
        anchors.fill: parent
        anchors.margins: defaultSpacing.standardBorderSize
        visible: hoverArea.containsMouse
        z: -1
    }

    MouseArea {
        id: hoverArea
        anchors.fill: parent
        hoverEnabled: true
        acceptedButtons: Qt.NoButton
    }
}
