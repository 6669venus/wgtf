import QtQuick 2.3
import QtQuick.Controls 1.2
import QtGraphicalEffects 1.0

//

/*!
 \brief Button with no frame until pressed or hovered. Generally intended not to have text.

\code{.js}
WGToolButton {
    checkable: true
    checkState: true
    exclusiveGroup: toolbarGroup
    iconSource: "icons/pause_16x16.png"
}
\endcode
*/

WGPushButton {
    objectName: "WGToolButton"
    implicitHeight: defaultSpacing.minimumRowHeight
    text: ""
    style: WGToolButtonStyle {}
}
