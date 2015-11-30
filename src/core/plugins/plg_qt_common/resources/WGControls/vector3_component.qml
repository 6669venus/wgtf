import QtQuick 2.3
import QtQuick.Layouts 1.1
import BWControls 1.0

//TODO:
// Would be nice if this just inherited vector4_component and turned off the last value
// but doing so caused a major crash.
// Need to add in proper min/max values and required decimal points.


WGExpandingRowLayout {
    anchors.fill: parent
    height: parent.height

    WGExpandingRowLayout{
        Layout.fillWidth: true
        // preferred controls the weight of two competing layout.fillWidth objects
        Layout.preferredWidth: 5
        Layout.minimumWidth: 30
        Layout.preferredHeight: parent.height
        spacing: 0

        WGLabel{
            id: labelX
            Layout.preferredWidth: paintedWidth
            Layout.minimumWidth: paintedWidth
            Layout.preferredHeight: parent.height
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            text: "X:"
            horizontalAlignment: Text.AlignRight
            verticalAlignment: Text.AlignVCenter
        }
        Item {
            Layout.preferredHeight: parent.height
            Layout.preferredWidth: 1
            Layout.minimumWidth: 0
            Layout.maximumWidth: 5
            Layout.fillWidth: true
        }
        WGNumberBox {
            id: numboxX
            Layout.fillWidth: true
            Layout.minimumWidth: 24
            Layout.preferredHeight: parent.height
            Layout.preferredWidth: 2
            number: itemData.Value.x
            maximumValue: 2147483647
            minimumValue: -2147483647
		    stepSize: 0.1
		    decimals: 3

            onNumberChanged: {
                itemData.Value.x = number
            }
        }
    }

    WGExpandingRowLayout {
        Layout.maximumWidth: 10
        Layout.preferredWidth: 1
        Layout.minimumWidth: 0
        Layout.preferredHeight: parent.height
        spacing: 0
        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: parent.height
        }
    }

    WGExpandingRowLayout{
        Layout.fillWidth: true
        Layout.minimumWidth: 30
        Layout.preferredWidth: 5
        Layout.preferredHeight: parent.height
        spacing: 0

        WGLabel{
            id: labelY
            Layout.preferredWidth: paintedWidth
            Layout.minimumWidth: paintedWidth
            Layout.preferredHeight: parent.height
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            text: "Y:"
            horizontalAlignment: Text.AlignRight
            verticalAlignment: Text.AlignVCenter
        }
        Item {
            Layout.preferredHeight: parent.height
            Layout.preferredWidth: 1
            Layout.maximumWidth: 5
            Layout.minimumWidth: 0
            Layout.fillWidth: true
        }
        WGNumberBox {
            id: numboxY
            Layout.fillWidth: true
            Layout.preferredHeight: parent.height
            Layout.preferredWidth: 2
            Layout.minimumWidth: 24
            number: itemData.Value.y
            maximumValue: 2147483647
            minimumValue: -2147483647
		    stepSize: 0.1
	    	decimals: 3

            onNumberChanged: {
                itemData.Value.y = number
            }
        }
    }

    WGExpandingRowLayout {
        Layout.maximumWidth: 10
        Layout.preferredWidth: 1
        Layout.minimumWidth: 0
        Layout.preferredHeight: parent.height
        spacing: 0
        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: parent.height
        }
    }

    WGExpandingRowLayout{
        Layout.fillWidth: true
        Layout.minimumWidth: 30
        Layout.preferredWidth: 5
        Layout.preferredHeight: parent.height
        spacing: 0
        WGLabel{
            id: labelZ
            Layout.preferredWidth: paintedWidth
            Layout.minimumWidth: paintedWidth
            Layout.preferredHeight: parent.height
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            text: "Z:"
            horizontalAlignment: Text.AlignRight
            verticalAlignment: Text.AlignVCenter
        }
        Item {
            Layout.preferredHeight: parent.height
            Layout.preferredWidth: 1
            Layout.minimumWidth: 0
            Layout.maximumWidth: 5
            Layout.fillWidth: true
        }
        WGNumberBox {
            id: numboxZ
            Layout.fillWidth: true
            Layout.preferredWidth: 2
            Layout.preferredHeight: parent.height
            Layout.minimumWidth: 24
            number: itemData.Value.z
            maximumValue: 2147483647
            minimumValue: -2147483647
	    	stepSize: 0.1
		    decimals: 3

            onNumberChanged: {
                itemData.Value.z = number
            }
        }
    }
}
