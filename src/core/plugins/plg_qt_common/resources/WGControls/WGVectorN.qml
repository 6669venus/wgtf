import QtQuick 2.3
import QtQuick.Layouts 1.1
import BWControls 1.0

/*!
 \brief A generic vector control that can be used to represent any number of vectors.
 Use WGVector2, WGVector3, WGVector4, WGVectorRGB, and WGVectorRGBA instead of this control for
 preset values.
*/

WGExpandingRowLayout {
    id: mainLayout
    Layout.fillWidth: true
    spacing: 0

    implicitHeight: defaultSpacing.minimumRowHeight ? defaultSpacing.minimumRowHeight : 22

    /*! This property holds each individual vectors label */
    property variant vectorLabels: []
    /*! This property holds the vector values */
    property variant vectorData: []

    //properties for controlling spinner behaviour
    property real stepsize: 0.1
    property real maximumValue: 2147483647
    property real minimumValue: -2147483647
    property int decimals: 3
    property real defaultValue: minimumValue

    //binding changes in value back to data
    signal elementChanged (var value, int index)

    onElementChanged: {
        vectorData[index] = value
    }

    Repeater {
        id: spinBoxRepeater
        model: vectorData.length
        delegate:
            WGExpandingRowLayout {
                id: spinnerLayout
                Layout.preferredHeight: parent.height
                Layout.fillWidth: true
                spacing:0
                implicitHeight: parent.height

                property var value: vectorData[index]

                onValueChanged: {
                    mainLayout.elementChanged(value, index)
                }

                WGExpandingRowLayout {
                    id: layoutN
                    Layout.preferredHeight: parent.height

                    Layout.fillWidth: true
                    Layout.preferredWidth: mainLayout.width / vectorData.length
                    implicitWidth:  numboxN.implicitWidth + labelN.width
                    Layout.minimumWidth: numboxN.implicitWidth + labelN.width
                    spacing: 0

                    WGLabel{
                        id: labelN
                        Layout.preferredWidth: paintedWidth
                        Layout.minimumWidth: paintedWidth
                        Layout.preferredHeight: parent.height
                        Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                        text: vectorLabels[index]
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
                        id: numboxN
                        Layout.fillWidth: true
                        Layout.preferredHeight: parent.height

                        Layout.preferredWidth: mainLayout.width / vectorData.length
                        implicitWidth:  contentWidth + defaultSpacing.doubleMargin
                        Layout.minimumWidth: numboxN.implicitWidth

                        number: spinnerLayout.value
                        maximumValue: mainLayout.maximumValue
                        minimumValue: mainLayout.minimumValue
                        //set a default value? make specific in rgb versions
                        stepSize: mainLayout.stepsize
                        decimals: mainLayout.decimals
                        defaultValue: mainLayout.defaultValue
                    }
                }
                WGExpandingRowLayout {
                    //only draw in between layoutN objects, not at end
                    visible: index + 1 < spinBoxRepeater.count ? true : false
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
        } //end delegate
    }//end repeater
}
