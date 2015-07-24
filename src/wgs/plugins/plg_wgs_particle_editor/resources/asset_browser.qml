import QtQuick 2.3
import WGControls 1.0

Rectangle {
	id: mainFrame

	property var title: "Asset Browser"
	property var layoutHints: { 'assetbrowser': 1.0, 'bottom': 0.5 }

	color: palette.MainWindowColor

	WGAssetBrowser {
		id: assetBrowser
		objectName: particelEditorAssetBrowser
		sourceModel: source
	}
}