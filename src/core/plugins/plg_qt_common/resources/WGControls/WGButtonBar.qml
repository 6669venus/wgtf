import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

/*!
 \brief Frame broken up into 'x' buttons with two small caps on either end.
 WGButtonBar is intended to contain WGToolButtons, WGPushButtons will have odd borders.
 A few odd +/- 1px tweaks in here to make the button highlights look ok with 2 pixel separators

Example:
\code{.js}
WGButtonBar {
    buttonList: [
        WGToolButton {
            text: "One"
        },
        WGToolButton {
            text: "Check"
            checkState: true
            checkable: true
        },
        WGToolButton {
            iconSource: "icons/save_16x16"
        }
    ]
}
\endcode
*/

WGButtonFrame {
    id: mainFrame
    objectName: "WGButtonFrame"

    /*! This property contains the list of objects, usually WGPushButtons, that will populate the buttonbar */
    property list <QtObject> buttonList

    /*! \internal */
    //TODO: This should be an internal control and should be marked as private by "__" prefix
    property int buttons_ : buttonList.length

    /*! \internal */
    //TODO: This should be an internal control and should be marked as private by "__" prefix
    property int totalWidth: defaultSpacing.doubleMargin

    /*!
        This property is used to define the label displayed used in a WGFormLayout
        The default value is an empty string
    */
    //TODO: This should be renamed, it does not require "_"
    property string label_: ""

    /*!
        This property defines whether the width alocated to each button is evenly distributed.
        The default value is \c true
    */
    property bool evenBoxes: true

    /*!
        This property defines whether vertical lines will be used to split the buttons in the bar.
        Note: There will be unhighlighted gaps at either ends on mouseover/checked state if this is set.
    */

    property bool showSeparators: true


    implicitHeight: defaultSpacing.minimumRowHeight ? defaultSpacing.minimumRowHeight : 22

    //if Layout.preferredWidth is not defined, or set to -1, the button bar will use the total width of all buttons
    implicitWidth: totalWidth

    Row {
        Rectangle {
            height: parent.height
            width: defaultSpacing.standardMargin
            color: "transparent"
        }

        Repeater {
            model: buttonList
            Rectangle {
                id: boxContainer
                width: {
                    if(evenBoxes)
                    {
                        (mainFrame.width - defaultSpacing.doubleMargin) / buttons_
                    }
                    else
                    {
                        buttonList[index].width
                    }
                }
                height: mainFrame.height
                color: "transparent"

                Component.onCompleted: {
                    if (typeof buttonList[index].text != "undefined" && typeof buttonList[index].iconSource != "undefined")
                    {
                        totalWidth += buttonList[index].width
                        buttonList[index].parent = this
                        buttonList[index].anchors.fill = boxContainer
                        buttonList[index].radius = 0
                    }
                    else
                    {
                        boxContainer.color = "red"
                    }
                }

                // left most cap, and button separators
                WGSeparator {
                    visible: showSeparators
                    anchors.horizontalCenter: parent.left
                    height: mainFrame.height - defaultSpacing.doubleBorderSize
                    anchors.verticalCenter: parent.verticalCenter
                    vertical_: true
                }
            }
        }
        //extra separator for the right end cap
        WGSeparator {
            visible: showSeparators
            height: mainFrame.height - defaultSpacing.doubleBorderSize
            anchors.verticalCenter: parent.verticalCenter
            vertical_: true
        }
    }
}
