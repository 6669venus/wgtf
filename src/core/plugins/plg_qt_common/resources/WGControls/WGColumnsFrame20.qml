import QtQuick 2.0

/*!
 \brief Controls behaviour for resizable column widths and spacing.
 Adds the mouse handles between columns.
 */
Row {
    id: columnsFrame
    width: childrenRect.width

    property var columnWidths: []
    property var implicitColumnWidths: []
    property real columnSpacing: 0
    property real availableWidth: 0
    property bool clamp: false

    onAvailableWidthChanged: {
        internal.jostleColumnWidths(-1);
    }

    Component.onCompleted: {
        internal.initialColumnWidths = columnWidths
    }

    Repeater {
        id: handles
        model: internal.initialColumnWidths

        Item {
            width: handle.x + handle.width
            height: columnsFrame.height

            Item {
                id: handle
                x: internal.initialColumnWidths[index]
                width: columnSpacing
                height: columnsFrame.height

                /* MOVE INTO STYLE*/
                Rectangle {
                    anchors.fill: parent
                    color: palette.darkColor
                }
                /**/

                MouseArea {
                    id: handleMouseArea
                    anchors.verticalCenter: parent.verticalCenter
                    width: handle.width + 1
                    height: handle.height
                    cursorShape: Qt.SplitHCursor

                    drag.target: handle
                    drag.threshold: 0
                    drag.minimumX: 1
                    drag.axis: Drag.XAxis

                    onDoubleClicked: {
                        parent.x = Math.max(drag.minimumX, implicitColumnWidths[index]);
                        internal.jostleColumnWidths(index);
                    }
                }

                onXChanged: {
                    var widths = columnWidths
                    widths[index] = x
                    columnWidths = widths

                    if (handleMouseArea.drag.active) {
                        internal.jostleColumnWidths(index);
                    }
                }
            }
        }
    }

    QtObject {
        id: internal
        property var initialColumnWidths: []
        property real previousAvailableWidth: 0

        function jostleColumnWidths(adjustedIndex) {
            if (!clamp) {
                return;
            }

            var columnCount = columnWidths.length
            if (columnCount === 0 || handles.count === 0) {
                return
            }

            var currentWidth = 0
            for (var i = 0; i < columnCount; ++i) {
                currentWidth += columnWidths[i] + columnSpacing
            }
            if (previousAvailableWidth == 0) {
                previousAvailableWidth = currentWidth
            }
            var totalWidthDelta = availableWidth - previousAvailableWidth
            if (totalWidthDelta < 0 && availableWidth >= currentWidth) {
                previousAvailableWidth = availableWidth
                return
            }

            while (totalWidthDelta != 0) {
                var resizeCount = columnCount
                if (totalWidthDelta < 0) {
                    for (var i = 0; i < columnCount; ++i) {
                        if (columnWidths[i] == 1) {
                            --resizeCount
                        }
                    }
                }

                var desiredWidthDelta = totalWidthDelta / resizeCount
                for (var i = 0; i < columnCount; ++i) {
                    var widthDelta = Math.max(1 - columnWidths[i], desiredWidthDelta)
                    if (widthDelta == 0) {
                        continue
                    }

                    var handle = handles.itemAt(i).children[0]
                    handle.x += widthDelta
                    totalWidthDelta -= widthDelta
                }
            }
            previousAvailableWidth = availableWidth
        }
    }
}
