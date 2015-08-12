import QtQuick 2.3
import QtQuick.Controls 1.2

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

    property list <QtObject> buttonList

    property bool exclusive_ : false

    property int buttons_ : buttonList.length //number of buttons

    property int totalWidth: 0 //used to store width of all internal buttons

    property string label_: ""

    signal buttonBarPressed(string buttonName, bool buttonChecked)

    signal buttonBarCheck(string buttonName, bool state)

    implicitHeight: {
        if (defaultSpacing.minimumRowHeight){
            defaultSpacing.minimumRowHeight
        } else {
            22
        }
    }

    implicitWidth: totalWidth //if Layout.preferredWidth is not defined, or set to -1, the button bar will use the total width of all buttons

    Row {
        Repeater {
            model: buttonList
            Rectangle {
                id: boxContainer
                width: mainFrame.width / buttons_
                height: mainFrame.height
                radius: 3
                color: "transparent"

                anchors.leftMargin: 3
                anchors.rightMargin: 2

                Component.onCompleted: {
                    if(typeof buttonList[index].text != "undefined" && typeof buttonList[index].iconSource != "undefined")
                    {
                        totalWidth += buttonList[index].width
                        buttonList[index].noFrame_ = true
                        buttonList[index].parent = this
                        buttonList[index].anchors.fill = boxContainer
                        buttonList[index].radius_ = 0
                    }
                    else
                    {
                        boxContainer.color = "red"
                    }

                    //nudge the left and right most buttons within the button frame end caps
                    if (index == 0) {
                        buttonList[index].anchors.leftMargin = defaultSpacing.standardMargin
                    } else {
                        - defaultSpacing.separatorWidth / 2
                    }

                    if (index == (buttons_ - 1)) {
                        buttonList[index].anchors.rightMargin = defaultSpacing.standardMargin
                    } else {
                        0
                    }
                }

                // left most cap, and button separators
                WGSeparator {
                    anchors.horizontalCenter: parent.left
                    anchors.horizontalCenterOffset: {
                        if (index == 0){
                            defaultSpacing.standardMargin
                        } else {
                            0
                        }
                    }
                    height: mainFrame.height - defaultSpacing.doubleBorderSize
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
            -defaultSpacing.standardRadius
        }
        height: mainFrame.height - defaultSpacing.doubleBorderSize
        anchors.verticalCenter: parent.verticalCenter
        vertical_: true
    }
}

