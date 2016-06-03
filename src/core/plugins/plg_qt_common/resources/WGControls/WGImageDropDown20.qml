import QtQuick 2.6
import QtQuick.Controls 1.2
import Qt.labs.controls 1.0 as Labs
import Qt.labs.templates 1.0 as T
import QtQuick.Layouts 1.3
import WGControls 2.0

/*!
 \  brief A Qt.Labs Drop Down box with styleable menu that only shows an image in its collapsed state

Example:
\code{.js}
WGDropDownBox {
    id: dropDown

    textRole: "label"
    imageRole: "icon"

    model: ListModel {
        ListElement { label: "Option 1"; icon: "icons/icon1.png"}
        ListElement { label: "Option 2"; icon: "icons/icon2.png"}
        ListElement { label: "Option 3"; icon: "icons/icon3.png"}
    }
}
\endcode
*/

WGDropDownBox {
    id: control
    objectName: "WGDropDownBox"

    implicitWidth: defaultSpacing.doubleMargin + control.height + (showDropDownIndicator ? defaultSpacing.doubleMargin + defaultSpacing.standardMargin : 0)

    showDropDownIndicator: false

    contentItem: Image {
        id: contentImage
        anchors.verticalCenter: parent.verticalCenter
        source: control.imageRole ? model.get(control.currentIndex)[control.imageRole] : ""
        visible: control.imageRole
        height: Math.min(sourceSize.height,control.height - defaultSpacing.doubleBorderSize)
        width: height
    }
}
