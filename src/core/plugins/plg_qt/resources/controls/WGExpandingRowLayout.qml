import QtQuick 2.0
import QtQuick.Layouts 1.1

//Layout of expanding controls. Needs a width or Left/Right Anchors to work properyly.

RowLayout {
    spacing: panelProps.rowSpacing_
    property int rowHeight_: panelProps.rowHeight_
    property string label_: ""
}
