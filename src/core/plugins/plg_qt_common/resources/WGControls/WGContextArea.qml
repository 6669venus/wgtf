import QtQuick 2.3
import QtQuick.Controls 1.2
import WGControls 1.0

/*!
 \brief Context sensitive Right Click Menu

Example:
\code{.js}
Component {
    id: fileContextMenu
    Item {
        WGContextArea {
            // TODO: Allow the menu component to be loaded via the view model to allow customization
            // Use the selection as context for determining if menu items are enabled
            contextMenu: WGMenu
            {
                WGMenu {
                    id: expolorerMenu
                    title: "Explorer"
                    MenuItem {
                        text: "Create Path"
                        onTriggered: rootFrame.viewModel.contextMenu.createPath
                        enabled: rootFrame.canCreatePath
                    }

                    MenuItem {
                        text: "Explore"
                        onTriggered: rootFrame.viewModel.contextMenu.explore
                        enabled: rootFrame.canExplore
                    }
                }
            }
        }
    }
}
\endcode
*/

MouseArea {
    objectName: "WGContextArea"
    anchors.fill: parent
    propagateComposedEvents: true
    preventStealing: true

    acceptedButtons: Qt.RightButton

    default property var contextMenu

    onClicked: {
        if (mouse.button == Qt.RightButton)
        {
            if (contextMenu && contextMenu.items.length > 0)
            {
                mouse.accepted = true
                contextMenu.popup()
            }
        }
        else
        {
            mouse.accepted = false
        }
    }
}
