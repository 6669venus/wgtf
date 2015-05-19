import QtQuick 2.0
import QtQuick.Controls 1.1

//Frame broken up into 'x' buttons with two small caps on either end.
//A few odd +/- 1px tweaks in here to make the button highlights look ok with 2 pixel separators

/* Use this to do something if a button is pressed - but not here.
onButtonBarPressed: {
    if(buttonName == "foo"){
        //do something if foo is pressed
    }
    if (buttonChecked) {
        //do something if a button is checked
    }
}
*/

WGButtonFrame {
    id: mainFrame
    property bool exclusive_ : false

    //placeholder buttons if nothing assigned
    property QtObject buttonList_: ListModel{
        ListElement {
            name_: "testButton1_"
            text_: "Test Button 1"
            checkedText_: ""
            icon_: ""
            checkedIcon_: ""
            checkable_: false
            checked_: false
        }
        ListElement {
            name_: "testButton2_"
            text_: "Test Button 2"
            checkedText_: ""
            icon_: ""
            checkedIcon_: ""
            checkable_: false
            checked_: false
        }
    }

    property int buttons_ : buttonList_.count//number of buttons

    property string label_: ""

    signal buttonBarPressed(string buttonName, bool buttonChecked)

    signal buttonBarCheck(string buttonName, bool state)

    implicitHeight: {
        if (panelProps.rowHeight_){
            panelProps.rowHeight_
        } else {
            22
        }
    }

    ExclusiveGroup {
        id: barGroup
    }

    Row {

        Repeater {
            model: buttonList_
            Rectangle {
                width: mainFrame.width / buttons_
                height: mainFrame.height

                color: "transparent"

                Loader {
                    anchors.fill: parent

                    //nudge the left and right buttons inside the caps
                    anchors.leftMargin: {
                        if (index == 0) {
                            panelProps.standardRadius_
                        } else {
                            - panelProps.separatorWidth_ / 2
                        }
                    }
                    anchors.rightMargin: {
                        if (index == (buttons_ - 1)) {
                            panelProps.standardRadius_
                        } else {
                            0
                        }
                    }

                    sourceComponent: WGPushButton {
                        anchors.fill: parent

                        enabled:{
                            if (typeof enabled_ != "undefined"){
                                enabled_
                            } else {
                                true
                            }
                        }

                        Connections {
                            target: mainFrame
                            onButtonBarCheck: {
                                if (buttonName == name_){
                                    checked = state
                                }
                            }
                        }

                        onClicked: {
                            if (typeof name_ != "undefined"){
                                buttonBarPressed(name_, checked)
                            }
                        }

                        b_Target: {
                            if(typeof target_ != "undefined"){
                                target_
                            } else {
                                null
                            }
                        }
                        b_Property: {
                            if(typeof property_ != "undefined"){
                                property_
                            } else {
                                ""
                            }
                        }
                        b_Value: {
                            if(typeof value_ != "undefined"){
                                value_
                            } else {
                                ""
                            }
                        }

                        //fit colour highlight inside the frame
                        anchors.margins: checked ? panelProps.standardBorder_ : 0

                        //assign defaults if values aren't specified
                        text: {
                            if (checked && typeof checkedText_ != "undefined"){
                                checkedText_
                            } else if(typeof text_ != "undefined"){
                                text_
                            } else {
                                ""
                            }
                        }

                        iconSource: {
                            if(checked && typeof checkedIcon_ != "undefined"){
                                checkedIcon_
                            } else if (typeof icon_ != "undefined") {
                                icon_
                            } else {
                                ""
                            }
                        }

                        noFrame_: true
                        radius_: 0

                        //assign defaults if values aren't specified, if exclusive assume checkable
                        checkable: {
                           if (exclusive_){
                               true
                           } else if (typeof checkable_ != "undefined"){
                               checkable_
                           } else {
                               false
                           }
                        }

                        checked: {
                            if (typeof checked_ != "undefined")
                                checked_
                            else {
                                false
                            }
                        }
                        exclusiveGroup: {
                            if (exclusive_){
                                barGroup
                            } else {
                                null
                            }
                        }
                    }
                }

                WGSeparator {
                    anchors.horizontalCenter: parent.left
                    anchors.horizontalCenterOffset: {
                        if (index == 0){
                            panelProps.standardMargin_
                        } else {
                            0
                        }
                    }
                    height: mainFrame.height - panelProps.doubleBorder_
                    anchors.verticalCenter: parent.verticalCenter
                    vertical_: true
                }
            }
        }
    }

    //extra separator for the right end cap
    WGSeparator {
        anchors.horizontalCenter: parent.right
        anchors.horizontalCenterOffset: {
            -panelProps.standardRadius_
        }
        height: mainFrame.height - panelProps.doubleBorder_
        anchors.verticalCenter: parent.verticalCenter
        vertical_: true
    }
}

