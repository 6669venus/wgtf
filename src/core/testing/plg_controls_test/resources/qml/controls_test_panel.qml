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

    //width: 500
    //height: 1000

	//property var title: "Controls"
	property var title: qsTr("QML Controls Test Window")
    //title: qsTr("QML Controls Test Window")

    color: palette.MainWindowColor

    //property QtObject panelProps: WGPanelProperties{}

    //contentItem.minimumWidth: panelProps.minPanelWidth_

    //menuBar: MenuBar {
    //    WGMenu {
    //        title: qsTr("File")

    //        MenuItem {
    //            text: qsTr("Exit")
    //            onTriggered: Qt.quit();
    //        }
    //    }
    //    //Menu to show a few different UI styles
    //    WGMenu {
    //        title: qsTr("UI Style")
    //        ExclusiveGroup {
    //            id: styleGroup
    //        }

    //        MenuItem {
    //            text: qsTr("Dark")
    //            checkable: true
    //            checked: true
    //            exclusiveGroup: styleGroup
    //            onTriggered: {
    //                palette.MainWindowColor = "#383a3d"
    //                palette.HighlightColor = "#3399ff"
    //                palette.TextColor = "#FFFFFF"

    //                palette.LightShade = "#10FFFFFF"
    //                palette.LighterShade = "#20FFFFFF"
    //                palette.LightestShade = "#30FFFFFF"

    //                palette.DarkShade = "#20000000"
    //                palette.DarkerShade = "#40000000"
    //                palette.DarkestShade = "#60000000"

    //                palette.HighlightShade = Qt.rgba(palette.HighlightColor.r,palette.HighlightColor.g,palette.HighlightColor.b,0.5)

    //                palette.DarkHeaderColor = Qt.darker(palette.MainWindowColor, 1.25)
    //                palette.LightPanelColor = Qt.lighter(palette.MainWindowColor, 1.32)

    //                palette.MidDarkColor = Qt.darker(palette.MainWindowColor, 1.1)
    //                palette.MidLightColor = Qt.lighter(palette.MainWindowColor, 1.2)
    //                palette.DarkColor = Qt.darker(palette.MainWindowColor, 1.5)

    //                palette.NeutralTextColor = Qt.darker(palette.TextColor, 1.2)
    //                palette.DisabledTextColor = palette.LightestShade
    //                palette.TextBoxColor = palette.DarkerShade
    //                palette.PlaceholderTextColor = palette.LighterShade

    //                palette.GlowStyle = false
    //            }
    //        }
    //        MenuItem {
    //            text: qsTr("Light")
    //            checkable: true
    //            exclusiveGroup: styleGroup
    //            onTriggered: {
    //                palette.MainWindowColor = "#D8D8D8"
    //                palette.HighlightColor = "#438bbf"
    //                palette.TextColor = "#000000"

    //                palette.LightShade = "#20FFFFFF"
    //                palette.LighterShade = "#50FFFFFF"
    //                palette.LightestShade = "#80FFFFFF"

    //                palette.DarkShade = "#20000000"
    //                palette.DarkerShade = "#35000000"
    //                palette.DarkestShade = "#50000000"

    //                palette.HighlightShade = Qt.rgba(palette.HighlightColor.r,palette.HighlightColor.g,palette.HighlightColor.b,0.5)

    //                palette.DarkHeaderColor = "#5693bf"
    //                palette.LightPanelColor = "#E8E8E8"

    //                palette.MidDarkColor = Qt.darker(palette.MainWindowColor, 1.1)
    //                palette.MidLightColor = Qt.lighter(palette.MainWindowColor, 1.2)
    //                palette.DarkColor = Qt.darker(palette.MainWindowColor, 1.6)

    //                palette.NeutralTextColor = Qt.lighter(palette.TextColor, 1.2)
    //                palette.DisabledTextColor = palette.DarkestShade
    //                palette.TextBoxColor = palette.LightestShade
    //                palette.PlaceholderTextColor = palette.DarkerShade

    //                palette.GlowStyle = false
    //            }
    //        }
    //        MenuItem {
    //            text: qsTr("Wargaming")
    //            checkable: true
    //            exclusiveGroup: styleGroup
    //            onTriggered: {
    //                palette.MainWindowColor = "#333333"
    //                palette.HighlightColor = "#b0302c"
    //                palette.TextColor = "#FFFFFF"

    //                palette.LightShade = "#10FFFFFF"
    //                palette.LighterShade = "#20FFFFFF"
    //                palette.LightestShade = "#30FFFFFF"

    //                palette.DarkShade = "#20000000"
    //                palette.DarkerShade = "#40000000"
    //                palette.DarkestShade = "#60000000"

    //                palette.HighlightShade = Qt.rgba(palette.HighlightColor.r,palette.HighlightColor.g,palette.HighlightColor.b,0.5)

    //                palette.DarkHeaderColor = Qt.darker(palette.MainWindowColor, 1.25)
    //                palette.LightPanelColor = Qt.lighter(palette.MainWindowColor, 1.32)

    //                palette.MidDarkColor = Qt.darker(palette.MainWindowColor, 1.1)
    //                palette.MidLightColor = Qt.lighter(palette.MainWindowColor, 1.2)
    //                palette.DarkColor = Qt.darker(palette.MainWindowColor, 1.5)

    //                palette.NeutralTextColor = Qt.darker(palette.TextColor, 1.2)
    //                palette.DisabledTextColor = palette.LightestShade
    //                palette.TextBoxColor = palette.DarkerShade
    //                palette.PlaceholderTextColor = palette.LighterShade

    //                palette.GlowStyle = true
    //            }
    //        }
    //        MenuItem {
    //            text: qsTr("World of Tanks")
    //            checkable: true
    //            exclusiveGroup: styleGroup
    //            onTriggered: {
    //                palette.MainWindowColor = "#2e2c27"
    //                palette.HighlightColor = "#e5a139"
    //                palette.TextColor = "#FFFFFF"

    //                palette.LightShade = "#10FFFFFF"
    //                palette.LighterShade = "#20FFFFFF"
    //                palette.LightestShade = "#30FFFFFF"

    //                palette.DarkShade = "#20000000"
    //                palette.DarkerShade = "#40000000"
    //                palette.DarkestShade = "#60000000"

    //                palette.HighlightShade = Qt.rgba(palette.HighlightColor.r,palette.HighlightColor.g,palette.HighlightColor.b,0.5)

    //                palette.DarkHeaderColor = Qt.darker(palette.MainWindowColor, 1.25)
    //                palette.LightPanelColor = Qt.lighter(palette.MainWindowColor, 1.32)

    //                palette.MidDarkColor = Qt.darker(palette.MainWindowColor, 1.1)
    //                palette.MidLightColor = Qt.lighter(palette.MainWindowColor, 1.2)
    //                palette.DarkColor = Qt.darker(palette.MainWindowColor, 1.5)

    //                palette.NeutralTextColor = Qt.darker(palette.TextColor, 1.2)
    //                palette.DisabledTextColor = palette.LightestShade
    //                palette.TextBoxColor = palette.DarkerShade
    //                palette.PlaceholderTextColor = palette.LighterShade

    //                palette.GlowStyle = true
    //            }
    //        }
    //        MenuItem {
    //            text: qsTr("World of Warplanes")
    //            checkable: true
    //            exclusiveGroup: styleGroup
    //            onTriggered: {
    //                palette.MainWindowColor = "#27393d"
    //                palette.HighlightColor = "#39b2cf"
    //                palette.TextColor = "#FFFFFF"

    //                palette.LightShade = "#10FFFFFF"
    //                palette.LighterShade = "#20FFFFFF"
    //                palette.LightestShade = "#30FFFFFF"

    //                palette.DarkShade = "#20000000"
    //                palette.DarkerShade = "#40000000"
    //                palette.DarkestShade = "#60000000"

    //                palette.HighlightShade = Qt.rgba(palette.HighlightColor.r,palette.HighlightColor.g,palette.HighlightColor.b,0.5)

    //                palette.DarkHeaderColor = Qt.darker(palette.MainWindowColor, 1.25)
    //                palette.LightPanelColor = Qt.lighter(palette.MainWindowColor, 1.32)

    //                palette.MidDarkColor = Qt.darker(palette.MainWindowColor, 1.1)
    //                palette.MidLightColor = Qt.lighter(palette.MainWindowColor, 1.2)
    //                palette.DarkColor = Qt.darker(palette.MainWindowColor, 1.5)

    //                palette.NeutralTextColor = Qt.darker(palette.TextColor, 1.2)
    //                palette.DisabledTextColor = palette.LightestShade
    //                palette.TextBoxColor = palette.DarkerShade
    //                palette.PlaceholderTextColor = palette.LighterShade

    //                palette.GlowStyle = true
    //            }
    //        }
    //        MenuItem {
    //            text: qsTr("World of Warships")
    //            checkable: true
    //            exclusiveGroup: styleGroup
    //            onTriggered: {
    //                palette.MainWindowColor = "#1d3340"
    //                palette.HighlightColor = "#15a3d2"
    //                palette.TextColor = "#FFFFFF"

    //                palette.LightShade = "#10FFFFFF"
    //                palette.LighterShade = "#20FFFFFF"
    //                palette.LightestShade = "#30FFFFFF"

    //                palette.DarkShade = "#20000000"
    //                palette.DarkerShade = "#40000000"
    //                palette.DarkestShade = "#60000000"

    //                palette.HighlightShade = Qt.rgba(palette.HighlightColor.r,palette.HighlightColor.g,palette.HighlightColor.b,0.5)

    //                palette.DarkHeaderColor = Qt.darker(palette.MainWindowColor, 1.25)
    //                palette.LightPanelColor = Qt.lighter(palette.MainWindowColor, 1.32)

    //                palette.MidDarkColor = Qt.darker(palette.MainWindowColor, 1.1)
    //                palette.MidLightColor = Qt.lighter(palette.MainWindowColor, 1.2)
    //                palette.DarkColor = Qt.darker(palette.MainWindowColor, 1.5)

    //                palette.NeutralTextColor = Qt.darker(palette.TextColor, 1.2)
    //                palette.DisabledTextColor = palette.LightestShade
    //                palette.TextBoxColor = palette.DarkerShade
    //                palette.PlaceholderTextColor = palette.LighterShade

    //                palette.GlowStyle = true
    //            }
    //        }
    //    }
    //}

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
                            text: "Open"
                        }

                        WGPushButton {
                            text: "Import"
                        }
                    }
                childObject_ :
                    WGFormLayout {
                        id: topForm
                        localForm_: true

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
                                    height: panelProps.rowHeight_

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

            WGButtonBar {
                buttonList: [
                    WGPushButton {
                        text: "One"
                    },
                    WGPushButton {
                        text: "Two"
                    },
                    WGPushButton {
                        text: "Check"
                        checkState: true
                        checkable: true
                    },
                    WGPushButton {
                        text: "icon"
                        iconSource: "icons/save_16x16.png"
                    },
                    WGPushButton {
                        iconSource: "icons/save_16x16.png"
                    }
                ]
                height: 24
                Layout.preferredWidth: -1//260
            }

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

            WGTabView{
                id: tabviewexample
                anchors.left: parent.left
                anchors.right: parent.right
                tabPosition: Qt.TopEdge // Qt.BottomEdge
                Layout.preferredHeight: 180

                Tab {
                    title: "Test one"

                    WGScrollPanel{
                        childObject_:
                            //WGColumnScrollPanel
                            WGColumnLayout {
                                WGInternalPanel{
                                    text: "Panel within a tab"
                                    clipContents_: true
                                    expanded_: true
                                    childObject_ :
                                        WGColumnLayout{
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
                                                    //snapping_: true
                                                    //snapValue_: 75
                                                    label_: "Snapping Slider:"
                                                }

                                                WGSliderControl {
                                                    Layout.fillWidth: true
                                                    minimumValue: 0
                                                    maximumValue: 1440
                                                    //decimals_: 2
                                                    //timeObject_: true
                                                    stepSize: 1
                                                    value: 720
                                                    enabled: true
                                                    label_: "Time of Day:"
                                                }

                                                WGRangeSlider {
                                                    Layout.fillWidth: true
                                                    minimumValue: 0
                                                    maximumValue: 100
                                                    //lowerValue_: 25
                                                    //value: 75
                                                    stepSize: 1
                                                    enabled: true
                                                    label_: "Range Slider:"
                                                }
                                            }
                                        }
                                }
                                WGInternalPanel{
                                    text: "Another panel within a tab"
                                    clipContents_: true
                                    expanded_: true
                                    childObject_ :
                                        WGColumnLayout{
                    WGTextBox {
                        width: 150
                        placeholderText: "Text Field"
                                                text: "Im in a tab"
                                            }
                                            WGCheckBox {
                                                width: 200
                                                text: "Tri-State Checkbox 1"
                                                partiallyCheckedEnabled: true
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
                                                    //snapping_: true
                                                    //snapValue_: 75
                                                    label_: "Snapping Slider:"
                                                }

                                                WGSliderControl {
                                                    Layout.fillWidth: true
                                                    minimumValue: 0
                                                    maximumValue: 1440
                                                    //decimals_: 2
                                                    //timeObject_: true
                                                    stepSize: 1
                                                    value: 720
                                                    enabled: true
                                                    label_: "Time of Day:"
                                                }

                                                WGRangeSlider {
                                                Layout.fillWidth: true
                                                minimumValue: 0
                                                maximumValue: 100
                                                //lowerValue_: 25
                                                //value: 75
                                                stepSize: 1
                                                enabled: true
                                                label_: "Range Slider:"
                                            }
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

                            /* // Currently doesnt work due to NGT-886
                            WGNumberBox {
                                Layout.preferredWidth: 120
                                value: 25
                                minimumValue: 0
                                maximumValue: 100

                                b_Target: perfBar2
                                b_Property: "value_"
                                b_Value: value
                            }

                            WGPerformanceBar {
                                id: perfBar2
                                Layout.preferredWidth: 200
                            }
                            */

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
                    //snapping_: true
                    //snapValue_: 75
                    label_: "Snapping Slider:"
                }

                WGSliderControl {
                    Layout.fillWidth: true
                    minimumValue: 0
                    maximumValue: 1440
                    //decimals_: 2
                    //timeObject_: true
                    stepSize: 1
                    value: 720
                    enabled: true
                    label_: "Time of Day:"
                }

                WGRangeSlider {
                    Layout.fillWidth: true
                    minimumValue: 0
                    maximumValue: 100
                    //lowerValue_: 25
                    //value: 75
                    stepSize: 1
                    enabled: true
                    label_: "Range Slider:"
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
                    //width: 150

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
                        //exclusive_: true
                        buttonList: [
                                WGPushButton {
                                    text: "X"
                                    checked: true
                                },
                                WGPushButton {
                                    text: "Y"
                                },
                                WGPushButton {
                                    text: "Z"
                                }
                        ]

                    }

                    WGButtonBar {
                        width: 120
                        buttonList: [
                                WGPushButton {
                                    iconSource: "icons/pause_16x16.png"
                                },
                                WGPushButton {
                                    iconSource: "icons/play_16x16.png"
                                },
                                WGPushButton {
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

            WGSubPanel {
                text: "Color Picker"
                expanded_: false
                childObject_ :
                    WGColumnLayout {

                        WGMultiLineText {
                            Layout.fillWidth: true
                            text: "This panel is a prototype color picker that is also connected to the main palette colors. This lets you experiment with different styles."
                        }
                        WGColorPicker {
                            Layout.fillWidth: true
                            combinedColor: "#000000"
                        }

                        WGLabel {
                            Layout.fillWidth: true
                            color: "#DD4411"
                            text: "For testing purposes only"
                            font.bold: true
                            horizontalAlignment: Text.AlignRight
                        }
                }
            }
        }
    }
}

