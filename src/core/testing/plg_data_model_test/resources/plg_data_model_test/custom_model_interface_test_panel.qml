import QtQuick 2.1
import QtQuick.Controls 1.0

Rectangle {
	property var viewId: "78AE2195-1DC6-4D69-93BF-1663C58D2354"
	property var title: "Custom Model Interface"
	property var layoutHints: { 'test': 0.1 }

	color: palette.MainWindowColor

	Column {
		id: original
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


	Column {
		id: clones
		y: original.y + original.height + 20
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