import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls 1.0

// WGPanel triggers auto-reloading when QML file is saved
WGPanel {
    id: mapsSettingsPanel
    color: palette.MainWindowColor
    property var title: "Maps Settings"
    property var layoutHints: { 'test': 0.1 }
    property var topControlsHeight: 20

    // -- Data models
    // Python objects provide the data model for the tree view
    WGListModel {
        id: xmlDataModel
        source: mapsSettingsXMLData

        ValueExtension {}
        ColumnExtension {}
        ComponentExtension {}
        SelectionExtension {
            id: xmlDataModelSelection
            multiSelect: false
        }
    }


    // -- View
    WGColumnLayout {
        id: mainColumnLayout
        anchors.fill: parent

        // Refresh button for debugging - refreshes entire tree
        WGPushButton {
            id: refreshButton
            height: topControlsHeight
            text: "Refresh"

            onClicked: {
                // Fire signal to update UI
                mapsSettingsXMLDataChanged( mapsSettingsXMLData );
            }
        }

        WGListView {
            id: mapsSettingsXMLDataListView

            model: xmlDataModel
            selectionExtension: xmlDataModelSelection
            // Implement a delegate for custom items in each column
            columnDelegates: [propertyDelegate]

            Layout.minimumHeight: 50
            Layout.minimumWidth: 100
            Layout.fillHeight: true
            Layout.fillWidth: true

			minimumRowHeight: 700

			property Component propertyDelegate: Loader {
				id: propertyDelegate
				source: "SpaceEntryDelegate.qml"
            }
        }
    }
}
