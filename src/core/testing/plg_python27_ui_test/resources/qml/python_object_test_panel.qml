import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls 1.0

Rectangle {
    color: palette.MainWindowColor
	property var title: "Python Test"
    property var layoutHints: { 'test': 0.1 }
    property var sourceModel: source
}
