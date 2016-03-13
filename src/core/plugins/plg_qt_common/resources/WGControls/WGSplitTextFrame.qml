import QtQuick 2.3
import QtQuick.Layouts 1.1

/*!
 \brief A Frame that can be broken up into X textboxes
 Can use separators or decimal places between each box

Example:
\code{.js}
WGSplitTextFrame {
    boxList: [
        WGNumberBox{
            value: 192
            minimumValue: 0
            maximumValue: 255
        },
        WGNumberBox{
            value: 168
            minimumValue: 0
            maximumValue: 255
        },
        WGNumberBox{
            value: 0
            minimumValue: 0
            maximumValue: 255
        },
        WGNumberBox{
            value: 1
            minimumValue: 0
            maximumValue: 255
        }
    ]
    decimalSeparator: true
    height: 24
    Layout.preferredWidth: 130
    label_: "IP Address:"
}
\endcode
*/

WGTextBoxFrame {
    id: mainFrame
    objectName: "WGSplitTextFrame"

    /*! This property contains the list of textboxes to be displayed in the frame */
    property list<QtObject> boxList

    /*! This property toggles use of a decimal '.' instead of a '|' between fields
        The default value is \c false
    */
    property bool decimalSeparator: false

    /*! \internal */
    //TODO: This should be an internal control and should be marked as private by "__" prefix
    property int totalBoxes : boxList.length //number of textboxes

    /*! This property is used to define the buttons label when used in a WGFormLayout
        The default value is an empty string
    */
    //TODO: This should be renamed, it does not require "_"
    property string label_: ""

    /*! \internal */
    //TODO: This should be an internal control and should be marked as private by "__" prefix
    property int totalWidth: 0

    /*!
        This property defines whether the width alocated to each box is evenly distributed.
        The default value is \c true
    */
    property bool evenBoxes: true

    property Component invisibleStyle: WGInvisTextBoxStyle {}

    implicitHeight: defaultSpacing.minimumRowHeight ? defaultSpacing.minimumRowHeight : 22

    implicitWidth: 40 * boxList.length

    Layout.preferredWidth: totalWidth
    width: totalWidth

    //TODO give this frame a disabled state

    Row {
        Repeater {
            model: boxList
            Rectangle {
                id: boxContainer
                width: {
                    if(evenBoxes)
                    {
                        mainFrame.width / boxList.length
                    }
                    else
                    {
                        model.modelData.width
                    }
                }

                height: mainFrame.height
                color: "transparent"


                Component.onCompleted: {
                    if(boxList[index].text != undefined)
                    {
                        totalWidth += boxList[index].width
                        boxList[index].style = invisibleStyle
                        boxList[index].horizontalAlignment = Text.AlignHCenter
                        boxList[index].parent = this
                        boxList[index].anchors.fill = boxContainer
                    }
                    else if (boxList[index].value != undefined)
                    {
                        totalWidth += boxList[index].width
                        boxList[index].hasArrows = false
                        boxList[index].textBoxStyle = invisibleStyle
                        boxList[index].horizontalAlignment = Text.AlignHCenter
                        boxList[index].parent = this
                        boxList[index].anchors.fill = boxContainer
                    }
                    else
                    {
                        boxContainer.color = "red"
                    }

                }

                WGLabel {
                    id: decimalPoint
                    anchors.horizontalCenter: parent.left
                    height: mainFrame.height - defaultSpacing.doubleBorderSize
                    y: 3
                    text: "."
                    visible: index != 0 && decimalSeparator ? true : false
                }

                WGSeparator {
                    id: internalBorder
                    anchors.horizontalCenter: parent.left
                    height: mainFrame.height - defaultSpacing.doubleBorderSize
                    anchors.verticalCenter: parent.verticalCenter
                    vertical: true

                    //first separator is invisible
                    visible: index != 0 && !decimalSeparator ? true : false
                }
            }
        }
    }
}
