import QtQuick 2.3

/*!
    \brief A WG styled frame for buttons with mouseovers, pressed states etc.
*/

Rectangle {
    objectName: "WGButtonFrame"

    /*! This property defines the color of the inner border
        The default value is \c palette.LighterShade
    */
    //TODO: This should be renamed, it does not require "_"
    property color innerBorderColor_: palette.LighterShade

    /*! This property defines the color of a highlight overlay
        The default value is \c "transparent"
    */
    //TODO: This should be renamed, it does not require "_"
    property color highlightColor_: "transparent"

    /*! This property defines the color of the border
        The default value is \c palette.DarkerShade
    */
    //TODO: This should be renamed, it does not require "_"
    property color borderColor_: palette.DarkerShade


    /*! This property is used to hold the hovered state of the control
        The default value is \c false
    */
    property bool hovered_: false


    /*! This property toggles the visibility of the control
        The default value is \c false
    */
    property bool noFrame_: false

    /*  TODO: Changing this doesnt always give a desirable effect.
        Is it necessary to have both radius_ and radius in this control?
    */
    /*! This property defines the radius of the control
        The default value is \c defaultSpacing.standardRadius
    */
    property int radius_: defaultSpacing.standardRadius

    id: outerFrame
    visible: noFrame_ ? false : true
    color: palette.LightShade

    border.width: defaultSpacing.standardBorderSize
    border.color: borderColor_

    //TODO: Changing this doesnt always give a desirable effect
    radius: radius_

    //inner border to make it look raised and different to a text box.
    Rectangle {
        id: innerFrame
        anchors.fill: parent
        anchors.margins: defaultSpacing.standardBorderSize
        radius: radius_ / 2

        border.width: defaultSpacing.standardBorderSize
        border.color: innerBorderColor_

        color: "transparent"
    }

    Rectangle {
        id: pressHighlight
        anchors.fill: parent
        anchors.margins: defaultSpacing.doubleBorderSize
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

        anchors.margins: defaultSpacing.standardBorderSize

        gradient: Gradient {
                 GradientStop { position: 0.0; color: palette.GlowStyle ? "transparent" : palette.LighterShade}
                 GradientStop { position: 1.0; color: palette.GlowStyle ? palette.HighlightShade : palette.LighterShade}
             }
    }

    Rectangle {
        id: mouseFlare
        height: parent.width
        width: defaultSpacing.standardBorderSize

        visible: hovered_ && palette.GlowStyle

        radius: 10

        anchors.centerIn: parent
        anchors.verticalCenterOffset: parent.height / 2 - defaultSpacing.doubleBorderSize

        rotation: 90

        gradient: Gradient {
                 GradientStop { position: 0.0; color: "transparent" }
                 GradientStop { position: 0.5; color: palette.HighlightColor}
                 GradientStop { position: 1.0; color: "transparent" }
             }
    }
}
