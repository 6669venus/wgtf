import QtQuick 2.0
import QtQuick.Layouts 1.0

//Two tone vertical or horizontal separator

Rectangle {
    property bool vertical_: false

    width: vertical_ ? panelProps.separatorWidth_ : undefined
    height: vertical_ ? undefined : panelProps.separatorWidth_

    Layout.fillWidth: vertical_ ? false : true
    Layout.fillHeight: vertical_ ? true : false

    Layout.maximumWidth: vertical_ ? panelProps.separatorWidth_ : Number.POSITIVE_INFINITY
    Layout.maximumHeight: vertical_ ? Number.POSITIVE_INFINITY : panelProps.separatorWidth_

    color: "transparent"

    Rectangle {
        width: vertical_ ? panelProps.separatorWidth_ / 2 : parent.width
        height: vertical_ ? parent.height : panelProps.separatorWidth_ / 2
        color: palette.DarkerShade
        anchors.left: parent.left
        anchors.top: parent.top
    }

    Rectangle {
        width: vertical_ ? panelProps.separatorWidth_ / 2 : parent.width
        height: vertical_ ? parent.height : panelProps.separatorWidth_ / 2
        color: palette.LighterShade
        anchors.right: parent.right
        anchors.bottom: parent.bottom
    }
}
