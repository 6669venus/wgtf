import QtQuick 2.3
import QtQuick.Controls 1.2
import WGControls 1.0


WGContextArea
{
    id: contextArea

    property string menuPath: "NodeEditor"

    contextMenu: WGMenu
    {
        id: dynamicContextMenu
        path: menuPath
        WGListModel
        {
            id : contextMenuModel
            source : graphView.nodeClassesModel

            ValueExtension {}
        }

        Instantiator
        {
            id : menuItemInstant
            model : contextMenuModel
            onObjectAdded: dynamicContextMenu.insertItem( index, object )
            onObjectRemoved: dynamicContextMenu.removeItem( object )
            delegate : MenuItem
            {
                text : Value
                onTriggered :
                {
                    createNode(contextArea.popupPoint.x, contextArea.popupPoint.y, Value);
                }
            }
        }
        MenuItem
        {
            text: qsTr("Create Group Box")
            onTriggered: {
                createGroup(contextArea.popupPoint.x, contextArea.popupPoint.y,"New Group", "#00FF00", 500, 500)
            }
        }
    }
}
