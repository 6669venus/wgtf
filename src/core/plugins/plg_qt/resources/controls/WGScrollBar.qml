import QtQuick 2.0

Item {
     id: scrollBar

     // The properties that define the scrollbar's state.
     // position and pageSize are in the range 0.0 - 1.0.  They are relative to the
     // height of the page, i.e. a pageSize of 0.5 means that you can see 50%
     // of the height of the view.
     // orientation can be either Qt.Vertical or Qt.Horizontal

     property real position
     property real pageSize
     property variant orientation : Qt.Vertical

     // A light, semi-transparent background

     Rectangle {
        id: background
        anchors.horizontalCenter: orientation == Qt.Vertical ? parent.horizontalCenter : undefined
        anchors.verticalCenter: orientation == Qt.Vertical ? undefined : parent.verticalCenter

        anchors.horizontalCenterOffset: orientation == Qt.Vertical ? panelProps.standardBorder_ : 0
        anchors.verticalCenterOffset: orientation == Qt.Vertical ? 0 : panelProps.standardBorder_

        anchors.top: orientation == Qt.Vertical ? parent.top : undefined
        anchors.bottom: orientation == Qt.Vertical ? parent.bottom : undefined

        anchors.left: orientation == Qt.Vertical ? undefined : parent.left
        anchors.right: orientation == Qt.Vertical ? undefined : parent.right

        width: orientation == Qt.Vertical ? panelProps.scrollBarWidth_ : undefined
        height: orientation == Qt.Vertical ? undefined : panelProps.scrollBarWidth_

        color: palette.LighterShade

        border.width: panelProps.standardBorder_
        border.color: palette.DarkestShade
     }

     // Size the bar to the required size, depending upon the orientation.
     Rectangle {

        x: orientation == Qt.Vertical ? 0 : (scrollBar.position * (scrollBar.width))
        y: orientation == Qt.Vertical ? (scrollBar.position * (scrollBar.height)) : 0

        width: orientation == Qt.Vertical ? panelProps.scrollBarWidth_ : (scrollBar.pageSize * scrollBar.width)
        height: orientation == Qt.Vertical ? (scrollBar.pageSize * scrollBar.height) : panelProps.scrollBarWidth_

        anchors.horizontalCenter: orientation == Qt.Vertical ? parent.horizontalCenter : undefined
        anchors.verticalCenter: orientation == Qt.Vertical ? undefined : parent.verticalCenter

        anchors.horizontalCenterOffset: orientation == Qt.Vertical ? panelProps.standardBorder_ : 0
        anchors.verticalCenterOffset: orientation == Qt.Vertical ? 0 : panelProps.standardBorder_

        color: palette.HighlightColor
        border.width: panelProps.standardBorder_
        border.color: palette.DarkHeaderColor
     }
 }
