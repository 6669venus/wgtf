import QtQuick 2.3

Item {
    objectName: "WGScrollBarStyle"
    id: scrollStyleBase

    property Component scrollHandle: Rectangle {
        color: palette.highlightColor

        border.width: defaultSpacing.standardBorderSize
        border.color: palette.darkHeaderColor
    }

    property Component scrollGroove: Rectangle {
        color: palette.lighterShade

        border.width: defaultSpacing.standardBorderSize
        border.color: palette.darkestShade
    }
}
