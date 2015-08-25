import QtQuick 2.3
import QtQuick.Layouts 1.1

//Layout of expanding controls. Needs a width or Left/Right Anchors to work properyly.

RowLayout {
    spacing: defaultSpacing.rowSpacing
    property int rowHeight_: defaultSpacing.minimumRowHeight
    property string label_: ""
}
