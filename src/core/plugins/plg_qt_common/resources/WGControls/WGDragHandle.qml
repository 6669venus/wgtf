import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

//A draggable handle behind objects in a WGColumnLayout
//DO NOT USE BY ITSELF!

Rectangle {
    property int index: 0

    //parent WGDraggableColumn
    property QtObject dragLayout

    //is this the handle for the fake last space?
    property bool lastSpace: (typeof parent.lineSpaces_ != "undefined")

    id: dragRect
    width: dragLayout.width + defaultSpacing.leftMargin + defaultSpacing.rightMargin
    height: parent.height
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.bottom: parent.bottom
    color: "transparent"

    //check that the parent doesn't change height for some other reason eg. panel expanding
    Connections {
        target: parent
        onHeightChanged: {
            height = parent.height
        }
    }

    //visual bar that appears above the space the object is being dragged to.
    Rectangle {
        width: dragLayout.width
        height: dropArea.hovered ? defaultSpacing.rowSpacing : 0
        anchors.bottom: parent.top
        z: 10
        anchors.horizontalCenter: parent.horizontalCenter
        color: palette.HighlightColor

        visible: {
            if(dropArea.hovered){
                if (dragRect.parent.Layout.row != (dragLayout.draggedObject.parent.Layout.row + 1)){
                    true
                } else {
                    false
                }
            } else {
                true
            }
        }

    }

    //mouse area in the left margin to begin dragging
    MouseArea {
        id: mouseArea

        anchors.top: dragRect.top
        anchors.bottom: dragRect.bottom
        anchors.left: dragRect.left

        hoverEnabled: true

        width: defaultSpacing.leftMargin

        drag.target: dragRect
        enabled: !lastSpace && dragLayout.unLocked_

        //because the dragged object contains a dragtarget, need to give it a key
        Component.onCompleted: {
            dragRect.Drag.keys.push(index)
        }

        onPressed: {
            parent.Drag.hotSpot.x = mouse.x
            parent.Drag.hotSpot.y = mouse.y
        }

        //activate the drag, pass all the heights etc. and dim the original object
        drag.onActiveChanged: {
            if (mouseArea.drag.active) {
                dragLayout.dragItemIndex = dragRect.parent.Layout.row;
                dragLayout.draggedObject = dragRect;
                dragRect.parent.opacity = 0.5
            } else {
                dragLayout.dragItemIndex = -1;
                dragRect.parent.opacity = 1
            }

            dragRect.Drag.drop();
        }
        //blue bar that appears when mouseovered in the left margin
        Rectangle {
            anchors.fill: parent
            anchors.margins: defaultSpacing.rowSpacing

            color: mouseArea.containsMouse || mouseArea.drag.active ? palette.HighlightShade : "transparent"
        }
    }

    //state changes for when drag is active
    states: [
        State {
            when: dragRect.Drag.active

            PropertyChanges {
                target: dragRect
                color: palette.LighterShade
            }

            AnchorChanges {
                target: dragRect
                anchors.bottom: undefined
            }
        }
    ]

    Drag.active: mouseArea.drag.active

    //the target for a dragged object
    DropArea{
        id: dropArea
        anchors.fill: parent
        property bool hovered: false

        //create a list of keys for all potential drag objects EXCEPT it's own.
        Component.onCompleted: {
            for (var i=0; i < dragLayout.children.length; i++){
                if (i != index){
                    dropArea.keys.push(i)
                }
            }
        }

        //if something else is hovering over this dragTarget, pass the necessary details to dragLayout
        onHoveredChanged: {
            if(hovered){
                dragLayout.targetObject = dragRect.parent
                dragLayout.dropTarget = dragRect.parent.Layout.row
            }
        }

        //update hovered status
        onEntered: {
            hovered = true
        }

        onExited: {
            hovered = false
        }

        //Shuffle everything into place
        onDropped: {
            dragRect.height = dragRect.parent.height

            //if child is not the dragged object, move it into its own column and down a row so we don't create space occupied warnings
            for(var i = 0; i < dragLayout.children.length; i++){
                if (dragLayout.children[i].Layout.row != dragLayout.dragItemIndex && dragLayout.children[i].Layout.row >= dragLayout.dropTarget){
                    dragLayout.children[i].Layout.column = 1+i
                    dragLayout.children[i].Layout.row += 1
                }
            }

            //move the dragged object into the correct row
            drag.source.parent.Layout.row = dragLayout.dropTarget

            //shuffle all the other objects back into the first column
            for(var j = 0; j < dragLayout.children.length; j++){
                if(dragLayout.children[j].Layout.column > 0){
                    dragLayout.children[j].Layout.column = 0
                }
            }

            //destroy any unecessary rows or columns
            dragLayout.columns = 1
            dragLayout.rows = dragLayout.children.length - 1

            //reset all the states
            hovered = false
            dragLayout.dropTarget = -1
            dragLayout.dragItemIndex = -1
        }
    }
}
