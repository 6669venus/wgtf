import QtQuick 2.3
import QtQuick.Controls 1.2
import WGControls 1.0
import BWControls 1.0

WGDropDownBox {
	id: combobox
	anchors.left: parent.left
	anchors.right: parent.right

	WGListModel {
		id: polyModel
		ValueExtension {}
	}

	BWDataChangeNotifier {
		id: comboboxSelection
		// When the model changes, update the combobox on the view
		onSourceChanged: {
			combobox.currentIndex = data
		}
		onDataChanged: {
			combobox.currentIndex = data
		}
	}
	Component.onCompleted: {
		polyModel.source = itemData.DefinitionModel
		comboboxSelection.source = itemData.ModelValue
		comboboxSelection.data = currentIndex;
	}

	model: polyModel
	textRole: "display"
	onCurrentIndexChanged: {
		comboboxSelection.data = currentIndex;
	}
}
