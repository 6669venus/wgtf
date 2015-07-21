import QtQuick 2.3
import QtQuick.Layouts 1.1

//Two tone vertical or horizontal separator

Rectangle {
    property bool vertical_: false

    width: vertical_ ? defaultSpacing.separatorWidth : undefined
    height: vertical_ ? undefined : defaultSpacing.separatorWidth

    Layout.fillWidth: vertical_ ? false : true
    Layout.fillHeight: vertical_ ? true : false

    Layout.maximumWidth: vertical_ ? defaultSpacing.separatorWidth : Number.POSITIVE_INFINITY
    Layout.maximumHeight: vertical_ ? Number.POSITIVE_INFINITY : defaultSpacing.separatorWidth

    color: "transparent"

    Rectangle {
        width: vertical_ ? defaultSpacing.separatorWidth / 2 : parent.width
        height: vertical_ ? parent.height : defaultSpacing.separatorWidth / 2
        color: palette.DarkerShade
        anchors.left: parent.left
        anchors.top: parent.top
    }

    Rectangle {
        width: vertical_ ? defaultSpacing.separatorWidth / 2 : parent.width
        height: vertical_ ? parent.height : defaultSpacing.separatorWidth / 2
        color: palette.LighterShade
        anchors.right: parent.right
        anchors.bottom: parent.bottom
    }
}
