import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import BWControls 1.0

Item {
    id: itemRow
    objectName: typeof(model.display) != "undefined" ? "WGItemRow_" + model.display : "WGItemRow"

    width: childrenRect.width != 0 ? childrenRect.width : 1024
    height: childrenRect.height != 0 ? childrenRect.height : 1024

    property var columnDelegates: []
    property var columnSequence: []
    property var columnWidths: []
    property alias columnSpacing: row.spacing

    property bool selected: false

    signal itemPressed(var mouse, var itemIndex)
    signal itemClicked(var mouse, var itemIndex)
    signal itemDoubleClicked(var mouse, var itemIndex)

    /* MOVE INTO STYLE*/
    Rectangle {
        id: backgroundArea
        anchors.fill: row
        color: palette.highlightShade
        opacity: selected ? 1 : 0.5
        visible: hoverArea.containsMouse || selected
    }

    MouseArea {
        id: hoverArea
        anchors.fill: backgroundArea
        hoverEnabled: true
    }
    /**/

    Row {
        id: row
        objectName: typeof(model.display) != "undefined" ? "Row_" + model.display : "Row"

        Repeater {
            model: SequenceList {
                model: columnModel
                sequence: columnSequence
            }

            Item {
                width: columnWidths[index]
                height: childrenRect.height
                clip: true

                MouseArea {
                    width: columnWidths[index]
                    height: row.height
                    acceptedButtons: Qt.RightButton | Qt.LeftButton;

                    onPressed: itemPressed(mouse, modelIndex)
                    onClicked: itemClicked(mouse, modelIndex)
                    onDoubleClicked: itemDoubleClicked(mouse, modelIndex)
                }

                Row {
                    id: columnLayoutRow
                    objectName: typeof(model.display) != "undefined" ? "columnLayoutRow_" + model.display : "columnLayoutRow"

                    /* MOVE INTO STYLE*/
                    Row {
                        id: iconArea
                        objectName: typeof(model.display) != "undefined" ? "iconArea_" + model.display : "iconArea"
                        anchors.verticalCenter: parent.verticalCenter

                        width: childrenRect.width
                        height: childrenRect.height

                        visible: __isTree && index == 0

                        property bool __isTree: typeof expanded != "undefined"
                        property real __depth: __isTree ? depth : 0
                        property bool __hasChildren: __isTree ? hasChildren : false
                        property bool __expanded: __isTree ? expanded : false

                        Item {
                            width: iconArea.__depth * 10
                            height: 1
                        }

                        Text {
                            objectName: typeof(model.display) != "undefined" ? "expandIcon_" + model.display : "expandIcon"
                            color: iconArea.__hasChildren ? iconArea.__expanded ? palette.textColor : palette.neutralTextColor : "transparent"
                            font.family : "Marlett"
                            text : iconArea.__expanded ? "\uF036" : "\uF034"
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter

                            MouseArea {
                                anchors.fill: parent
                                enabled: iconArea.__hasChildren
                                onPressed: {
                                    expanded = !expanded
                                }
                            }
                        }
                    }
                    /**/

                    Loader {
                        id: columnDelegateLoader
                        property var itemData: model
                        property var itemWidth: columnWidths[index] - x
                        sourceComponent: itemRow.columnDelegates[index]
                    }
                }
            }
        }
    }
}
