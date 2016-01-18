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
            source: "WGTimelineEntryDelegate.qml"
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

    /*! Specifies the way the background is coloured, can be one of the constants:
        noBackgroundColour
        uniformRowBackgroundColours
        alternatingRowBackgroundColours */
    property int backgroundColourMode: noBackgroundColour

    /*! Colour mode with no background */
    readonly property int noBackgroundColour: 0
    /*! Colour mode with a sigle background colour */
    readonly property int uniformRowBackgroundColours: 1
    /*! Colour mode with a sigle background colour */
    readonly property int alternatingRowBackgroundColours: 2

    readonly property color backgroundColour: palette.MidDarkColor
    readonly property color alternateBackgroundColour:
        backgroundColourMode === uniformRowBackgroundColours ? backgroundColour
        : Qt.darker(palette.MidLightColor,1.2)

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
	property int defaultHandlePosition: 100

    function setCurrentIndex( modelIndexToSet ) {
        selectionExtension.currentIndex = modelIndexToSet

        // Make sure the listView has active focus, otherwise the listView's keyboard event handles won't work
        listView.forceActiveFocus()
    }

    function keyboardScroll( /* bool */ isUpward, /* bool */ calculateRows ) {

        var currentRow = listView.model.indexRow(selectionExtension.currentIndex);
        listView.positionViewAtIndex( currentRow, ListView.Contain );
    }

    Keys.onUpPressed: {
        // Handle the up key pressed event
        if (selectionExtension.moveUp()) {
            keyboardScroll(true, true);
        }
    }

    Keys.onDownPressed: {
        // Handle the down key pressed event
        if (selectionExtension.moveDown()) {
            keyboardScroll(false, true);
        }
    }

    Keys.onReturnPressed: {
        returnPressed();
    }

    /*! This signal is sent when the row is clicked.
    */
    signal rowClicked(var mouse, var modelIndex)

    /*! This signal is sent when the row is double clicked.
    */
    signal rowDoubleClicked(var mouse, var modelIndex)

    /*! This signal is sent when the Retern Key is pressed.
    */
    signal returnPressed()

    delegate: WGListViewRowDelegate {
        anchors.left: parent.left
        width: parent.width - leftMargin - rightMargin - (enableVerticalScrollBar ? verticalScrollBar.collapsedWidth : 0) - 1
        defaultColumnDelegate: listView.defaultColumnDelegate
        columnDelegates: listView.columnDelegates
        selectionExtension: listView.selectionExtension
        modelIndex: listView.model.index(rowIndex, 0)
        showBackgroundColour: backgroundColourMode !== noBackgroundColour
        backgroundColour: listView.backgroundColour
        alternateBackgroundColour: listView.alternateBackgroundColour
		handlePosition: listView.defaultHandlePosition
        hasActiveFocusDelegate: listView.activeFocus
        handlePosition: x + width / 3

        onClicked: {
            var modelIndex = listView.model.index(rowIndex, 0);
            listView.rowClicked(mouse, modelIndex);

            // Update the selectionExtension's currentIndex
            setCurrentIndex( modelIndex )
        }

        onDoubleClicked: {
            var modelIndex = listView.model.index(rowIndex, 0);
            listView.rowDoubleClicked(mouse, modelIndex);

            // Update the selectionExtension's currentIndex
            setCurrentIndex( modelIndex )
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
    property alias verticalScrollBar : verticalScrollBar
}
