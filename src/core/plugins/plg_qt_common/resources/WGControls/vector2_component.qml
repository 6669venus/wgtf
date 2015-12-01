import QtQuick 2.3
import QtQuick.Layouts 1.1
import BWControls 1.0

//TODO:
// Would be nice if this just inherited vector4_component and turned off the last two value
// but doing so caused a major crash.
// Need to add in proper min/max values and required decimal points.

WGExpandingRowLayout {
    id: mainLayout
    anchors.fill: parent
    height: parent.height
    spacing: 0

    WGExpandingRowLayout {
        id: layoutX
        Layout.preferredHeight: parent.height

        Layout.fillWidth: true
        Layout.preferredWidth: mainLayout.width / 2
        implicitWidth:  numboxX.implicitWidth + labelX.width
        Layout.minimumWidth: numboxX.implicitWidth + labelX.width
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
            Layout.preferredWidth: defaultSpacing.standardMargin
            Layout.minimumWidth: 0
            Layout.maximumWidth: defaultSpacing.standardMargin
            Layout.fillWidth: true
        }

        WGNumberBox {
            id: numboxX
            Layout.fillWidth: true
            Layout.preferredHeight: parent.height

            Layout.preferredWidth: mainLayout.width / 2
            implicitWidth:  contentWidth + defaultSpacing.doubleMargin
            Layout.minimumWidth: numboxX.implicitWidth

            number: itemData.Value.x
            maximumValue: 2147483647
            minimumValue: -2147483647

            onNumberChanged: {
                itemData.Value.x = number
            }
        }
    }

    WGExpandingRowLayout {
        Layout.maximumWidth: defaultSpacing.standardMargin
        Layout.preferredWidth: defaultSpacing.standardMargin
        Layout.minimumWidth: 0
        Layout.preferredHeight: parent.height
        spacing: 0
        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: parent.height
        }
    }

    WGExpandingRowLayout {
        id: layoutY
        Layout.preferredHeight: parent.height

        Layout.fillWidth: true
        Layout.preferredWidth: mainLayout.width / 2
        implicitWidth:  numboxY.implicitWidth + labelY.width
        Layout.minimumWidth: numboxY.implicitWidth + labelY.width
        spacing:0

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
            Layout.preferredWidth: defaultSpacing.standardMargin
            Layout.minimumWidth: 0
            Layout.maximumWidth: defaultSpacing.standardMargin
            Layout.fillWidth: true
        }

        WGNumberBox {
            id: numboxY
            Layout.fillWidth: true
            Layout.preferredHeight: parent.height

            Layout.preferredWidth: mainLayout.width / 2
            implicitWidth:  contentWidth + defaultSpacing.doubleMargin
            Layout.minimumWidth: numboxY.implicitWidth

            number: itemData.Value.y
            maximumValue: 2147483647
            minimumValue: -2147483647

            onNumberChanged: {
                itemData.Value.y = number
            }
        }
    }
}
