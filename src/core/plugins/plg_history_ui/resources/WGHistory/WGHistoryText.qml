import QtQuick 2.1

Text {
	id: text

	// -- Begin Interface
	text: "Unknown"
	property bool isCurrentItem: false
	property bool isApplied: false
	// -- End Interface

	clip: false
	color: isCurrentItem ?
		palette.HighlightTextColor :
		palette.TextColor
	font.bold: isCurrentItem
	font.italic: !isApplied
	opacity: isApplied ? 1 : 0.35
}

