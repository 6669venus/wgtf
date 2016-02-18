import QtQuick 2.3

Item {
    objectName: "WGScrollBarStyle"
    id: scrollStyleBase

    property Component scrollHandle: Rectangle {
        color: palette.DarkColor

        border.width: defaultSpacing.standardBorderSize
        border.color: palette.HighlightColor
    }

    property Component scrollGroove: Rectangle {
        color: palette.LighterShade

        border.width: defaultSpacing.standardBorderSize
        border.color: palette.DarkestShade
    }
}
