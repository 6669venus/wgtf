import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import QtQuick.Window 2.1
import WGControls 1.0

WGPanel {
	id: mainFrame

	title: "Asset Browser"

	layoutHints: { 'assetbrowser': 1.0, 'bottom': 0.5 }

	color: palette.MainWindowColor	

	Layout.fillWidth: true
	Layout.fillHeight: true

	WGAssetBrowser {
		id: testAssetBrowserControl
		viewModel: view
	}
}
