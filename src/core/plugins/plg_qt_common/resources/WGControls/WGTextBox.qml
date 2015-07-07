import QtQuick 2.3
import QtQuick.Controls 1.2

//Text entry field mostly intended for strings

TextField {
    id: textBox
    property bool noFrame_ : false
    property string label_: ""

    //context menu switches
    property bool assetBrowserContextMenu : true

    activeFocusOnTab: readOnly ? false : true

    implicitHeight: {
        if (panelProps.rowHeight_){
            panelProps.rowHeight_
        } else {
            22
        }
    }

    property alias b_Target: dataBinding.target
    property alias b_Property: dataBinding.property
    property alias b_Value: dataBinding.value

    Binding {
        id: dataBinding

    }

    //fix for input text being too high. There is a matching -1 in WGTextBoxStyle
    y: 1

    //Placeholder text in italics
    font.italic: text == "" ? true : false

    style: WGTextBoxStyle {

	}

	WGContextArea {

		cursorShape: Qt.IBeamCursor

		WGMenu{
			id: contextMenu
			MenuItem{
				text: "Cut"
				shortcut: "Ctrl+X"
				enabled: readOnly == true  || selectedText == "" ? false : true
				onTriggered: {
					cut()
				}
			}
			MenuItem{
				text: "Copy"
				shortcut: "Ctrl+C"
				enabled: selectedText == "" ? false : true
				onTriggered: {
					copy()
				}
			}
			MenuItem{
				text: "Paste"
				shortcut: "Ctrl+V"
				enabled: canPaste == true ? true : false
				onTriggered: {
					paste()
				}
			}

			MenuSeparator{}

			MenuItem{
				text: "Select All"
				shortcut: "Ctrl+A"
				onTriggered: {
					selectAll()
				}
			}
		}
	}
}
