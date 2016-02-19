import QtQuick 2.3

Item {
    objectName: "WGScrollBarStyle"
    id: scrollStyleBase

    property Component scrollHandle: Rectangle {
        color: palette.HighlightColor

        border.width: defaultSpacing.standardBorderSize
        border.color: palette.DarkHeaderColor
    }

    property Component scrollGroove: Rectangle {
        color: palette.LighterShade

        border.width: defaultSpacing.standardBorderSize
        border.color: palette.DarkestShade
    }
}
