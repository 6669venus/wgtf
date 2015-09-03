import QtQuick 2.3

/*TODO
 When used in conjuction with WGToolTipMouseArea, this control is currently not working due to limitations with mouse handling
 Cannot pass hovered state to parent so that the tooltip and hovered state highlight are handled
 Document this control during next pass
*/

/*!
 \brief Intended as a replacement for Qt's mouse over tooltip feature,
  WGToolTip is called by WGToolTipMouseArea and was designed to allow for both text and image tooltips.
  This control is currently not working.

Intended use:
Add the following property to your control at the root level
property string mouseOverInfo: ""
If you want an image in the mouse over optionally add
property url mouseOverImage: ""
Then add the following custom version of MouseArea to where you would like the tooltip to activate
\code{.js}
WGToolTipMouseArea{
    parentControl: (id_of_parent_control)
    parentControlsMouseOverInfo: (id_of_parent_control).mouseOverInfo
}
\endcode

Wherever instances of the control are called add unique mouseover information
\code{.js}
WGPushButton {
    width: 150
    checkable: true
    text: "Toggle Button"
    mouseOverInfo: "This is a toggle button, do with it what you will"
    mouseOverImage: "example/url.png"
}
\endcode
*/

Rectangle {
    property string mouseOverInfo: ""
    property url mouseOverImage: ""
    id: mouseOver_
    visible: false
    layer.enabled: true

    // TODO: This shouldn't be positioned as shown.
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
