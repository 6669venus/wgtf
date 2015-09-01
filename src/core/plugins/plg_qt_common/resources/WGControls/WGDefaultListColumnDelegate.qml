import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import BWControls 1.0
import WGControls 1.0

/*!
    \brief Default component for displaying each column in a WGMultiColumnListView made obsolete by WGListView.
    Documentation incomplete due to obsolete status
*/

Item {
    objectName: "WGDefaultListColumnDelegate"
    Layout.fillWidth: true
    Layout.preferredHeight: defaultSpacing.minimumRowHeight

    Image {
        id: thumbnail
        source: typeof itemData.decoration != "undefined" ?
            itemData.decoration : ""
    }

    Text {
        id: value
        x: thumbnail.x + thumbnail.width
        clip: false
        text: typeof itemData.Value == "string" ?
            itemData.Value : typeof itemData.Value
        color: palette.TextColor
    }
}
