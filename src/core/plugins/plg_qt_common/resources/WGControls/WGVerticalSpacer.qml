import QtQuick 2.3

/*!
 \brief An invisible box that can have height set by number of lineSpaces_

Example:
\code{.js}
WGVerticalSpacer{
    anchors{left: parent.left; right:parent.right;}
}
\endcode
*/

Rectangle {
    objectName: "WGVerticalSpacer"

    anchors {left: parent.left; right: parent.right}
    color: "transparent"

    /*!
        This property defines how high the spacer should be in line spaces
        The default value is an empty 1
    */
    //TODO: This should be renamed, it does not require "_"
    property int lineSpaces_: 1

    implicitHeight: defaultSpacing.minimumRowHeight * lineSpaces_

    /*! This property is used to define the buttons label when used in a WGFormLayout
        The default value is an empty string
    */
    //TODO: This should be renamed, it does not require "_"
    property string label_: ""
}
