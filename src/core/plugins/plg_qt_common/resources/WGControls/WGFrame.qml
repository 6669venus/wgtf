import QtQuick 2.3
import QtQuick.Layouts 1.1

//Stripped back dark or light frame. Not expandable, not toggleable, no title.

/*
    If at lvl 0 recommend:
    dark_: false
    shade_: false

    If at lvl 1 recommend:
    dark_: true
    shade_: false

    These will match WGSubPanel

    Use shade_: true sparingly for a subtle darker or lighter frame to group objects
*/

Rectangle {
    property bool dark_: true
    property bool shade_: false

    property alias color_: frame.color

    property string label_: ""

    property Component childObject_

    color: "transparent"

    height: content.height + defaultSpacing.doubleMargin + (defaultSpacing.topBottomMargin * 2)

    Layout.preferredHeight: content.height + defaultSpacing.doubleMargin + (defaultSpacing.topBottomMargin * 2)

    Layout.fillWidth: true

    Rectangle {
        id: frame
        color: {
            if(shade_){
                if(dark_){
                    palette.DarkShade
                } else {
                    palette.LightShade
                }
            } else {
                if(dark_){
                    palette.MainWindowColor
                } else {
                    palette.LightPanelColor
                }
            }
        }

        radius: defaultSpacing.standardRadius

        height: parent.height - (defaultSpacing.topBottomMargin * 2)

        Layout.preferredHeight: parent.height - (defaultSpacing.topBottomMargin * 2)

        anchors {left: parent.left; right: parent.right}

        anchors.verticalCenter: parent.verticalCenter
    }

    Loader {
        id: content
        anchors {left: parent.left; right: parent.right}

        anchors.leftMargin: defaultSpacing.leftMargin
        anchors.rightMargin: defaultSpacing.rightMargin

        anchors.verticalCenter: parent.verticalCenter

        sourceComponent: childObject_

        z: 1
    }
}

