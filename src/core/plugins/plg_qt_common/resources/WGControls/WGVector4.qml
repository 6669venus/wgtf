import QtQuick 2.3
import QtQuick.Layouts 1.1
import BWControls 1.0

//TODO:
// Would be nice if other Vectors inherited vector4_component and turned off the last value
// but doing so caused a major crash.
// Need to add in proper min/max values and required decimal points.

WGExpandingRowLayout {
    id: mainLayout
    Layout.fillWidth: true
    spacing: 0

    implicitHeight: defaultSpacing.minimumRowHeight ? defaultSpacing.minimumRowHeight : 22

    property string label_1: "X:"
    property real vector_1

    property string label_2: "Y:"
    property real vector_2

    property string label_3: "Z:"
    property real vector_3

    property string label_4: "W:"
    property real vector_4

    WGExpandingRowLayout {
        id: layoutX
        Layout.preferredHeight: parent.height

        Layout.fillWidth: true
        Layout.preferredWidth: mainLayout.width / 4
        implicitWidth:  numboxX.implicitWidth + labelX.width
        Layout.minimumWidth: numboxX.implicitWidth + labelX.width
        spacing: 0

        WGLabel{
            id: labelX
            Layout.preferredWidth: paintedWidth
            Layout.minimumWidth: paintedWidth
            Layout.preferredHeight: parent.height
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            text: label_1
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

            Layout.preferredWidth: mainLayout.width / 4
            implicitWidth:  contentWidth + defaultSpacing.doubleMargin
            Layout.minimumWidth: numboxX.implicitWidth

            number: vector_1
            maximumValue: 2147483647
            minimumValue: -2147483647
            stepSize: 0.1
            decimals: 3
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
        Layout.preferredWidth: mainLayout.width / 4
        implicitWidth:  numboxY.implicitWidth + labelY.width
        Layout.minimumWidth: numboxY.implicitWidth + labelY.width
        spacing:0

        WGLabel{
            id: labelY
            Layout.preferredWidth: paintedWidth
            Layout.minimumWidth: paintedWidth
            Layout.preferredHeight: parent.height
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            text: label_2
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

            Layout.preferredWidth: mainLayout.width / 4
            implicitWidth:  contentWidth + defaultSpacing.doubleMargin
            Layout.minimumWidth: numboxY.implicitWidth

            number: vector_2
            maximumValue: 2147483647
            minimumValue: -2147483647
            stepSize: 0.1
            decimals: 3
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
        id: layoutZ
        Layout.preferredHeight: parent.height

        Layout.fillWidth: true
        Layout.preferredWidth: mainLayout.width / 4
        implicitWidth:  numboxZ.implicitWidth + labelZ.width
        Layout.minimumWidth: numboxZ.implicitWidth + labelZ.width
        spacing:0

        WGLabel{
            id: labelZ
            Layout.preferredWidth: paintedWidth
            Layout.minimumWidth: paintedWidth
            Layout.preferredHeight: parent.height
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            text: label_3
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
            id: numboxZ
            Layout.fillWidth: true
            Layout.preferredHeight: parent.height

            Layout.preferredWidth: mainLayout.width / 4
            implicitWidth:  contentWidth + defaultSpacing.doubleMargin
            Layout.minimumWidth: numboxZ.implicitWidth

            number: vector_3
            maximumValue: 2147483647
            minimumValue: -2147483647
            stepSize: 0.1
            decimals: 3

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
        id: layoutW
        Layout.preferredHeight: parent.height

        Layout.fillWidth: true
        Layout.preferredWidth: mainLayout.width / 4
        implicitWidth:  numboxW.implicitWidth + labelW.width
        Layout.minimumWidth: numboxW.implicitWidth + labelW.width
        spacing:0

        WGLabel{
            id: labelW
            Layout.preferredWidth: paintedWidth
            Layout.minimumWidth: paintedWidth
            Layout.preferredHeight: parent.height
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            text: label_4
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
            id: numboxW
            Layout.fillWidth: true
            Layout.preferredHeight: parent.height

            Layout.preferredWidth: mainLayout.width / 4
            implicitWidth:  contentWidth + defaultSpacing.doubleMargin
            Layout.minimumWidth: numboxW.implicitWidth

            number: vector_4
            maximumValue: 2147483647
            minimumValue: -2147483647
            stepSize: 0.1
            decimals: 3

        }
    }
}
