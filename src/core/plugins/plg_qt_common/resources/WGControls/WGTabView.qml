import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

TabView {
    style: WGTabViewStyle{}    
    // Requires QtQuick.Controls 1.3 leaving commented out until Qt5.5 utilized.
    // contentItem.anchors.leftMargin: defaultSpacing.leftMargin
    // contentItem.anchors.rightMargin: defaultSpacing.rightMargin
    // meanwhile tabview anchors will be set individually
    //Component.onCompleted: console.log(getTab(0))
}

