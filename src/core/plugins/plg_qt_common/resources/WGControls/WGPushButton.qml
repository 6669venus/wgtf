import QtQuick 2.3
import QtQuick.Controls 1.2
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.1

/*!
 \brief Raised button with border and label.
 Button width determined by label width.
 Can have text, text+icon or just icon

Example:
\code{.js}
WGPushButton {
    text: "Open"
    iconSource: "icons/file"
}
\endcode
*/

Button {
    id: pushButton
    objectName: "WGPushButton"

    /*! This property toggles the visibility of the button frame
        The default value is \c false
    */
    //TODO: This should be renamed, it does not require "_"
    property bool noFrame_: false

    /*! This property toggles the visibility of all visually interactive button elements, excluding mouse over highlight
        The default value is \c false
    */
    //TODO: This should be renamed, it does not require "_"
    property bool noInteraction_: false

    /*! This property is not used by anything
        The default value is defaultSpacing.standardRadius
    */
    //TODO: This should be removed, it is not used by this button or its parent
    property int radius_: defaultSpacing.standardRadius

    /*! This property is used to define the buttons label when used in a WGFormLayout
        The default value is an empty string
    */
    //TODO: This should be renamed, it does not require "_"
    property string label_: ""

    /*! This property determines the checked state of the control
        The default value is false
    */
    property bool checkState: false

    //TODO: This should be removed, enabled is not a valid state and the parent is true already
    activeFocusOnTab: enabled

    activeFocusOnPress: true

    onClicked: {
        setValueHelper( pushButton, "checkState", checked ? true : false );
        pushButton.forceActiveFocus()
    }

    onCheckStateChanged: {
        checked = checkState ? true : false;
    }

    menu: null

    //Auto-sized widths

    implicitWidth: {
        if (iconSource){
            labelText.paintedWidth + icon.width + defaultSpacing.minimumRowHeight
        } else {
            labelText.paintedWidth + defaultSpacing.minimumRowHeight
        }
    }

    implicitHeight: defaultSpacing.minimumRowHeight ? defaultSpacing.minimumRowHeight : 22

    /*! This property holds the target control's id to be bound to this control's b_Value */
    property alias b_Target: dataBinding.target

    /*! This property determines b_Target's property which is to be bound to this control's b_Value */
    property alias b_Property: dataBinding.property

    /*! This property determines this control's value which will drive b_Target's b_Property */
    property alias b_Value: dataBinding.value

    Binding {
        id: dataBinding

    }

    RowLayout {
        id: labelFrame
        anchors.centerIn: pushButton
        spacing: defaultSpacing.standardMargin

        anchors {left: parent.left; right: parent.right}

        //Disabled icons are desaturated and faded.
        //For some reason having the opacity change in the image didn't work with Desaturate so added parent Item

        //Icon and label are here rather than in ButtonStyle. ImageEffects were too difficult inside the style.

        Item {
            anchors.verticalCenter: parent.verticalCenter
            opacity: pushButton.enabled ? 1 : 0.35

            width: {
                if (pushButton.iconSource){
                    icon.width
                } else {
                    0
                }
            }

            Image{
                id: icon
                anchors.verticalCenter: parent.verticalCenter
                source: pushButton.iconSource
                visible: pushButton.enabled
            }

            Image{
                id: dropDownArrow
                anchors.verticalCenter: parent.verticalCenter
                source: "icons/drop_down_arrow_16x16.png"
                z: 1
                visible: pushButton.enabled && (pushButton.menu != null)
            }

            Desaturate {
                anchors.fill: icon
                cached: true
                source: icon
                desaturation: pushButton.enabled ? 0 : 1
            }
        }

        Text {
            id: labelText
            text: pushButton.text

            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter

            renderType: Text.NativeRendering

            //icon only buttons are offcentre without this
            visible: {
                if (pushButton.text){
                    true
                } else {
                    false
                }
            }

            color: {
                if (pushButton.enabled && pushButton.checked){
                    palette.HighlightTextColor
                } else if (pushButton.enabled && pushButton.hovered) {
                    palette.TextColor
                } else if (pushButton.enabled && pushButton.pressed) {
                    palette.TextColor
                } else if (pushButton.enabled && !pushButton.hovered && !pushButton.pressed) {
                    palette.NeutralTextColor
                } else if (!pushButton.enabled) {
                    palette.DisabledTextColor
                }
            }
        }
    }

    style: WGButtonStyle{

    }
}
