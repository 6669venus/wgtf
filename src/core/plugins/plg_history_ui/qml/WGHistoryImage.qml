import QtQuick 2.1

Image {
	id: thumbnail

	// -- Begin Interface
	property string type: undefined
	property bool isApplied: false
	// -- End Interface

	source: {
		if (typeof type == 'undefined') {
			return "qrc:///icons/unknown";
		}
		if (type == "Batch") {
			return "qrc:///icons/add";
		}
		return "qrc:///icons/edit_16x16";
	}
	opacity: isApplied ? 1 : 0.35
}

