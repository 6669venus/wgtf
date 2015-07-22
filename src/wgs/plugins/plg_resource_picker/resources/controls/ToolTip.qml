import QtQuick 2.0

Rectangle {
    id:tooltip

    property string text: ""
    property Rectangle display : Rectangle{
        implicitHeight: rect.height
        Rectangle{
            id: rect
            color: palette.ToolTipColor
            width: displayText.width + 8
            height: displayText.height + 4
        }
        Text{
            id: displayText
            text: tooltip.text
            color: palette.ToolTipTextColor
            anchors.horizontalCenter: rect.horizontalCenter
            anchors.verticalCenter: rect.verticalCenter
        }
    }
    function root()
    {
        var parent = tooltip
        while(parent && parent.parent)
            parent = parent.parent;
        return parent;
    }

    function showToolTip()
    {
        var ro = root();
        display.parent = ro
        positionToolTip()
    }

    function hideToolTip()
    {
        display.visible = false
    }

    function positionToolTip()
    {
        var xy = mouseArea.mapToItem(
                    display.parent,
                    mouseArea.mouseX,
                    mouseArea.mouseY + display.height)
        display.x = xy.x
        display.y = xy.y
        display.visible = true
    }

    color: "transparent"

    Timer {
        id: timer
        interval: 500;
        onTriggered: showToolTip();
    }
    MouseArea{
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {
            timer.start()
        }
        onPositionChanged: {
            if(display.visible){
               positionToolTip()
            }
        }
        onExited: {
            hideToolTip();
            timer.stop();
        }
    }
}

