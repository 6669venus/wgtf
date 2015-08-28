import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.1

/*!
 \brief Progress control that can have a % complete, an indeterminate style or even a fake completion animation.

Example:
\code{.js}
WGProgressControl {
    Layout.fillWidth: true
    text: "File Importing..."
    units_: "%"
}
\endcode
*/

Rectangle {
    id: progControl
    objectName: "WGProgressControl"

    //TODO: This should be renamed and marked as internal by "__" prefix
    /*! \internal */
    property int percentage_: 0

    /*!
        This property toggles the display of the percentage complete indicator
        The default value is \c false
    */
    property bool indeterminate_: false

    /*!
        This property defines the suffix string used to indicate the units used
        The default value is an empty string
    */
    property string units_: ""

    /*!
        This property will cause the progress bar to progress for demonstration purposes and testing.
        The default value is \c false
    */
    property bool fakeProgress_: false

    /*!
        This property determines how long progress takes when fakeProgress_ is true.
        The default value is \c false
    */
    property int fakeDuration_: 1000

    //TODO: This should be renamed and marked as internal by "__" prefix
    /*! \internal */
    property real endValue_: 1

    /*! This property holds the string to be displayed above the progress bar.
        The default value is an empty string
    */
    property alias text: descriptionText.text

    //TODO: This should be renamed and marked as internal by "__" prefix
    /*! \internal */
    property alias value: progBar.value

    /*  TODO: Should this be marked internal?
        It appears to be a magic number that has no affect on the control
        If so, this should be renamed and marked as internal by "__" prefix
    /*! \internal */
    property alias maximumValue: progBar.maximumValue

    signal progressEnded (bool completed)

    PropertyAnimation {
        target: progBar; property: "value"
        duration: fakeDuration_; from: 0; to: maximumValue
        easing.type: Easing.InOutQuad ; running: fakeProgress_
    }
    color: "transparent"

    implicitHeight: 36

    ColumnLayout {
        anchors.fill: parent
        spacing: defaultSpacing.standardMargin

        Rectangle {
            id: labelBox
            color: "transparent"

            Layout.fillHeight: true
            Layout.fillWidth: true


            Text {
                id: descriptionText
                anchors.left: parent.left
                anchors.top: parent.top

                renderType: Text.NativeRendering

                color: palette.TextColor
            }

            Text {
                id: valueText

                anchors.left: descriptionText.right
                anchors.top: parent.top
                anchors.leftMargin: defaultSpacing.standardMargin

                renderType: Text.NativeRendering

                text: percentage_

                visible: !indeterminate_
                color: palette.TextColor
                horizontalAlignment: Text.AlignRight
            }

            Text {
                id: unitText
                anchors.left: valueText.right
                anchors.top: parent.top

                renderType: Text.NativeRendering

                text: units_

                visible: !indeterminate_ && units_ != ""
                color: palette.TextColor
                horizontalAlignment: Text.AlignRight
            }

            WGPushButton {
                text: "Cancel"
                anchors.right: parent.right
                height: defaultSpacing.minimumRowHeight
                onClicked: {
                    progControl.progressEnded(false)
                }
            }
        }

        ProgressBar {
            id: progBar

            value: progControl.indeterminate_ ? maximumValue : 0

            Layout.fillWidth: true
            Layout.maximumHeight: 8
            Layout.minimumHeight: 8

            onValueChanged: {
                percentage_ = progBar.value/maximumValue * 100
                if (!indeterminate_ && progBar.value == endValue_)
                {
                    progControl.progressEnded(true)
                }
            }

            style: ProgressBarStyle {

                background: WGTextBoxFrame {}

                progress: Rectangle {
                    color: palette.HighlightColor
                    radius: defaultSpacing.halfRadius
                    Item {
                            anchors.fill: parent
                            anchors.margins: 1
                            visible: progControl.indeterminate_
                            clip: true
                            Row {
                                Repeater {
                                    Rectangle {
                                        color: index % 2 ? "transparent" : palette.DarkShade
                                        width: 10 ; height: control.height
                                    }
                                    model: control.width / 10 + 2
                                }
                                XAnimator on x {
                                    from: -20 ; to: 0
                                    loops: Animation.Infinite
                                    running: progControl.indeterminate_
                                }
                            }
                    }
                }
            }
        }
    }
}
