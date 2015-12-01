import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls 1.0

/**
 *	Sample panel with all the available controls and styles.
 *	Used for control testing
 */
Rectangle {
	id: mainWindow

    visible: true
    property var title: qsTr("QML Controls Test Window")

    color: palette.MainWindowColor

    WGBusyIndicator {
        id: busyIndicator1
        z:100
        anchors.centerIn: parent
        running: false
    }

    WGScrollPanel {

        childObject_ :
        WGDraggableColumn {

            WGSubPanel {
                text: "Form Layout"
                headerObject_ :
                    WGExpandingRowLayout {
                        Rectangle {
                            color: "transparent"
                            Layout.fillWidth: true
                        }

                        WGPushButton {
                            text: checked ? "Turn Panel Dragging Off" : "Turn Panel Dragging On"
                            checkable: true
                            onClicked: {
                                globalSettings.dragLocked = !globalSettings.dragLocked
                                console.log(globalSettings.dragLocked)
                            }
                        }
                    }
                childObject_ :
                    WGFormLayout {
                        id: topForm
                        localForm_: true

                        WGColumnLayout {
                            Layout.fillWidth: true
                            WGMultiLineText {
                                Layout.fillWidth: true
                                text: "This panel demonstrates the current QML controls that have been created.
Note: A lot of the functionality such as panels, draggable columns, form layouts etc. are QML implementations only and while ok for small panels, are probably not ideal as an application wide solution.

It is recommended to look at the generic_app_test to view other controls such as the Asset Browser, Active Filters, reflected properties etc."
                            }
                        }

                        WGLabel {
                            text: "Import Files"
                        }

                        WGExpandingRowLayout {
                            Layout.fillWidth: true
                            label_: "Filename:"

                            Column {
                                Layout.fillWidth: true
                                Layout.minimumHeight: openImage.height

                                WGTextBox {
                                  id: filenameBox
                                  anchors{left: parent.left; right:parent.right;}
                                }

                                WGExpandingRowLayout {
                                    anchors{left: parent.left; right:parent.right;}
                                    height: defaultSpacing.minimumRowHeight

                                    WGPushButton {
                                        text: "Open"
                                        Layout.fillWidth: true
                                        onClicked:{
                                            openImage.clicked();
                                        }
                                    }

                                    WGPushButton {
                                        text: "Import"
                                        Layout.fillWidth: true
                                        onClicked:{
                                            openImage.clicked();
                                        }
                                    }
                                }

                                    WGVerticalSpacer{

                                        anchors{left: parent.left; right:parent.right;}
                                    }
                                }

                            WGThumbnailButton {
                                id: openImage
                                anchors.top: parent.top
                                defaultText: "Click to Load an Image"
                                Layout.preferredWidth: implicitWidth
                            }

                        }

                        WGFrame {
                            dark_: false
                            shade_: true

                            label_: "File Options:"

                            childObject_:
                            WGColumnLayout {

                                WGCheckBox {
                                    text: "File Option 1"
                                }
                                WGCheckBox {
                                    text: "File Option 2"
                                }
                                WGCheckBox {
                                    enabled: false
                                    text: "Disabled File Option"
                                }
                            }
                        }

                        WGProgressControl {
                            Layout.fillWidth: true
                            indeterminate_: true
                            text: "File Importing..."
                        }

                    }


                }


            WGExpandingRowLayout {
                Layout.fillWidth: true

                WGTextBox {
                    placeholderText: "Placeholder Text"
                    Layout.fillWidth: true
                }

                WGTextBox {
                    text: "Read Only Text Box"
                    readOnly: true
                    Layout.fillWidth: true
                }

                WGTextBox {
                    text: "Disabled Text Box"
                    enabled: false
                    Layout.fillWidth: true
                }
            }


            WGGroupBox {
                text: "Group Box"
                toggleable_ : true
                childObject_ :
                WGBoolGridLayout {
                    ExclusiveGroup { id: radioPanelGroup }
                    WGRadioButton {
                        text: "Radio Button"
                        exclusiveGroup: radioPanelGroup
                        checked: true
                    }
                    WGRadioButton {
                        text: "Radio Button"
                        exclusiveGroup: radioPanelGroup
                    }
                    WGRadioButton {
                        text: "Radio Button with a long name"
                        exclusiveGroup: radioPanelGroup
                    }
                }
            }
            WGSubPanel {
                text: "Color Sliders"
                childObject_ :
                    WGColumnLayout {

                    WGColorSlider {
                        Layout.fillWidth: true
                        minimumValue: 0
                        maximumValue: 255
                        stepSize: 1
                        colorData: [Qt.rgba(0,0,0,1), Qt.rgba(1,1,1,1)]
                        positionData: [0, 255]
                        value: 128
                        linkColorsToHandles: false
                    }

                    WGColorSlider {
                        Layout.fillWidth: true
                        minimumValue: 0
                        maximumValue: 255
                        stepSize: 1
                        colorData: [Qt.rgba(1,0,0,0), Qt.rgba(1,0,0,1)]
                        positionData: [0, 255]
                        value: 255
                        linkColorsToHandles: false
                    }

                    WGColorSlider {
                        Layout.fillWidth: true
                        minimumValue: 0
                        maximumValue: 255
                        stepSize: 1
                        colorData: [Qt.rgba(1,0,0,1), Qt.rgba(1,1,0,1), Qt.rgba(0,1,0,1), Qt.rgba(0,0,1,1), Qt.rgba(1,0,1,1)]
                        positionData: [0, 64, 128, 192, 255]
                        value: 128
                        linkColorsToHandles: false
                    }

                    WGSeparator {
                        vertical_: false

                    }

                    WGLabel {
                        Layout.fillWidth: true
                        text: "Gradient/Ramp Slider."
                        font.bold: true
                    }

                    WGMultiLineText {
                        Layout.fillWidth: true
                        text: "Shift+Click to add handles.\nCtrl+Click to delete handles.\nDouble click to change color"
                    }

                    WGColorSlider {
                        Layout.fillWidth: true
                        minimumValue: 0
                        maximumValue: 100
                        stepSize: 0.1
                        colorData: [Qt.rgba(0.5,0,0,1), Qt.rgba(1,0,0,1), Qt.rgba(1,1,0,1), Qt.rgba(1,1,1,1)]
                        positionData: [20, 40, 60, 80]
                        linkColorsToHandles: true
                    }
                }
            }


            WGSubPanel {
                text: "Sub Panel"
                childObject_ :
                    WGColumnLayout {

                        WGExpandingRowLayout {

                            WGPushButton {
                                text: "Auto-Size Button"
                                iconSource: "icons/save_16x16.png"
                            }

                            WGPushButton {
                                iconSource: "icons/close_16x16.png"
                            }

                        }

                        WGPushButton {
                            text: "Auto-Size Button"
                        }

                        WGExpandingRowLayout {
                            ExclusiveGroup { id: toolbarGroup }
                            WGToolButton {
                                checkable: true
                                checked: true
                                exclusiveGroup: toolbarGroup
                                iconSource: "icons/pause_16x16.png"
                            }
                            WGToolButton {
                                checkable: true
                                exclusiveGroup: toolbarGroup
                                iconSource: "icons/play_16x16.png"
                            }
                            WGSeparator {
                                vertical_: true
                            }

                            WGToolButton {
                                checkable: true
                                iconSource: "icons/loop_16x16.png"
                            }
                        }

                        WGInternalPanel {
                            text: "Scrolling Panel of Buttons"
                            clipContents_: true
                            expanded_: false
                            childObject_ :
                                WGSubScrollPanel {
                                    implicitHeight: 100
                                    childObject_:
                                        WGFormLayout {
                                        localForm_: true
                                            WGLabel{
                                                text: "Internal scrollbars possible, but not recommended"
                                            }

                                            Repeater {
                                                model: 10
                                                WGPushButton {
                                                    text: "Button"
                                                    label_: "Repeated Label: "
                                                }
                                            }
                                        }
                            }
                        }

                        WGFrame {
                            childObject_ :
                            WGColumnLayout {
                                WGProgressControl {
                                    Layout.fillWidth: true
                                    maximumValue: 100
                                    fakeProgress_: true
                                    text: "Progress Bar Complete..."
                                    units_: "%"
                                }
                            }
                        }
                    }
            }

            WGTabView {
                id: tabviewexample
                anchors.left: parent.left
                anchors.right: parent.right
                tabPosition: Qt.TopEdge // Qt.BottomEdge
                Layout.preferredHeight: 180

                Tab {
                    title: "Test one"
                    WGScrollPanel{
                    childObject_:
                        WGColumnLayout {
                            WGInternalPanel{
                                text: "Panel within a tab"
                                clipContents_: true
                                expanded_: true
                                childObject_ :
                                WGColumnLayout{
                                    WGMultiLineText {
                                        Layout.fillWidth: true
                                        text: "These tabs are QML only and cannot be detached, undocked etc. Use Dock Widgets for this."
                                    }

                                    WGTextBox {
                                        width: 150
                                        placeholderText: "Text Field"
                                        text: "Im in a tab"
                                    }

                                    WGExpandingRowLayout {
                                        WGNumberBox {
                                            width: 120
                                            value: 25
                                            minimumValue: 0
                                            maximumValue: 100

                                            b_Target: perfBar3
                                            b_Property: "value_"
                                            b_Value: value
                                        }

                                        WGPerformanceBar {
                                            id: perfBar3
                                        }
                                    }

                                    WGCheckBox {
                                        width: 200
                                        text: "Tri-State Checkbox 1"
                                        partiallyCheckedEnabled: true
                                    }
                                }
                            }
                        }
                    }
                }
                Tab {
                    title: "Test two"
                    anchors.left: parent.left
                    anchors.right: parent.right

                    WGSubScrollPanel{
                        anchors.rightMargin: defaultSpacing.rightMargin

                        childObject_ :
                        WGColumnLayout {
                            anchors.left: parent.left
                            anchors.right: parent.right

                            WGExpandingRowLayout {
                                Layout.fillWidth: true
                                WGTextBox {
                                    placeholderText: "Text Field"
                                    text: "Text Value"
                                    Layout.fillWidth: true
                                }
                                WGTextBox {
                                    text: "Read Only Text Box"
                                    readOnly: true
                                    Layout.fillWidth: true
                                }
                                WGTextBox {
                                    width: 150
                                    text: "Disabled Text Box"
                                    Layout.fillWidth: true
                                }
                            }
                            WGCheckBox {
                                width: 200
                                text: "Tri-State Checkbox 1"
                                partiallyCheckedEnabled: true
                            }
                            WGBoolGridLayout {
                                ExclusiveGroup { id: radioGroup2 }
                                WGRadioButton {
                                    text: "Radio 1"
                                    exclusiveGroup: radioGroup2
                                    checked: true
                                }
                                WGRadioButton {
                                    text: "Radio 2"
                                    exclusiveGroup: radioGroup2
                                }
                                WGRadioButton {
                                    text: "Radio 3"
                                    exclusiveGroup: radioGroup2
                                    enabled: false
                                }
                            }
                            WGNumberBox {
                                Layout.preferredWidth: 120
                                value: 25
                                minimumValue: 0
                                maximumValue: 100
                            }
                        }
                    }
                }

                Tab {
                    title: "test very large title three"
                    anchors.left: parent.left
                    anchors.right: parent.right

                    WGSubScrollPanel{
                        anchors.rightMargin: defaultSpacing.rightMargin

                        childObject_ :
                        WGColumnLayout {
                            anchors.left: parent.left
                            anchors.right: parent.right

                            WGNumberBox {
                            width: 120
                            value: 25
                            minimumValue: 0
                            maximumValue: 100

                            b_Target: perfBar2
                            b_Property: "value_"
                            b_Value: value
                            }

                            WGPerformanceBar {
                                id: perfBar2
                            }

                            WGCheckBox {
                                width: 200
                                text: "Tri-State Checkbox 1"
                                partiallyCheckedEnabled: true
                            }
                            WGCheckBox {
                                width: 200
                                text: "Tri-State Checkbox 2"
                                partiallyCheckedEnabled: true
                            }
                            WGCheckBox {
                                width: 200
                                text: "Tri-State Checkbox 3"
                                partiallyCheckedEnabled: true
                            }
                            WGCheckBox {
                                width: 200
                                text: "Tri-State Checkbox 4"
                                partiallyCheckedEnabled: true
                            }
                        }
                    }
                }
            }

            Rectangle{
                color: "transparent"
                height: defaultSpacing.rowSpacing
            }

            WGColumnLayout {

                WGExpandingRowLayout {
                    WGTextBox {
                        width: 150
                        placeholderText: "Text Field"
                        text: "Text Value"
                    }
                    WGTextBox {
                        width: 150
                        text: "Read Only Text Box"
                        readOnly: true
                        Layout.fillWidth: true
                    }
                    WGTextBox {
                        width: 150
                        text: "Disabled Text Box"
                        enabled: false
                    }
                }

                WGNumberBox {
                    width: 120
                    value: 25
                    minimumValue: 0
                    maximumValue: 100

                    b_Target: perfBar
                    b_Property: "value_"
                    b_Value: value
                }

                WGPerformanceBar {
                    id: perfBar
                }

                WGCheckBox {
                    width: 200
                    text: "Tri-State Checkbox"
                    partiallyCheckedEnabled: true
                }

                WGBoolGridLayout {
                    ExclusiveGroup { id: radioGroup }
                    WGRadioButton {
                        text: "Radio 1"
                        exclusiveGroup: radioGroup
                        checked: true
                    }
                    WGRadioButton {
                        text: "Radio 2"
                        exclusiveGroup: radioGroup
                    }
                    WGRadioButton {
                        text: "Radio 3"
                        exclusiveGroup: radioGroup
                        enabled: false
                    }
                }
            }

            WGFormLayout {

                WGSplitTextFrame {
                    boxList: [
                        WGNumberBox{
                            value: 192
                            minimumValue: 0
                            maximumValue: 255
                        },
                        WGNumberBox{
                            value: 168
                            minimumValue: 0
                            maximumValue: 255
                        },
                        WGNumberBox{
                            value: 0
                            minimumValue: 0
                            maximumValue: 255
                        },
                        WGNumberBox{
                            value: 1
                            minimumValue: 0
                            maximumValue: 255
                        }
                    ]
                    decimalSeparator: true
                    height: 24
                    Layout.preferredWidth: 130
                    label_: "IP Address:"
                }
                WGSplitTextFrame {
                    boxList: [
                        WGNumberBox{
                            value: 60
                            minimumValue: 0
                            maximumValue: 255
                        },
                        WGNumberBox{
                            value: 128
                            minimumValue: 0
                            maximumValue: 255
                        },
                        WGNumberBox{
                            value: 55
                            minimumValue: 0
                            maximumValue: 255
                        }
                    ]
                    height: 24
                    Layout.preferredWidth: 130
                    label_: "RGB:"
                }

                WGSplitTextFrame {
                    boxList: [
                        WGNumberBox{
                            value: 7
                            minimumValue: 0
                            maximumValue: 100
                            width: 36
                        },
                        WGTextBox{
                            text: "Greendale"
                            width: 80
                        },
                        WGTextBox{
                            text: "Drive"
                            width: 80
                        }
                    ]
                    evenBoxes: false
                    height: 24
                    label_: "Address:"
                }

                WGSliderControl {
                    Layout.fillWidth: true
                    minimumValue: 0
                    maximumValue: 100
                    stepSize: 1
                    value: 40
                    label_: "Single Slider:"
                }

                WGRangeSlider {
                    Layout.fillWidth: true
                    minimumValue: 0
                    maximumValue: 100
                    lowerValue: 25
                    upperValue: 25
                    stepSize: 1
                    enabled: true
                    label_: "Range Slider:"
                }

                WGSlider {
                    Layout.fillWidth: true
                    Layout.preferredHeight: defaultSpacing.minimumRowHeight
                    minimumValue: 0
                    maximumValue: 100
                    stepSize: 1
                    enabled: true
                    property string label_: "Multi Handle Slider:"

                    WGSliderHandle {
                        minimumValue: 0
                        maximumValue: 100
                        value: 0
                        showBar: false
                    }
                    WGSliderHandle {
                        minimumValue: 0
                        maximumValue: 100
                        value: 25
                        showBar: false
                    }
                    WGSliderHandle {
                        minimumValue: 0
                        maximumValue: 100
                        value: 50
                        showBar: false
                    }
                    WGSliderHandle {
                        minimumValue: 0
                        maximumValue: 100
                        value: 75
                        showBar: false
                    }
                    WGSliderHandle {
                        minimumValue: 0
                        maximumValue: 100
                        value: 100
                        showBar: false
                    }
                }
            }


            WGSubPanel {
                text: "LOD Control"
                childObject_ :
                    WGColumnLayout {
                        WGLodSlider {
                            Layout.fillWidth: true
                        }
                }
            }


            WGColumnLayout {

                WGExpandingRowLayout {
                    WGPushButton {
                        id: busyIndicatorButton
                        width: 150
                        checkable: true
                        text: "Busy Indicator"
                        iconSource: "icons/close_16x16.png"

                        onCheckedChanged: {
                            if (busyIndicatorButton.checked == true){
                                busyIndicator1.running = true
                            }
                            else {
                                busyIndicator1.running = false
                            }
                        }
                    }

                    WGPushButton {
                        width: 150
                        checkable: true
                        enabled: false
                        text: "Disabled Button"
                    }
                }

                WGExpandingRowLayout {

                    WGPushButton {
                        width: 150
                        text: "Icon Button"
                        iconSource: "icons/save_16x16.png"
                    }

                    WGPushButton {
                        width: 150
                        checkable: true
                        text: "Icon Button"
                        iconSource: "icons/close_16x16.png"
                        enabled: false
                    }
                }

                WGExpandingRowLayout {
                    WGToolButton {
                        iconSource: "icons/save_16x16.png"
                    }
                    WGToolButton {
                        iconSource: "icons/paste_16x16.png"
                        enabled: false
                    }
                    WGToolButton {
                        iconSource: "icons/close_16x16.png"
                    }
                }

                WGDropDownBox {
                    id: dropDown

                    model: ListModel {
                        ListElement { text: "Option 1" }
                        ListElement { text: "Option 2" }
                        ListElement { text: "Option 3 Has a Really long Name" }
                        ListElement { text: "Option 4" }
                        ListElement { text: "Option 5" }
                    }
                }

                WGButtonBar {
                    Layout.fillWidth: true
                    buttonList: [
                            WGPushButton {
                                text: "Button 1"
                            },
                            WGPushButton {
                                text: "Button 1"
                            },
                            WGPushButton {
                                text: "Button 1"
                            },
                            WGPushButton {
                                text: "Button 4"
                            }
                    ]
                }

                WGExpandingRowLayout {

                    WGButtonBar {
                        width: 120
                        ExclusiveGroup {
                            id: xyzGroup
                        }

                        buttonList: [
                            WGToolButton {
                                text: "X"
                                checked: true
                                checkable: true
                                exclusiveGroup: xyzGroup
                            },
                            WGToolButton {
                                text: "Y"
                                checkable: true
                                exclusiveGroup: xyzGroup
                            },
                            WGToolButton {
                                text: "Z"
                                checkable: true
                                exclusiveGroup: xyzGroup
                            }
                        ]

                    }

                    WGButtonBar {
                        width: 120
                        buttonList: [
                            WGToolButton {
                                iconSource: "icons/pause_16x16.png"
                            },
                            WGToolButton {
                                iconSource: "icons/play_16x16.png"
                            },
                            WGToolButton {
                                iconSource: "icons/loop_16x16.png"
                            }
                        ]
                    }

                }

                WGSeparator {
                    Layout.fillWidth: true
                    vertical_: false
                }
            }
        }
    }
}

