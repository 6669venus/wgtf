import QtQuick 2.3
import QtQuick.Controls 1.2
import BWControls 1.0

//Text entry field mostly intended for strings

TextField {
    id: textBox
    property bool noFrame_ : false
    property string label_: ""

    //context menu switches
    property bool assetBrowserContextMenu : true

	property string oldText

	activeFocusOnTab: readOnly ? false : true

	verticalAlignment: TextInput.AlignVCenter

	implicitHeight: {
		if (defaultSpacing.minimumRowHeight){
			defaultSpacing.minimumRowHeight
		} else {
			22
		}
	}

    property alias b_Target: dataBinding.target
    property alias b_Property: dataBinding.property
    property alias b_Value: dataBinding.value

	// This signal is emitted when test field loses focus and text changes is accepted
	signal editAccepted();

    Binding {
        id: dataBinding

    }

	// support copy&paste
	WGCopyable {
		id: copyableControl

		BWCopyable {
			id: copyableObject

			onDataCopied : {
				setValue( textBox.text )
			}

			onDataPasted : {
				textBox.text = data
			}
		}

		onSelectedChanged : {
			if(selected)
			{
				selectControl( copyableObject )
			}
			else
			{
				deselectControl( copyableObject )
			}
		}
	}

    //Placeholder text in italics
	font.italic: text == "" ? true : false

	onActiveFocusChanged: {
		if (activeFocus)
		{
			setValueHelper( textBox, "oldText", text );
		}
		else
		{
			if (acceptableInput && (text !== oldText))
			{
				editAccepted();
			}
		}
	}

    style: WGTextBoxStyle {

    }

    MouseArea {
        id: mouseAreaContextMenu
        acceptedButtons: Qt.RightButton

        propagateComposedEvents: true
        preventStealing: false
        anchors.fill: parent
        hoverEnabled: true

        cursorShape: Qt.IBeamCursor        
        onClicked:{
            var highlightedText = selectedText
            contextMenu.popup()
        }
    }

	Keys.onPressed: {
		if (activeFocus)
		{
			if(event.key == Qt.Key_Enter || event.key == Qt.Key_Return)
			{
				textBox.focus = false;
			}
			else if (event.key == Qt.Key_Escape)
			{
				setValueHelper( textBox, "text", oldText );
				textBox.focus = false;
			}
		}
	}

    // Some context menu items may be data driven.
    // I have added a visibility switch to contextMenu
	WGMenu {
        id: contextMenu
        title: "Edit"

        MenuItem {
            text: "Cut"
            shortcut: "Ctrl+X"
            enabled: readOnly == true ? false : true
            onTriggered: {                
                cut()
            }
        }

        MenuItem {
            text: "Copy"
            shortcut: "Ctrl+C"
            onTriggered: {                
                copy()
            }
        }

        MenuItem {
            text: "Paste"
            shortcut: "Ctrl+V"
            enabled: canPaste == true ? true : false
            onTriggered: {
                paste()
            }
        }

        MenuSeparator { }

        MenuItem {
            text: "Select All"
            shortcut: "Ctrl+A"
            onTriggered: {                
                selectAll()
            }
        }

        MenuSeparator { }

        MenuItem {
            text: "Find In AssetBrowser"
            shortcut: "Ctrl+?"
            visible: assetBrowserContextMenu == true ? true : false
            onTriggered: {
                console.log("Not yet implemented")
            }
        }
    }
}
