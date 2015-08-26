import QtQuick 2.3

/*!
 \brief Generic rounded 'recessed' frame that can get focus.
 Typically used to frame text or a number of controls.

Example:
\code{.js}
WGTextBoxFrame {
    Layout.fillWidth: true
    Layout.preferredHeight: 100
    clip: true

    Rectangle {
        color: "red"
        width: 10
        anchors.top: parent.top
        anchors.bottom: parent.bottom
    }
}
\endcode
*/

Rectangle {
    objectName: "WGTextBoxFrame"
    color: palette.TextBoxColor
    border.width: defaultSpacing.standardBorderSize
    border.color: palette.DarkestShade
    radius: defaultSpacing.halfRadius
}
