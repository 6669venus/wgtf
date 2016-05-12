import QtQuick 2.5
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3

import BWControls 1.0
import QtQml.Models 2.2
import WGControls 1.0

/*  TODO
    1. The drop down will not always draw above other neighboring components
    Z  height is unfortunately only relative to siblings. This is fine for custom controls where height can be
    manually set for all components but it will cause problems for property panels that are generated dynamically.
    2. Dropdown should disapear if anywhere else is clicked. This can be done with a proper floating window implementation
    It cannot be done with focusScope in its current form
    3. The scrollbar in WGScrollPanel steals hover and thus trigers the timer.
    This can probably be fixed by using a Flickable with a custom scrollbar.
    4. It would be nice to add press on collapsed button button and hold, drag to target then release.
    5. Implement showPath
    6. Warning, data when no data is given
*/

/*!
\brief A Custom Combobox that contains an Image as a drop down option

\code{.js}
WGComboBoxImage {
}
\endcode
*/

Item {
    id: comboboximage
    z: 10
    implicitWidth: textMetricsCreator.maxWidth + defaultSpacing.standardMargin * 4 + height + 16

    height: defaultSpacing.minimumRowHeight * 2

    /*! This property limits the number of list items shown in the drop down*/
    property int showXItems: 5

    // Ugly hack for creation purposes. Do not use this.
    /*Component.onCompleted: {
        parent.z = 9
        parent.parent.z = 8
        parent.parent.parent.z = 7
        parent.parent.parent.parent.z = 6
        parent.parent.parent.parent.parent.z = 5
        parent.parent.parent.parent.parent.parent.z = 4
        parent.parent.parent.parent.parent.parent.parent.z = 3
        parent.parent.parent.parent.parent.parent.parent.parent.z = 2
        parent.parent.parent.parent.parent.parent.parent.parent.parent.z = 1
     }*/

    //find the widest text in model to help set control width
    Repeater {
        id: textMetricsCreator
        model: comboboximage.model
        property real maxWidth: 0

        Item {
            id:itemWrapper

            TextMetrics {
                id: fakeText
                text: model.path
                onTextChanged: {
                    textMetricsCreator.maxWidth = Math.max(textMetricsCreator.maxWidth, width)
                }
            }
        }
    }

    property bool showpath: true
    property int dropDownCurrentIndex: 0
    property bool dropdownChecked: false

    property ListModel model: ListModel {
        id: comboboximageModel
        ListElement {path: "example/path1"; img: "icons/modelThumbnail.png"}
        ListElement {path: "example/path2"; img: "icons/diffuse3.png"}
        ListElement {path: "example/path3"; img: "icons/modelThumbnail.png"}
        ListElement {path: "example/a/long/really really long/path4"; img: "icons/proxyThumbnail.png"}
    }

    Timer {
        id: fadeTimer
        running: false
        interval: 1000

        onTriggered: {
            dropdownChecked = false
        }
    }

    // The default delegate for the collapsed pulldown box
    property Component defaultCollapsedViewDelegate: Item {
        id: defaultCollapsedView
        WGPushButton {
            id: displayButton
            objectName: model != null ? "WGComboBoxImage " + model.get(dropDownCurrentIndex).path : "WGComboBoxImage"
            height: comboboximage.height
            width: comboboximage.width
            style: WGComboBoxButtonStyle {}

            checkable: true
            checked: comboboximage.dropdownChecked
            iconSource: model.get(dropDownCurrentIndex).img
            text: showPath ? model.get(dropDownCurrentIndex).path : ""
            onActiveFocusChanged: {
                if(!activeFocus)
                {
                    comboboximage.dropdownChecked = false
                }
            }

            onCheckedChanged: {
                if (displayButton.checked == true){
                    comboboximage.dropdownChecked = true
                }
                else
                {
                    comboboximage.dropdownChecked = false
                }
            }
        }
    }

    // Loader for the collapsed button
    Loader {
        id: pushButtonDropdownLoader
        anchors.fill: parent
        sourceComponent: defaultCollapsedViewDelegate
    }

    // This item covers the button and the drop down
    Item {
        id: pulldownMenu
        z: 5
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        // pulldownMenu Component coveres entire control to capture mouse leaving pulldown
        height: {
            if (showXItems == 0) {
                return comboboximage.height * model.count + comboboximage.height
            }
            else
                return (comboboximage.height * Math.min(showXItems, model.count)) + comboboximage.height
        }
        width: comboboximage.width
        visible: dropdownChecked

        MouseArea {
            // Triggers the timer when the mouse leaves the control
            id: timerMouseArea
            anchors.fill: parent
            propagateComposedEvents: true
            hoverEnabled: true

            onEntered: {
                fadeTimer.stop()
            }

            onExited: {
                fadeTimer.restart()
            }

            //Required to turn the checked state off if the pushbutton is pressed again.
            onClicked: {
                dropdownChecked = false
            }
        }

        MouseArea {
            // prevents items being selected behind drop down and fires off timer
            // Haven't seen this triggered yet
            id: catchMouseArea
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            height: pulldownMenu.height - comboboximage.height

            //This should stop them escaping
            propagateComposedEvents: false
        }

        Rectangle {
            id: dropdowndropshadow
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: 2
            anchors.topMargin: 2
            anchors.rightMargin: -2
            anchors.bottomMargin: -2
            radius: 3
            visible: true
            height: pulldownMenu.height - comboboximage.height
            color: "#80000000"
        }

        Rectangle {
            id: droprect
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            height: pulldownMenu.height - comboboximage.height

            color: palette.mainWindowColor
            border.width: defaultSpacing.standardBorderSize
            border.color: palette.darkColor

            WGScrollPanel {
                childObject: ListView {
                            id: combolistview
                            height: droprect.height
                            width: droprect.width
                            clip: true
                            anchors.margins: {left: 2; right: 2; top: 5; bottom: 5}
                            model: comboboximageModel
                            delegate: defaultPullDownListViewDelegate
                            }
                        }
        }
    }


    property Component defaultPullDownListViewDelegate: Item {
        id: defaultListViewDeletgate
        objectName: model != null ? "Dropdown " + model.path : "Dropdown"
        width: parent.width
        height: comboboximage.height

        RowLayout {
            id: dropdownrow
            spacing: defaultSpacing.standardMargin
            anchors.left: defaultListViewDeletgate.left
            anchors.right: defaultListViewDeletgate.right
            MouseArea {  //The mouse area for index selection
                id: dropmousearea
                Layout.preferredHeight: comboboximage.height
                Layout.preferredWidth: comboboximage.width

                hoverEnabled: true

                onClicked: {
                    dropDownCurrentIndex = index
                    comboboximage.dropdownChecked = false
                }

                onEntered: {
                    fadeTimer.stop()
                    dropdownhighlight.visible = true
                }

                onExited: {
                    fadeTimer.restart()
                    dropdownhighlight.visible = false
                }

                WGHighlightFrame {
                    id: dropdownhighlight
                    anchors.left: dropmousearea.left
                    anchors.right: dropmousearea.right
                    anchors.rightMargin: defaultSpacing.standardMargin
                    height: comboboximage.height
                    visible: false
                }

                RowLayout {
                    anchors.left: dropmousearea.left
                    anchors.right: dropmousearea.right
                    Image {
                        id: image
                        Layout.preferredHeight: comboboximage.height
                        Layout.preferredWidth: comboboximage.height
                        source: model.img
                    }
                    Text {
                        id: text
                        visible: showpath
                        text: model.path
                    }
                    Item {
                        Layout.fillWidth: true
                    }
                }
            }
        }
    }
}
