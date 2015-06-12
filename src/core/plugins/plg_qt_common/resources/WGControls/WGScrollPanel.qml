import QtQuick 2.3
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

	property bool vertical: true

    Flickable {
        id: scrollPanel

        anchors.fill: parent

		anchors.leftMargin: panelProps.leftMargin_
		anchors.rightMargin: panelProps.rightMargin_
		anchors.topMargin: panelProps.topBottomMargin_
		anchors.bottomMargin: panelProps.topBottomMargin_

		flickableDirection: scrollableFrame.vertical ? Flickable.VerticalFlick : Flickable.HorizontalFlick
        boundsBehavior: Flickable.StopAtBounds

        contentHeight: scrollContent.height
        contentWidth: scrollContent.width

        Loader {
            id: scrollContent

			width: scrollableFrame.vertical ? scrollableFrame.width - panelProps.leftMargin_ - panelProps.rightMargin_ : undefined
			height: !scrollableFrame.vertical ? scrollableFrame.height : undefined

			//x: panelProps.leftMargin_
			//y: panelProps.topBottomMargin_

            sourceComponent: childObject_
        }

    }

    WGScrollBar {
         id: verticalScrollBar
		 width: scrollableFrame.vertical ? panelProps.rightMargin_ : scrollPanel.width
		 height: !scrollableFrame.vertical ? panelProps.topBottomMargin_ : scrollPanel.height
		 anchors.left: scrollableFrame.vertical ? scrollPanel.right : undefined
		 anchors.bottom: !scrollableFrame.vertical ? scrollPanel.bottom: undefined
		 orientation: scrollableFrame.vertical ? Qt.Vertical : Qt.Horizontal
		 position: scrollableFrame.vertical ? scrollPanel.visibleArea.yPosition : scrollPanel.visibleArea.xPosition
		 pageSize: scrollableFrame.vertical ? scrollPanel.visibleArea.heightRatio : scrollPanel.visibleArea.widthRatio

		 visible: scrollableFrame.vertical ? scrollContent.height > scrollPanel.height : scrollContent.width > scrollPanel.width
     }
}
