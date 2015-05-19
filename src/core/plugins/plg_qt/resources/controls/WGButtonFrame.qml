import QtQuick 2.0

//Raised style for the rectangle of buttons, with mouseovers, pressed states etc.
Rectangle {
    property color innerBorderColor_: palette.LighterShade
    property color highlightColor_: "transparent"
    property color borderColor_: palette.DarkerShade

    property bool hovered_: false

    property bool noFrame_: false
    property int radius_: panelProps.standardRadius_

    id: outerFrame
    visible: noFrame_ ? false : true
    color: palette.LightShade

    border.width: panelProps.standardBorder_
    border.color: borderColor_

    radius: radius_

    //inner border to make it look raised and different to a text box.
    Rectangle {
        id: innerFrame
        anchors.fill: parent
        anchors.margins: panelProps.standardBorder_
        radius: radius_ / 2

        border.width: panelProps.standardBorder_
        border.color: innerBorderColor_

        color: "transparent"
    }

    Rectangle {
        id: pressHighlight
        anchors.fill: parent
        anchors.margins: panelProps.doubleBorder_
        radius: radius_ / 2

        color: highlightColor_
    }

    //this only shows up in glow styles.
    Rectangle {
        id: mouseGlow
        anchors.fill: parent
        radius: radius_

        color: palette.LightShade

        visible: hovered_ && palette.GlowStyle

        anchors.margins: panelProps.standardBorder_

        gradient: Gradient {
                 GradientStop { position: 0.0; color: palette.GlowStyle ? "transparent" : palette.LighterShade}
                 GradientStop { position: 1.0; color: palette.GlowStyle ? palette.HighlightShade : palette.LighterShade}
             }
    }

    Rectangle {
        id: mouseFlare
        height: parent.width
        width: panelProps.standardBorder_

        visible: hovered_ && palette.GlowStyle

        radius: 10

        anchors.centerIn: parent
        anchors.verticalCenterOffset: parent.height / 2 - panelProps.doubleBorder_

        rotation: 90

        gradient: Gradient {
                 GradientStop { position: 0.0; color: "transparent" }
                 GradientStop { position: 0.5; color: palette.HighlightColor}
                 GradientStop { position: 1.0; color: "transparent" }
             }
    }
}
