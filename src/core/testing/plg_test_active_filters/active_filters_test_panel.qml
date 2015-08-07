import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls 1.0


Rectangle {
	property var title: "Active Filters Control Test"
	property var layoutHints: { 'test': 0.1 }

	property var activeFilters_: activeFilters

	color: palette.MainWindowColor

	WGTreeFilter {
		id: treeFilter
		source: sampleDataToFilter
		filter: activeFilters_.stringValue
	}

	WGTreeModel {
		id: sampleDataTreeModel
		source: treeFilter.filteredSource

		ValueExtension {}
        ColumnExtension {}
        ComponentExtension {}
        TreeExtension {}
        ThumbnailExtension {}
		SelectionExtension {}
	}
	
	ColumnLayout {
		anchors.fill: parent

		Rectangle {
			id: activeFiltersRect
			Layout.fillWidth: true

			WGActiveFilters {
				id: activeFilters
				dataModel: filtersModel
			}
		}

		WGSeparator {
			id: separator
		}

		Rectangle {
			anchors.top: separator.bottom
			Layout.fillWidth: true
			color: "transparent"

			ColumnLayout {
				anchors.fill: parent

				Rectangle {
					Layout.fillWidth: true
					Layout.preferredHeight: 500
					color: "transparent"

					WGTreeView {
						id: sampleDataListView
						model: sampleDataTreeModel
						anchors.fill: parent
						columnDelegates : [defaultColumnDelegate]
					}
				}
			}
		}
	}
}