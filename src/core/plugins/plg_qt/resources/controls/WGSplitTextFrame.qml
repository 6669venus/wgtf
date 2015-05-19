import QtQuick 2.0

//Frame broken up into X textboxes
//Can use separators or decimal places between each box

WGTextBoxFrame {

    id: mainFrame

    property QtObject boxList_: ListModel{
        ListElement {
            name_: "testBox1_"
            text_: "Text Box 1"
            num_: false
            value_: 50
            minValue_: 0
            maxValue_: 100
            prefix_: ""
            suffix_: ""
            placeholder_: ""
        }
        ListElement {
            name_: "testBox2_"
            text_: "Text Box 2"
            num_: false
            value_: 50
            minValue_: 0
            maxValue_: 100
            prefix_: ""
            suffix_: ""
            placeholder_: ""
        }
    }

    property bool decimalSeparator_: false //use a '.' instead of a '|' between fields

    property int textBoxes_ : boxList_.count //number of textboxes

    property bool numberBox_: false

    property string label_: ""

    property int totalWidth_: 0

    implicitHeight: {
        if (panelProps.rowHeight_){
            panelProps.rowHeight_
        } else {
            22
        }
    }

    implicitWidth: 40 * parent.textBoxes_

    width: {
        if(totalWidth_ > 0){
            totalWidth_
        } else {
            implicitWidth
        }
    }

    //TODO give this frame a disabled state

    Row {
        Repeater {
            model: boxList_
            Rectangle {
                width: {
                    if(typeof width_ != "undefined"){
                        width_
                    } else {
                        Math.round(mainFrame.width / textBoxes_)
                    }
                }
                height: mainFrame.height

                color: "transparent"

                Loader {
                    id: fieldLoader
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    height: parent.height
                    sourceComponent: num_ ? numBox : textBox

                    onLoaded: {
                        if(typeof width_ != "undefined"){
                            totalWidth_ += width_
                        }
                    }

                    Component {
                        id: textBox
                        WGTextBox {
                            text: {
                                if(typeof text_ != "undefined"){
                                    text_
                                } else {
                                    ""
                                }
                            }
                            placeholderText: {
                                if(typeof placeholder_ != "undefined"){
                                    placeholder_
                                } else {
                                    ""
                                }
                            }

                            noFrame_: true
                            horizontalAlignment: Text.AlignHCenter

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
                        }
                    }

                    Component {
                        id: numBox
                        WGSpinBox {
                            value: {
                                if(typeof value_ != "undefined"){
                                    value_
                                } else {
                                    0
                                }
                            }

                            minimumValue: {
                                if(typeof minValue_ != "undefined"){
                                    minValue_
                                } else {
                                    0
                                }
                            }

                            maximumValue: {
                                if(typeof maxValue_ != "undefined"){
                                    maxValue_
                                } else {
                                    999
                                }
                            }

                            prefix: {
                                if(typeof prefix_ != "undefined"){
                                    prefix_
                                } else {
                                    ""
                                }
                            }

                            suffix: {
                                if(typeof suffix_ != "undefined"){
                                    suffix_
                                } else {
                                    ""
                                }
                            }

                            noFrame_: true
                            noArrows_: true
                            horizontalAlignment: Text.AlignHCenter

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
                        }
                    }
                }
                //decimal point to be used for IP addresses etc.
                WGLabel {
                    anchors.horizontalCenter: parent.left
                    height: mainFrame.height - panelProps.doubleBorder_
                    y: 3
                    text: "."
                    visible: {
                        if (index != 0 && decimalSeparator_){
                            true
                        } else {
                            false
                        }
                    }
                }

                WGSeparator {
                    anchors.horizontalCenter: parent.left
                    height: mainFrame.height - panelProps.doubleBorder_
                    anchors.verticalCenter: parent.verticalCenter
                    vertical_: true

                    //first separator is invisible
                    visible: {
                        if (index != 0 && !decimalSeparator_){
                            true
                        } else {
                            false
                        }
                    }
                }
            }
        }
    }
}
