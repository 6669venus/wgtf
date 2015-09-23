import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Controls.Private 1.0
import BWControls 1.0

//TODO: Does this control require some more work as indicated in the brief?

/*!
 \  brief Drop Down box with styleable menu
    This control is still a WIP
    The default QML ComboBox is feature lacking.
    This modification adds a styleable menu into it but still has most of its other problems.

Example:
\code{.js}
WGDropDownBox {
    id: dropDown

    model: ListModel {
        ListElement { text: "Option 1" }
        ListElement { text: "Option 2" }
        ListElement { text: "Option 3 Has a Really long Name" }
        ListElement { text: "Option 4" }
        ListElement { text: "Option 5" }
    }
}
\endcode
*/

ComboBox {
    objectName: "WGDropDownBox"

    /*! This property is used to define the buttons label when used in a WGFormLayout
        The default value is an empty string
    */
    //TODO: This should be renamed, it does not require "_"
    property string label_: ""


    /*! This property holds the target control's id to be bound to this control's b_Value */
    property alias b_Target: dataBinding.target

    /*! This property determines b_Target's property which is to be bound to this control's b_Value */
    property alias b_Property: dataBinding.property

    /*! This property determines this control's value which will drive b_Target's b_Property */
    property alias b_Value: dataBinding.value

    id: box

    activeFocusOnTab: enabled

    currentIndex: 0

    implicitWidth: fakeText.paintedWidth + defaultSpacing.leftMargin + defaultSpacing.rightMargin + defaultSpacing.doubleMargin

    implicitHeight: defaultSpacing.minimumRowHeight ? defaultSpacing.minimumRowHeight : 22

    Binding {
        id: dataBinding
    }

    /*! \internal */
    function getMaxWidth (model){

        var longestItem = "";
        var itemText = "";

        for (var i=0; i < model.count; i++)
        {
            itemText = model.get(i).text
            if (itemText.length > longestItem.length)
            {
                longestItem = itemText;
            }
        }
        return longestItem;
    }

    // support copy&paste
    WGCopyable {
        id: copyableControl

        BWCopyable {
            id: copyableObject

            onDataCopied : {
                setValue( box.currentIndex )
            }

            onDataPasted : {
                box.currentIndex = data
            }
        }

        onSelectedChanged : selected ? selectControl( copyableObject ) : deselectControl( copyableObject )
    }

    Text {
        //fake text to make the implicit width large enough for the longest item
        id: fakeText
        anchors.fill: parent
        visible: false
        text: getMaxWidth(model)
    }

    style: ComboBoxStyle {
        id: comboBox
        background: WGButtonFrame {

            color: palette.LightShade

            borderColor_: {
                if (control.enabled)
                {
                    palette.DarkerShade
                }
                else if (!control.enabled)
                {
                    palette.DarkShade
                }
            }

            innerBorderColor_: {
                if (control.enabled && control.pressed)
                {
                    palette.DarkerShade
                }
                else if (control.enabled && !control.pressed && control.activeFocus)
                {
                    palette.LighterShade
                }
                else if (control.enabled && !control.pressed && !control.activeFocus)
                {
                    palette.LightShade
                }
                else if (!control.enabled)
                {
                    "transparent"
                }
            }

            highlightColor_: {
                if (control.pressed)
                {
                    palette.DarkerShade
                }
                else if (control.hovered && !control.pressed && !palette.GlowStyle)
                {
                    palette.LighterShade
                }
                else
                {
                    "transparent"
                }
            }

            hovered_: control.hovered

            Text {
                color : {
                    if (control.enabled && control.hovered && control.pressed)
                    {
                        palette.TextColor
                    }
                    else if (control.enabled)
                    {
                        palette.NeutralTextColor
                    }
                    else if (!control.enabled)
                    {
                        palette.DisabledTextColor
                    }
                }

                anchors.fill: parent
                anchors.rightMargin: defaultSpacing.standardMargin

                font.family : "Marlett"
                font.pixelSize: parent.height / 2
                renderType: Text.NativeRendering
                text : "u"
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignRight
            }
        }

        label: Text {
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignLeft
            color: {
                if (control.enabled && control.hovered)
                {
                    palette.TextColor
                }
                else if (control.enabled && control.pressed)
                {
                    palette.TextColor
                }
                else if (control.enabled && !control.hovered && !control.pressed)
                {
                    palette.NeutralTextColor
                }
                else if (!control.enabled)
                {
                    palette.DisabledTextColor
                }
            }
            text: control.currentText
            renderType: Text.NativeRendering
        }

        // drop-down customization here
        property Component __dropDownStyle: MenuStyle {
            __maxPopupHeight: 600
            __menuItemType: "comboboxitem"

            frame: Rectangle {              // background
                color: palette.MainWindowColor
                border.width: defaultSpacing.standardBorderSize
                border.color: palette.DarkColor
            }

            itemDelegate.label:             // an item text
                Text {
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                color: styleData.selected ? palette.TextColor : palette.HighlightTextColor
                text: styleData.text
            }

            itemDelegate.background: WGHighlightFrame {  // selection of an item
                visible: styleData.selected ? true : false
            }

            __scrollerStyle: ScrollViewStyle { }
        }
    }
}
