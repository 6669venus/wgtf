import QtQuick 2.3

// TODO: Delete this control. Users should use WGExpandingRowLayout instead.
// When a Row is required just use the base Row control.

/*!
 \brief Users should use WGExpandingRowLayout instead.
*/

Row {
    spacing: defaultSpacing.rowSpacing
    property int rowHeight_: defaultSpacing.minimumRowHeight
    property string label_: ""
}
