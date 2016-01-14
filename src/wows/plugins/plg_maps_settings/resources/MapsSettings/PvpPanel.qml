import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls 1.0

// WGPanel triggers auto-reloading when QML file is saved
WGPanel {
    color: palette.MainWindowColor
    property var title: "PvP Settings"
    property var layoutHints: { 'test': 0.1 }
    property var topControlsHeight: 20

    // -- Data models
    // Provided from Python scripts
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
	WGListView {
		id: mapsSettingsXMLDataListView

		anchors.fill: parent

		model: xmlDataModel
		selectionExtension: xmlDataModelSelection
		// Implement a delegate for custom items in each column
		columnDelegates: [
			nameDelegate,
			nameDelegate,
			nameDelegate,
			nameDelegate,
			levelDelegate,
			levelDelegate,
			levelDelegate,
			levelDelegate,
			levelDelegate,
			levelDelegate,
			levelDelegate,
			levelDelegate,
			levelDelegate,
			levelDelegate,
			levelDelegate
			]

		Layout.minimumHeight: 50
		Layout.minimumWidth: 100
		Layout.fillHeight: true
		Layout.fillWidth: true

		property Component nameDelegate: Text {
			id: nameDelegate
			text: typeof itemData.Value == "string" ? itemData.Value : typeof itemData.Value
			color: palette.TextColor
			width: 160
		}

		property Component levelDelegate: Text {
			id: levelDelegate
			text: {
				if (typeof itemData.Value == "string")
				{
					return itemData.Value
				}
				if (typeof itemData.Value == "number")
				{
					return itemData.Value
				}
				return typeof itemData.Value
			}
			color: palette.TextColor
			width: 20
		}
		//property Component levelDelegate: WGNumberBox {
		//	id: levelDelegate
		//	number: typeof itemData.Value == "number" ? itemData.Value : -1.0
		//	width: 20
		//}
	}
}
