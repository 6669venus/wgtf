import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import BWControls 1.0

Loader
{	
	id: color3_Loader
	Component.onCompleted: {
		color3_Loader.setSource("color4_component.qml", { "showAlpha": false });
	}
}
