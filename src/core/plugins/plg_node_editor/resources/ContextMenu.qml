import QtQuick 2.3
import QtQuick.Controls 1.2
import WGControls 1.0

 
WGContextArea
{
    id: contextArea

    contextMenu: WGMenu
    {
        id: dynamicContextMenu
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
    }
}
