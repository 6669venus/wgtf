import QtQuick 2.3
import QtQuick.Controls 1.2
import BWControls 1.0

/*!
 \brief Text entry field mostly intended for strings

Example:
\code{.js}
WGTextBox {
    placeholderText: "Placeholder Text"
    Layout.fillWidth: true
}
\endcode
*/

TextField {
    id: textBox
    objectName: "WGTextBox"

    /*! This property is used to define the buttons label when used in a WGFormLayout
        The default value is an empty string
    */
    //TODO: This should be renamed, it does not require "_"
    property string label_: ""

    /*! This property determines if the context menu for this control contains the "Find In AssetBrowser" option
        The default value is \c true
    */
    property bool assetBrowserContextMenu : true

    /*! This property is used by the setValueHelper function which requires documenting */
    //TODO This requires documenting
    property string oldText

    activeFocusOnTab: readOnly ? false : true

    verticalAlignment: TextInput.AlignVCenter

    implicitHeight: defaultSpacing.minimumRowHeight ? defaultSpacing.minimumRowHeight : 22

    /*! This property holds the target control's id to be bound to this controls b_Value */
    property alias b_Target: dataBinding.target

    /*! This property determines b_Target's property which is to be bound to this controls b_Value */
    property alias b_Property: dataBinding.property

    /*! This property determines this control's value which will drive b_Target's b_Property */
    property alias b_Value: dataBinding.value
			
	/*! This property denotes if the control's text should be scaled appropriately as it is resized */
	smooth: true


    /*! This signal is emitted when test field loses focus and text changes is accepted */
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
