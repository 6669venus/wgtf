import QtQuick 2.4
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import BWControls 1.0
import WGControls 1.0 as WG1
import WGControls 2.0

WG1.WGPanel {
    title: "TreeModel Test"
    layoutHints: { 'test': 0.1 }

    property var sourceModel: useModel ? source : null
	color: palette.mainWindowColor

	property var useModel: 1
	property var topControlsHeight: 20

	Button {
		id: switchModelButton
		anchors.top: parent.top
		anchors.left: parent.left
		width: 150
		height: topControlsHeight
		text: useModel ? "Switch Model Off" : "Switch Model On"

		onClicked: {
			useModel = useModel == 0 ? 1 : 0;
		}
	}

	ScrollView {
		anchors.top: switchModelButton.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom

		WGTreeView {
			model: sourceModel
			columnSpacing: 1
			columnSequence: [0,0]
            showColumnHeaders: true
            showColumnFooters: true
            roles: ["headerText", "footerText"]
		}
	}
}
