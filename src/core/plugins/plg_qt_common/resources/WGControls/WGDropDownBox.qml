import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Controls.Private 1.0
import BWControls 1.0

//Drop Down box with styleable menu

//This control is still very rough.
//The default QML ComboBox is fairly poor, this just hacks a styleable menu into it but still has most of its other problems.

ComboBox {

    function getMaxWidth (model){

        var longestItem = "";
        var itemText = "";

        for (var i=0; i < model.count; i++){
            itemText = model.get(i).text
            if (itemText.length > longestItem.length){
                longestItem = itemText;
            }
        }
        return longestItem;
    }

    id: box

    activeFocusOnTab: enabled

    currentIndex: 0

    implicitWidth: fakeText.paintedWidth + defaultSpacing.leftMargin + defaultSpacing.rightMargin + defaultSpacing.doubleMargin

    implicitHeight: {
        if (defaultSpacing.minimumRowHeight){
            defaultSpacing.minimumRowHeight
        } else {
            22
        }
    }

    property string label_: ""

    property alias b_Target: dataBinding.target
    property alias b_Property: dataBinding.property
    property alias b_Value: dataBinding.value

    Binding {
        id: dataBinding

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
                if (control.enabled){
                    palette.DarkerShade
                } else if (!control.enabled){
                    palette.DarkShade
                }
            }

            innerBorderColor_: {
                if (control.enabled && control.pressed){
                    palette.DarkerShade
                } else if (control.enabled && !control.pressed && control.activeFocus){
                    palette.HighlightShade
                } else if (control.enabled && !control.pressed && !control.activeFocus){
                    palette.LighterShade
                } else if (!control.enabled){
                    "transparent"
                }
            }

            highlightColor_: {
                if (control.pressed){
                    palette.DarkerShade
                } else if (control.hovered && !control.pressed && !palette.GlowStyle){
                    palette.LighterShade
                } else {
                    "transparent"
                }
            }

            hovered_: control.hovered

            Text {
                color : {
                    if (control.enabled && control.hovered && control.pressed){
                        palette.TextColor
                    } else if (control.enabled) {
                        palette.NeutralTextColor
                    } else if (!control.enabled) {
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
                if (control.enabled && control.hovered) {
                    palette.TextColor
                } else if (control.enabled && control.pressed) {
                    palette.TextColor
                } else if (control.enabled && !control.hovered && !control.pressed) {
                    palette.NeutralTextColor
                } else if (!control.enabled) {
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

        //Not sure what this does... apparently nothing so commented out
        /*
        property Component __popupStyle: Style {
            property int __maxPopupHeight: 400
            property int submenuOverlap: 0

            property Component frame: Rectangle {
                width: (parent ? parent.contentWidth : 0)
                height: (parent ? parent.contentHeight : 0) + 2
                color: "transparent"
                property real maxHeight: 500
                property int margin: 1
            }

            property Component menuItemPanel: Text {
                text: "NOT IMPLEMENTED"
                color: "red"
                font {
                    pixelSize: 14
                    bold: true
                }
            }

            property Component __scrollerStyle: null
        }
        */
    }
}
