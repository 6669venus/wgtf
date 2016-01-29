import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Controls.Private 1.0
import BWControls 1.0

/*!
 \  brief Drop Down box intended to be used over a 3D viewport with styleable menu
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

WGDropDownBox {
    id: box
    objectName: "WGOverlayDropDownBox"

    __textColor: palette.OverlayDarkerShade

    style: ComboBoxStyle {
        id: comboBox
        background: WGButtonFrame {
            id: buttonFrame

            border.color: palette.OverlayDarkerShade
            color: palette.OverlayLightShade

            states: [
                State {
                    name: "PRESSED"
                    when: control.pressed && control.enabled
                    PropertyChanges {target: buttonFrame; color: palette.DarkestShade}
                },
                State {
                    name: "HOVERED"
                    when: control.hovered && control.enabled
                    PropertyChanges {target: buttonFrame; color: palette.OverlayLighterShade}
                    PropertyChanges {target: box; __textColor: palette.OverlayTextColor}
                },
                State {
                    name: "DISABLED"
                    when: !control.enabled
                    PropertyChanges {target: buttonFrame; color: palette.LightestShade }
                    PropertyChanges {target: box; __textColor: palette.DarkestShade}
                },
                State {
                    name: "ACTIVE FOCUS"
                    when: control.enabled && control.activeFocus
                    PropertyChanges {target: buttonFrame; innerBorderColor: palette.HighlightShade}
                }

            ]

            Text {
                id: expandIcon
                color : box.__textColor

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
            color : box.__textColor
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
