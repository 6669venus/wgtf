import QtQuick 2.3
import QtQuick.Controls 1.2
import WGControls 2.0

WGTreeView {
	ComponentExtension {
		id: componentExtension
	}

	extensions: [componentExtension]

	property Component propertyDelegate: Loader {
		width: itemWidth
		height: 22
        sourceComponent: itemData.component
    }

	columnDelegates: [columnDelegate, propertyDelegate]
	columnSequence: [0, 0]
	columnSpacing: 1

	roles: [
		"Value",
		"ValueType",
		"EnumModel",
		"Definition",
		"DefinitionModel",
		"Object",
		"RootObject",
		"Key",
		"KeyType",
		"MinValue",
		"MaxValue",
		"StepSize",
		"Decimals",
		"IndexPath",
		"UrlIsAssetBrowser",
		"UrlDialogTitle",
		"UrlDialogDefaultFolder",
		"UrlDialogNameFilters",
		"UrlDialogSelectedNameFilter",
		"UrlDialogModality",
		"IsReadOnly",
		"Thumbnail"
	]
}
