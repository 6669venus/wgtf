import QtQuick 2.3
import QtQuick.Layouts 1.1

/*!
 \brief Two tone vertical or horizontal separator

\code{.js}
WGSeparator {
    vertical_: true
}
\endcode
*/

Rectangle {
    objectName: "WGSeparator"

    /*! This property toggles between a vertical and horizontal control
        The default value is \c false
    */
    //TODO: This should be renamed, it does not require "_"
    property bool vertical_: false

    width: vertical_ ? defaultSpacing.separatorWidth : undefined
    height: vertical_ ? undefined : defaultSpacing.separatorWidth

    Layout.fillWidth: vertical_ ? false : true
    Layout.fillHeight: vertical_ ? true : false

    Layout.maximumWidth: vertical_ ? defaultSpacing.separatorWidth : Number.POSITIVE_INFINITY
    Layout.maximumHeight: vertical_ ? Number.POSITIVE_INFINITY : defaultSpacing.separatorWidth

    color: "transparent"

    Rectangle {
        width: vertical_ ? defaultSpacing.separatorWidth / 2 : parent.width
        height: vertical_ ? parent.height : defaultSpacing.separatorWidth / 2
        color: palette.darkerShade
        anchors.left: parent.left
        anchors.top: parent.top
    }

    Rectangle {
        width: vertical_ ? defaultSpacing.separatorWidth / 2 : parent.width
        height: vertical_ ? parent.height : defaultSpacing.separatorWidth / 2
        color: palette.lighterShade
        anchors.right: parent.right
        anchors.bottom: parent.bottom
    }
}
