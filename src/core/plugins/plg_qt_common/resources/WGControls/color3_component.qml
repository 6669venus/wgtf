import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import BWControls 1.0

Loader
{
	source:"color4_component.qml"
	onLoaded:{
		item.showAlpha = false;
	}
}
