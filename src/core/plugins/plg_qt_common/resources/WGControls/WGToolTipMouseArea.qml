import QtQuick 2.3

/*TODO
 This control is currently not working due to limitations with mouse handling
 Cannot pass hovered state to parent so that the tooltip and hovered state highlight are handled
 Document this control during next pass
*/

/*!
 \brief Intended as a replacement for Qt's mouse over tooltip feature,
  WGToolTipMouseArea was designed to  allow for both text and image tooltips.
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


MouseArea{
    objectName: "WGToolTipMouseArea"
    id: mouseOverArea
    propagateComposedEvents: true
    property string parentControlsMouseOverInfo: ""
    property url parentControlsMouseOverImage: ""
    property Item parentControl
    property int parentControlOriginalZDepth
    anchors.fill: parent
    hoverEnabled: true
    WGToolTip {
        id: mouseOver
        mouseOverInfo: parentControl.mouseOverInfo
        mouseOverImage: {
            console.log("WGToolTipMouseArea: parent.iconSource is ", parentControl.iconSource)
            return parentControl.iconSource
        }
    }

    onCanceled: {
        console.log("WGToolTipMouseArea CANCELED: debug ")
        mouse.accepted = false
    }
    onReleased: {
        console.log("WGToolTipMouseArea RELEASED: debug ")
        mouse.accepted = false
    }
    onPressed: {
        console.log("WGToolTipMouseArea PRESSED: debug ")
        // Its not propagating through.....
        //mouseOverArea.hoverEnabled = false
        mouse.accepted = false
    }

    Timer {
        id: tooltipTimer
        interval: 200
        running: false
        onTriggered: {
            mouseOver.visible = true
        }
    }
    onEntered: {
        tooltipTimer.running = true
        // Set the tooltips "parent" element to z:100 temporarilly
        // so that the tooltip draws on top of everything
        mouseOverArea.parentControlOriginalZDepth = mouseOverArea.parentControl.z
        //thumbnailButton.z = 100
        parentControl.z = 100
    }
    onExited: {
        mouseOver.visible = false
        tooltipTimer.running = false  // Is this required?
        mouseOverArea.parentControl.z = mouseOverArea.parentControlOriginalZDepth
    }
}
