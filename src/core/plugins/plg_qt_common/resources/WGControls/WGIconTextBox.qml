import QtQuick 2.3
import QtQuick.Controls 1.2
import BWControls 1.0

/*  TODO: WIP? - Is this control still necessary? If not remove it.
    Was originally intended for a text box containing either a right or left aligned icon.
    Icon needs to be aliased and definable
*/

Item {
    objectName: "WGIconTextBox"

    /*! This property anchors the text field to the left if true, and to the right if false.
        The default value is \c true
    */
    property bool isLeft_ : true

    /*!
        This property contains the text that is shown in the text field when the
        text field is empty.
    */
    property alias placeholderText : textBox.placeholderText

    /*! This property is used to define the buttons label when used in a WGFormLayout
        The default value is an empty string
    */
    //TODO: This should be renamed, it does not require "_"
    property alias label_ : textBox.label_

    /*! This property holds the text to be displayed in the text field
    */
    property alias text : textBox.text

    implicitHeight: defaultSpacing.minimumRowHeight ? defaultSpacing.minimumRowHeight : 22

    TextField {
        id: textBox
        property string label_: ""

        anchors.left: isLeft_ ? icon.right : parent.left
        anchors.right: isLeft_ ? parent.right : icon.left
        anchors.leftMargin: isLeft_ ? defaultSpacing.rowSpacing : undefined
        anchors.rightMargin: isLeft_ ? undefined : defaultSpacing.rowSpacing

        activeFocusOnTab: enabled

        // support copy&paste
        WGCopyable {
            id: copyableControl

            BWCopyable {
                id: copyableObject

                onDataCopied : {
                    setValue( textBox.text )
                }

                onDataPasted : {
                    textBox.text = data
                }
            }

            onSelectedChanged : {
                if (selected)
                {
                    selectControl( copyableObject )
                }
                else
                {
                    deselectControl( copyableObject )
                }
            }
        }

        implicitHeight: defaultSpacing.minimumRowHeight ? defaultSpacing.minimumRowHeight : 22

        //Placeholder text in italics
        font.italic: text == "" ? true : false

        style: WGTextBoxStyle {

        }
    }

    Image {
        id: icon
        source: "icons/search_16x16.png"
        anchors.left: isLeft_ ? parent.left : undefined
        anchors.right: isLeft_ ? undefined : parent.right
        anchors.verticalCenter: parent.verticalCenter

        width : 16
        height: 16
    }
}
