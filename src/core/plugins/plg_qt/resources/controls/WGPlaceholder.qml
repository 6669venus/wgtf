import QtQuick 2.3

//Empty box with placeholder text.

Rectangle {
    color: "transparent"
    border.width: panelProps.standardBorder_
    border.color: palette.LighterShade

    property alias text: placeholderText.text

    WGMultiLineText{
        id: placeholderText
        anchors.fill: parent

        font.pointSize: 12

        anchors.topMargin: parent.height/3
        anchors.bottomMargin: parent.height/3
        anchors.leftMargin: parent.width/3
        anchors.rightMargin: parent.width/3

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
