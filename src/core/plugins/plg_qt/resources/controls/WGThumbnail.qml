import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Dialogs 1.2

// Thumbnail with default size and boder
// Use WGThumbnailButton if thumbnail is editable

Rectangle {
    id: thumbnail
    color: palette.LightShade
    //color: QColor(255,255,255,16)
    property url source_: ""

    implicitWidth: {
        panelProps.rowHeight_ * 4
    }

    implicitHeight: {
        panelProps.rowHeight_ * 4
    }

    Item {
        anchors.fill: parent
        Image {
            id: icon
            anchors.fill: parent            
            anchors.margins: panelProps.standardBorder_
            source: thumbnail.source_
        }
    }
}
