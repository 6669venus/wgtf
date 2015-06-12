import QtQuick 2.3

//WIP list view, probably needs a much more in depth solution

ListView {
    id: mainList
    anchors.leftMargin: panelProps.standardMargin_
    anchors.rightMargin: panelProps.standardMargin_
    currentIndex: -1

    property bool noFrame_: false

    highlight: WGHighlightFrame {
        visible: mainList.enabled
    }

    WGTextBoxFrame {
        id: listFrame
        visible: !noFrame_
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width + (panelProps.standardMargin_ * 2)
        z: -1
    }
}
