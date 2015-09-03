import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

/*!
 \brief WGListView displays data from a model defined by its delegate.
 The WGListView is contructed of rows of WGListViewRowDelegates which in turn hold WGListViewColumnDelegates.

Example:
\code{.js}

WGListView {
    id: history
    model: historyModel
    anchors.fill: parent
    anchors.margins: defaultSpacing.standardMargin
    selectionExtension: root.historySelectionExtension
    columnDelegates: [columnDelegate]

    Component {
        id: columnDelegate

        Loader {
            source: "qrc:///plg_history_ui/WGTimelineEntryDelegate.qml"
        }
    }

    onCurrentIndexChanged: {
        historySelection.data = currentIndex
    }

    onRowDoubleClicked: {
        history.currentIndex = historyModel.indexRow(modelIndex);
    }
}

\endcode
*/

ListView {
    id: listView
    objectName: "WGListView"
    boundsBehavior: Flickable.StopAtBounds
    currentIndex: -1
    clip: true
    leftMargin: 2
    rightMargin: 2
    topMargin: 2
    bottomMargin: 2
    spacing: 0

    //TODO: Document this. Should this be internal?
    /*! This property holds multi select state information */
    property var selectionExtension: null

    /*! This property holds a lits of items to be passed to the WGListViewRowDelegate
        The default value is an empty list
    */
    property var columnDelegates: []

    property var treeExtension: null

    /*! This property holds the spacing between rows
        The default value is set by defaultSpacing.minimumRowHeight
    */
    property real minimumRowHeight: defaultSpacing.minimumRowHeight

    /*! This property defines the anchors.margins used by the selection highlight
        The default value is \c 0
    */
    property real selectionMargin: 0

    /*! This property toggles addition of a WGScrollBar to the list view
        The default value is \c true
    */
    property bool enableVerticalScrollBar: true

    /*! This property holds the spacing between column items
        The default value is \c 1
    */
    property real columnSpacing: 1

    //TODO: document this
    /*! This property holds a default list component. */
    property var defaultColumnDelegate: Component {
        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: minimumRowHeight

        Text {
                id: value
                clip: true
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.margins: 4
                verticalAlignment: Text.AlignVCenter
                text: typeof itemData.Value === "string" ? itemData.Value : typeof itemData.Value
                color: palette.TextColor
            }
        }
    }

    /*! This signal is sent when the row is clicked.
    */
    signal rowClicked(var mouse, var modelIndex)

    /*! This signal is sent when the row is double clicked.
    */
    signal rowDoubleClicked(var mouse, var modelIndex)

    delegate: WGListViewRowDelegate {
        anchors.left: parent.left
        width: parent.width - leftMargin - rightMargin - (enableVerticalScrollBar ? verticalScrollBar.collapsedWidth : 0) - 1
        defaultColumnDelegate: listView.defaultColumnDelegate
        columnDelegates: listView.columnDelegates
        selectionExtension: listView.selectionExtension

        onClicked: {
            var modelIndex = listView.model.index(rowIndex, 0);
            listView.rowClicked(mouse, modelIndex);
        }

        onDoubleClicked: {
            var modelIndex = listView.model.index(rowIndex, 0);
            listView.rowDoubleClicked(mouse, modelIndex);
        }
    }

    WGScrollBar {
        id: verticalScrollBar
        width: defaultSpacing.rightMargin
        anchors.top: listView.top
        anchors.right: listView.right
        anchors.bottom: listView.bottom
        orientation: Qt.Vertical
        position: listView.visibleArea.yPosition
        pageSize: listView.visibleArea.heightRatio
        scrollFlickable: listView
        visible: listView.contentHeight > listView.height && enableVerticalScrollBar
    }
}
