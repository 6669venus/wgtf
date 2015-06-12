import QtQuick 2.3
import QtQuick.Layouts 1.1

//A grid layout that left aligns all children on to a single line
//Or two columns if they don't fit.

//Intended for radio buttons and checkboxes with different widths
//Best with 5+ controls otherwise use WGColumnLayout or WGRowLayout

Grid {
    id: gridFrame

    property string label_: ""

    //calculate the total width of all the children objects
    function getWidth(){
        var width = 0;
        for (var i=0; i<children.length - 1; i++){
            width += children[i].implicitWidth;
        }
        return width;
    }

    property int totalWidth_: 0

    anchors {left: parent.left; right: parent.right}

    Component.onCompleted: {

        //set columns to 2 if too wide or unlimited otherwise
        totalWidth_ = getWidth();
        if (totalWidth_ > gridFrame.width){
            gridFrame.columns = 2
            gridFrame.rows = -1
        } else {
            gridFrame.columns = -1
            gridFrame.rows = 1
        }

    }

    //Handle the window being resized
    onWidthChanged: {
        totalWidth_ = getWidth();
        if (totalWidth_ > gridFrame.width){
            gridFrame.columns = 2
            gridFrame.rows = -1
        } else {
            gridFrame.columns = -1
            gridFrame.rows = 1
        }
    }

    flow: GridLayout.TopToBottom

    rowSpacing: panelProps.rowSpacing_
    columnSpacing: panelProps.doubleMargin_
}
