import QtQuick 2.3

//TODO: Remove this contorl. It shouldn't be in use anymore and has been replaced with values in c++

//contains the default spacing settings for the panel
//Should probably be replaced with some kind of global app settings

Item {
    readonly property int rowHeight_: 22
    readonly property int rowSpacing_: 3

    readonly property int leftMargin_: 10
    readonly property int rightMargin_: 10

    readonly property int scrollBarWidth_: (rightMargin_ - rowSpacing_ * 2)

    readonly property int topBottomMargin_: 3

    readonly property int separatorWidth_: 2

    readonly property int childIndent_: 0

    readonly property int minPanelWidth_: 400

    readonly property int standardBorder_: 1 //standard frame border width
    readonly property int doubleBorder_: standardBorder_ * 2

    readonly property int standardRadius_: 4 //standard frame radius
    readonly property int halfRadius_: standardRadius_ / 2

    readonly property int standardMargin_: standardBorder_ + standardRadius_ //default internal spacing
    readonly property int doubleMargin_: standardMargin_ * 2

    //TabViewStyle properties
    property int tabHeight_: 22
    property int tabHeightDiff_: 2
    property int tabMinWidth_: 60
    property int tabSpacer_: 0
    property int tabTextSpacer_: 10

    property int labelColumnWidth_: 0

    property bool dragUnlocked_: true

    property bool copyEnabled_: false

    property var copiedData_: ""
    property QtObject copiedObject_: null

}
