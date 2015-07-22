import QtQuick 2.0
import QtQuick.Window 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.0

Rectangle {
    property ListModel treeModel;
    signal nodeSelected(variant model, variant node)

    color: palette.MainWindowColor
    anchors.fill: parent

    ScrollView
    {
        anchors.fill: parent
        ListView {
            id: listView
            model: treeModel
            delegate: nodeComponent
        }
    }

    Component {
        id: nodeComponent

        ColumnLayout {
            id: nodeRoot
            objectName: "nodeRoot"
            FocusScope {
                id: node;
                objectName: "node";
                focus: true;
                width: row.implicitWidth
                height: row.implicitHeight

                function getParentNode() {
                    if(!model.parentNode){
                        var foundParent = nodeRoot.parent;
                        while(foundParent && foundParent.objectName != "nodeRoot"){
                            foundParent = foundParent.parent;
                        }
                        model.parentNode = findDescendant(foundParent,"node");
                    }
                    return model.parentNode;
                }

                function collapse() {
                    var expanded = childContainer.visible;
                    childContainer.visible = false;
                    return expanded;
                }

                function expand() {
                    var expanded = childContainer.visible;
                    if(model.nodes.count > 0)
                        childContainer.visible = true;
                    return !expanded;
                }

                function collapseExpandToggle(){
                    if(childContainer.visible)
                        collapse();
                    else
                        expand();
                }

                function findDescendant(start, name){
                    if(!start)
                        return null;
                    var found = null;
                    for(var i = 0; i < start.children.length && !found; ++i){
                        var child = start.children[i];
                        if(child.objectName === name)
                            found = child;
                        else
                            found = findDescendant(child, name);
                    }
                    return found;
                }

                onActiveFocusChanged:{
                    if(node.focus)
                        nodeSelected(model, node);
                }

                Keys.onPressed: {
                    if(event.key === Qt.Key_Left){
                        // Collapse this node if possible
                        if(!node.collapse()){
                            // Move focus up to parent node if this node can't collapse
                            var parentNode = getParentNode();
                            if(parentNode)
                                parentNode.focus = true;
                        }

                        event.accepted = true;
                    }
                    else if(event.key === Qt.Key_Right){
                        if(!node.expand()){
                            // Move focus down to the first child if this node can't expand
                            var firstChild = findDescendant(childContainer, "node");
                            if(firstChild)
                                firstChild.focus = true;
                        }

                        event.accepted = true;
                    }
                }

                Row {
                    id: row

                    // Indentation for children nodes
                    Item {
                        height: 1
                        width: model.level * 20
                    }

                    // TODO: Use a delegate to allow custom presentation for the collapse/expand area
                    // Collapse/Expand
                    ToolButton{
                        width: collapseText.paintedHeight;
                        height: collapseText.paintedHeight;
                        enabled: model.nodes.count > 0;
                        onClicked: node.collapseExpandToggle();
                        Rectangle
                        {
                            border.color: palette.TextColor;
                            border.width: 2;
                            Text {
                                id: collapseText
                                verticalAlignment: Text.AlignVCenter;
                                horizontalAlignment: Text.AlignHCenter;
                                color: palette.TextColor;
                                text: (model.nodes.count > 0) ? childContainer.visible ? qsTr("▼") : qsTr("►") : qsTr("");
                            }
                        }
                    }
                    // TODO: Use a delegate to allow custom presentation for the node label
                    // Node Label
                    Rectangle{
                        color: node.activeFocus ? palette.HighlightColor : palette.MainWindowColor;
                        width: label.paintedWidth;
                        height: label.paintedHeight;
                        Text {
                            id: label;
                            objectName: "label";
                            text: model.name;
                            focus: false;
                            color: node.activeFocus ? palette.HighlightTextColor : palette.TextColor;
                        }

                        MouseArea {
                            anchors.fill: parent
                            onDoubleClicked: {
                                node.collapseExpandToggle();
                                mouse.accepted = true;
                            }
                            onClicked:{
                                node.forceActiveFocus();
                                node.focus = true;
                            }
                        }
                    }
                }
            }
            ColumnLayout
            {
                id: childContainer
                //visible: model.nodes.count > 0;
                visible: false;

                Repeater {
                    id: repeater
                    model: nodes
                    delegate: nodeComponent
                }
            }
        }
    }
}
