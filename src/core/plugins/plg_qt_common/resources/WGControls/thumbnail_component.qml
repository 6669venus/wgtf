import QtQuick 2.3
import QtQuick.Controls 1.2

Item {
    objectName:  itemData != null ? itemData.IndexPath : "thumbnail_component"
    WGThumbnail {
        objectName: "thumbnail"
        source_: itemData.Thumbnail
        anchors.left: parent.left
        height: defaultSpacing.minimumRowHeight
        width: defaultSpacing.minimumRowHeight
    }
}


