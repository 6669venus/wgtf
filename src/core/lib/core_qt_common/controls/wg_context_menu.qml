import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import QtQuick.Window 2.1
import WGControls 1.0

/*!
    \brief A context menu control that uses actions data tied to a specific path to 
	determine what menu items are displayed.

Example:
\code{.js}
    WGContextMenu {
		id: dynamicContextMenu
		path: "versionControl"
		windowId: "fileListWindow"
    }
\endcode
*/

Item {
	id: rootFrame
	width: 300
	height: 600

	property var internalActions_: actions_

	Component.onCompleted: {
		console.log("TEST - qml view for context menu was made - [" + internalActions_ + "]");
        var actionItr = iterator( internalActions_ )
        while (actionItr.moveNext()) {
            console.log("TEST - action: " + actionItr.current);
        }
	}

	//TODO - ask art what we can do to make this actually show up since it's showing up too small!
	WGDropDownBox {
		id: dropDownContextMenu
		Layout.preferredWidth: 150
		currentIndex: 0
		model: internalActions_
	}
}