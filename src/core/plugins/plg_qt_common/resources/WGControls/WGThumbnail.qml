import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2

// Thumbnail with default size and boder
// Use WGThumbnailButton if thumbnail is editable

Rectangle {
    id: thumbnail
    color: palette.LightShade
    //color: QColor(255,255,255,16)
    property url source_: ""

    implicitWidth: {
        defaultSpacing.minimumRowHeight * 4
    }

    implicitHeight: {
        defaultSpacing.minimumRowHeight * 4
    }

    Item {
        anchors.fill: parent
        Image {
            id: icon
            anchors.fill: parent            
            anchors.margins: defaultSpacing.standardBorderSize
            source: thumbnail.source_
        }
    }
}
