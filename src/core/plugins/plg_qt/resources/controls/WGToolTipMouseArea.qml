import QtQuick 2.0

//WIP

// THIS CURRENTLY ISNT WORKING DUE TO LIMITATIONS WITH MOUSEHANDLING
// REQUIRES ABILITY TO PASS HOVERED STATE TO PARENT SO THAT THE TOOLTIP
// AND THE HOVERED STATE HIGHLIGHT ARE HANDLED

/*
How to use:
Add the following property to your control at the root level
property string mouseOverInfo: ""
If you want an image in the mouse over optionally add
property url mouseOverImage: ""
Then add the following custom version of MouseArea to where you would like the tooltip to activate
WGToolTipMouseArea{
    parentControl: (id_of_parent_control)
    parentControlsMouseOverInfo: (id_of_parent_control).mouseOverInfo
}
In the main panel where instaces of the control are called add the unique mouseover info
WGPushButton {
    width: 150
    checkable: true
    text: "Toggle Button"
    mouseOverInfo: "This is a toggle button, do with it what you will"
    mouseOverImage: "example/url.png"
}
*/

MouseArea{
    // Parts of this need to be pulled out and made generic as a mouse over that can
    // be used everywhere WGToolTipMouseArea, will then call WGToolTip
    id: mouseOverArea
    propagateComposedEvents: true
    // The Store the original z depth of the control in original so it can be reset after onExit
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
