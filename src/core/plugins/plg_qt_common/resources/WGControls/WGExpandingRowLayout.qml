import QtQuick 2.3
import QtQuick.Layouts 1.1

/*!
 \brief Layout of expanding controls. Needs a width or Left/Right Anchors to work properyly.

Example:
\code{.js}
WGExpandingRowLayout {
    WGNumberBox {
        width: 120
        value: 25
        minimumValue: 0
        maximumValue: 100

        b_Target: perfBar3
        b_Property: "value_"
        b_Value: value
    }

    WGPerformanceBar {
        id: perfBar3
        showValue_: true
        ranges_:[10,25,100]
    }
}
\endcode
*/

RowLayout {
    objectName: "WGExpandingRowLayout"

    /*! This property defines the row height.
        The default value is defined by defaultSpacing.minimumRowHeight
    */
    property int rowHeight_: defaultSpacing.minimumRowHeight

    /*! This property is used to define the buttons label when used in a WGFormLayout
        The default value is an empty string
    */
    //TODO: This should be renamed, it does not require "_"
    property string label_: ""

    spacing: defaultSpacing.rowSpacing
}
