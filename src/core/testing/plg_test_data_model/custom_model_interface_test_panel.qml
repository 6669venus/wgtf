import QtQuick 2.1
import QtQuick.Controls 1.0

Rectangle {
	property var title: "Custom Model Interface"
	property var layoutHints: { 'test': 0.1 }

	color: palette.MainWindowColor

	Column {
		CustomModelInterfaceTestControl {
			source: Implementation1
		}

		CustomModelInterfaceTestControl {
			source: Implementation2
		}

		CustomModelInterfaceTestControl {
			source: Implementation3
		}
	}

}