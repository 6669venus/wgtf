import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

/*!
 \brief Multi-column ListView
*/

//TODO: Requires example code

ListView {
    id: listView
    objectName: "WGMultiColumnListview"

    /*! This property determines the default component for displaying each row
        The default value is \c WGDefaultListRowDelegate.qml
    */
    property string defaultRowDelegate: "WGDefaultListRowDelegate.qml"

    /*! This property determines the default component for displaying each column
        The default value is \c WGDefaultListColumnDelegate.qml
    */
    property string defaultColumnDelegate: "WGDefaultListColumnDelegate.qml"

    /*! Fill this list with custom components for displaying each column.
        ColumnDelegates will override defaultDelegate
        The default value is an empty list
    */
    property variant columnDelegates: []

    //Previously clicked row for multi-select
    /*! \internal */
    property int lastIndexClicked: -1

    delegate: Loader {
        id: rowDelegateLoader

        // -- Begin Interface
        property bool isCurrentItem: ListView.isCurrentItem
        property int rowIndex: index
        property int lastIndexClicked: listView.lastIndexClicked
        // -- End Interface

        source: defaultRowDelegate

        // Stretch width to parent
        anchors.left: parent.left
        anchors.right: parent.right
        // Height is determined by row
    }
}
