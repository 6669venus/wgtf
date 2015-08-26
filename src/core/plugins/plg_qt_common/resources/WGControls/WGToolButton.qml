import QtQuick 2.3
import QtQuick.Controls 1.2
import QtGraphicalEffects 1.0

//

/*!
 \brief Button with no frame or text in it's default state

\code{.js}
WGToolButton {
    checkable: true
    checked: true
    exclusiveGroup: toolbarGroup
    iconSource: "icons/pause_16x16"
}
\endcode
*/

WGPushButton {
    objectName: "WGToolButton"
    noFrame_: true
    implicitHeight: defaultSpacing.minimumRowHeight ? defaultSpacing.minimumRowHeight : 22
    implicitWidth: implicitHeight
    text: ""
}
