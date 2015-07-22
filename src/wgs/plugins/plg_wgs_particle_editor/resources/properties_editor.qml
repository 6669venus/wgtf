import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import BWControls 1.0
import WGControls 1.0

ColumnLayout 
{
	property var title: "Properties"
	property var layoutHints: { 'properties': 1.0, 'left': 0.5 }
    spacing: 0

    Rectangle 
	{
        color: palette.DarkColor
        Layout.fillWidth: true
        Layout.fillHeight: true
    }
}
