import QtQuick 2.5
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
    id: box
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

    /*! \internal */
    // helper property for text color so states can all be in the background object
    property color __textColor: palette.neutralTextColor

    activeFocusOnTab: true

    activeFocusOnPress: true

    currentIndex: 0

    implicitWidth: textMetricsCreator.maxWidth + defaultSpacing.leftMargin + defaultSpacing.rightMargin + defaultSpacing.doubleMargin

    implicitHeight: defaultSpacing.minimumRowHeight ? defaultSpacing.minimumRowHeight : 22

    Binding {
        id: dataBinding
    }

    //find the widest text in model
    Repeater {
        id: textMetricsCreator
        model: box.model
        property real maxWidth: 0

        Item {
            id:itemWrapper
            TextMetrics {
                id: fakeText
                text: model.text
                onTextChanged: {
                    textMetricsCreator.maxWidth = Math.max(textMetricsCreator.maxWidth, width)
                }
            }
        }
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

    MouseArea {
        id: wheelMouseArea
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        onWheel: {
            if (box.activeFocus || box.pressed)
                {
                if (wheel.angleDelta.y > 0)
                {
                    __selectPrevItem();
                } else if (wheel.angleDelta.y < 0)
                {
                    __selectNextItem();
                }
            }
        }
    }

    style: ComboBoxStyle {
        objectName: "comboBox"
        id: comboBox
        renderType: Text.NativeRendering
        background: WGButtonFrame {
            id: buttonFrame

            color: palette.lightShade

            states: [
                State {
                    name: "PRESSED"
                    when: control.pressed && control.enabled
                    PropertyChanges {target: buttonFrame; color: palette.darkShade}
                    PropertyChanges {target: buttonFrame; innerBorderColor: "transparent"}
                },
                State {
                    name: "HOVERED"
                    when: control.hovered && control.enabled
                    PropertyChanges {target: box; __textColor: palette.textColor}
                },
                State {
                    name: "DISABLED"
                    when: !control.enabled
                    PropertyChanges {target: buttonFrame; color: "transparent"}
                    PropertyChanges {target: buttonFrame; borderColor: palette.darkShade}
                    PropertyChanges {target: buttonFrame; innerBorderColor: "transparent"}
                    PropertyChanges {target: box; __textColor: palette.disabledTextColor}
                },
                State {
                    name: "ACTIVE FOCUS"
                    when: control.enabled && control.activeFocus
                    PropertyChanges {target: buttonFrame; innerBorderColor: palette.lightestShade}
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
                text : "\uF075"
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
                color: palette.mainWindowColor
                border.width: defaultSpacing.standardBorderSize
                border.color: palette.darkColor
            }

            itemDelegate.label:             // an item text
                Text {
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                color: styleData.selected ? palette.textColor : palette.highlightTextColor
                text: styleData.text
                renderType: Text.NativeRendering
            }

            itemDelegate.background: WGHighlightFrame {  // selection of an item
                visible: styleData.selected ? true : false
            }

            __scrollerStyle: ScrollViewStyle { }
        }
    }
}
