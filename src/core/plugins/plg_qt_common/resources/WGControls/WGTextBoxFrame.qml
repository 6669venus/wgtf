import QtQuick 2.3

//Generic rounded 'recessed' frame that can get focus. Text boxes etc.

Rectangle {
    color: palette.TextBoxColor

    border.width: defaultSpacing.standardBorderSize
    border.color: palette.DarkestShade
    radius: defaultSpacing.halfRadius
}
