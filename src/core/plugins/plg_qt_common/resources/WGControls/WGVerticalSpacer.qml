import QtQuick 2.3

//Invisible box that can have height set by number of lineSpaces_

Rectangle {
    anchors {left: parent.left; right: parent.right}
    color: "transparent"

    property int lineSpaces_: 1

    implicitHeight: defaultSpacing.minimumRowHeight * lineSpaces_

    property string label_: ""
}
