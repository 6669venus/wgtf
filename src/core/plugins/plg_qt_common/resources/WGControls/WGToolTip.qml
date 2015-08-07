import QtQuick 2.3

//WIP

Rectangle {
    property string mouseOverInfo: ""
    property url mouseOverImage: ""
    id: mouseOver_
    visible: false
    layer.enabled: true

    // TODO This shouldn't be positioned as shown.
    // Ideally it will get created over the 3dviewport, away from the UI
    // Obscruing the UI with tooltips is BAD!
    // See Zbrush mouse over for example
    // Will probably be done by looking for the furthest parent node (G1)
    // Find G1's width, then offset using that witdth
    // G1 will have to have clipping off
    x: parent.width + 20
    radius: defaultSpacing.standardRadius
    border.width: defaultSpacing.standardBorderSize
    border.color: palette.HighlightTextColor  //must be vec3 needs to be more subtle but have no options in palette
    //opacity: 0.8
    color: palette.DarkestShade
    width: drawMouseOverImage.paintedWidth + drawLayout.anchors.leftMargin + drawLayout.anchors.rightMargin
    height: drawMouseOverImage.paintedHeight + drawMouseOverInfo.contentHeight + drawLayout.anchors.topMargin + drawLayout.anchors.bottomMargin
    WGColumnLayout {
        id: drawLayout

        anchors { //TODO, needs its own constants
            left: parent.left; right: parent.right;
            top: parent.top; bottom: parent.bottom
            topMargin: defaultSpacing.doubleMargin;
            bottomMargin: defaultSpacing.doubleMargin;
            leftMargin: defaultSpacing.doubleMargin;
            rightMargin: defaultSpacing.doubleMargin;
        }
        Image{
            id: drawMouseOverImage
            source: mouseOver_.mouseOverImage
            visible: (mouseOver_.mouseOverImage != "") ? true : false
            width: 170
            height: 170
            onVisibleChanged: {
                console.log("WGToolTip: mouseOver_.mouseOverImage is ", mouseOver_.mouseOverImage)
            }
        }
        Text {
            id: drawMouseOverInfo
            text: mouseOver_.mouseOverInfo
            width: parent.width
            wrapMode: Text.Wrap
            color: palette.HighlightTextColor
        }
    }
}
