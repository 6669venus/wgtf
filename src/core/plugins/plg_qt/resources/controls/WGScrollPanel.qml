import QtQuick 2.0
import QtQuick.Controls 1.2

//Scrolling Panel. Recommend childObject_ be a WGColumnLayout or similar
//Use this class for a scroll panel intended to fill the area, eg. the ApplicationWindow

//Use WGSubScrollPanel if the parent depends on this height eg in a WGSubPanel
//Use WGColumnScrollPanel if you cannot anchor to top or bottom eg in a WGColumnLayout

Item {
    id: scrollableFrame

    //not anchors.fill because of inherited classes reassigning top & bottom
    anchors {left: parent.left; right: parent.right; top: parent.top; bottom: parent.bottom}

    property Component childObject_

    Flickable {
        id: scrollPanel

        anchors.fill: parent

        flickableDirection: Flickable.VerticalFlick
        boundsBehavior: Flickable.StopAtBounds

        contentHeight: scrollContent.height
        contentWidth: scrollContent.width

        Loader {
            id: scrollContent

            width: scrollableFrame.width - panelProps.leftMargin_ - panelProps.rightMargin_
            x: panelProps.leftMargin_
            y: panelProps.topBottomMargin_

            sourceComponent: childObject_
        }

    }

    WGScrollBar {
         id: verticalScrollBar
         width: panelProps.rightMargin_;
         height: scrollPanel.height
         anchors.right: scrollPanel.right
         orientation: Qt.Vertical
         position: scrollPanel.visibleArea.yPosition
         pageSize: scrollPanel.visibleArea.heightRatio

         visible: scrollContent.height > scrollPanel.height
     }
}
