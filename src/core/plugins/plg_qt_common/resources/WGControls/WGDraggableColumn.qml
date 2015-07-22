import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

// A Column layout where every child has a draggable handle on the left hand side
// The handle can be dragged to a new position, then the actual children are shuffled into place
// Every child MUST be able to use anchors: eg WGSubPanel, WGExpandingRowLayout

//WGRowLayout, WGBoolGridLayout etc. must be inside a WGColumnLayout or they will cause errors!!!!

WGColumnLayout {
    id: mainColumn
    rows: children.length + 1
    columns: 1

    //The panel being dragged and it's row
    property QtObject draggedObject
    property int dragItemIndex: -1

    //The dragHandle underneath the dragged panel
    property QtObject targetObject
    property int dropTarget: -1

    //fake object at the end of the column so panels can be dragged to the bottom
    property QtObject lastSpacer

    //has a dragged object been dragged over a space?
    property bool hovering_: false

    property bool unLocked_: !globalSettings.dragLocked

    flow: GridLayout.TopToBottom

    //if something is being dragged, give the fake object a size
    onDragItemIndexChanged: {
        if(dragItemIndex > -1){
            lastSpacer.lineSpaces_ = 5
        } else {
            lastSpacer.lineSpaces_ = 0
        }
    }


    Component.onCompleted: {

        //create an invisible object to be the last space in the column
        var bottomRow = Qt.createComponent("WGVerticalSpacer.qml");
        if (bottomRow.status === Component.Ready){
            lastSpacer = bottomRow.createObject(mainColumn, {
                                       "Layout.alignment": Qt.AlignTop,
                                       "Layout.fillWidth": true,
                                       "lineSpaces_": 0
                                   });
        }

        var shuffleRow = 0
        for(var i = 0; i < children.length; i++){

            //shuffle the draggable children up a row and generate a DragHandle behind them
            children[i].Layout.row = shuffleRow
            children[i].Layout.column = 0
            shuffleRow++
            var newHandle = Qt.createComponent("WGDragHandle.qml");
            if (newHandle.status === Component.Ready){
                newHandle.createObject(mainColumn.children[i], {
                                           "index": i,
                                           "z": -1,
                                           "dragLayout": mainColumn,
                                       });
            }
            children[i].Layout.alignment = Qt.AlignBottom
        }

        columns = 2
        rows = children.length - 1
    }
}
