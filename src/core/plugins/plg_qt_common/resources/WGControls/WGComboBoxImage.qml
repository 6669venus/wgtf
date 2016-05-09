import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import BWControls 1.0
import QtQml.Models 2.2
import WGControls 1.0

/*  TODO
    1. The drop down will not always draw above other neighboring components
    Z  height is unfortunately only relative to siblings. This is fine for custom controls where
    Z  height can be manually set for all components but it will cause problems for property panels.
    2. Try to get mouse over dropdown highlight working
    3. Limit the list size
    4. Provide a scrollbar?
    5. Base width on the largest text path
    6. Dropdown should disappear on focus change
    7. Is this control data compliant?
    8. Check for disabled state
    9. What about lists of size 1, disable drop down arrow in style?
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
    width: 200 + 2 * defaultSpacing.standardMargin
    height: defaultSpacing.minimumRowHeight * 2

    property bool showpath: true
    property int dropDownCurrentIndex: 0
    property bool dropdownChecked: false

    property ListModel model: ListModel {
        id: comboboximageModel
        ListElement {path: "example/path1"; img: "icons/proxyThumbnail.png"}
        ListElement {path: "example/path2"; img: "icons/diffuse3.png"}
        ListElement {path: "example/path3"; img: "icons/modelThumbnail.png"}
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
            text: model.get(dropDownCurrentIndex).path

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

    // ToDo. Limit the size of the drop down and provide a scroll
    Item {
        id: pulldownMenu
        z: 5
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        // pulldownMenu Component coveres entire control to capture mouse leaving pulldown
        height: comboboximage.height * model.count + comboboximage.height
        width: comboboximage.width
        visible: dropdownChecked

        // WE MIGHT NEED TWO MOUSE AREAS.. ONE FOR TIMER OVER ENTIRE FIELD
        // ONE FOR STOPPING SELECTION OF CONTROLS UNDER

        MouseArea {
            // Triggers the timeer when the mouse leaves the control
            id: timerMouseArea
            anchors.fill: parent
            propagateComposedEvents: true

            hoverEnabled: true
            preventStealing: true

            onEntered: {
                fadeTimer.stop()
            }

            onExited: {
                fadeTimer.restart()
            }
        }

        MouseArea {
            // prevents items being selected behind drop down and fires off timer
            // Haven't seen this triggered yet
            id: catchMouseArea
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            height: comboboximage.height * model.count

            propagateComposedEvents: false //This should stop them escaping

            /*onClicked: {
                console.log("STOPPED")
            }*/
        }

        //ORIGINAL LOCATION OF LISTVIEW
        Rectangle {
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            height: comboboximage.height * model.count
            color: palette.mainWindowColor
            border.width: defaultSpacing.standardBorderSize
            border.color: palette.darkColor
            visible: true

            // Iconsize Pulldown Buttons
            ListView {
                id: combolistview
                height: parent.height
                width: parent.width
                anchors.margins: {left: 2; right: 2; top: 5; bottom: 5}
                model: comboboximageModel
                delegate: defaultPullDownListViewDelegate
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

                //Todo: using hoverEnabled here causes timerMouseArea to onExit and triggers the timer resulting
                //in the drop down disappearing.
                //hoverEnabled: true

                onClicked: {
                    dropDownCurrentIndex = index
                    comboboximage.dropdownChecked = false
                }

                //todo
                onEntered: {
                    dropdownhighlight.visible = true
                }

                onExited: {
                    dropdownhighlight.visible = false
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
                        Layout.alignment: Qt.AlignHCenter
                        text: model.path
                    }
                }

                Rectangle { //NOTE: the position of this highlight is incorrect for now
                    id: dropdownhighlight
                    anchors.left: dropmousearea.left
                    anchors.right: dropmousearea.right
                    height: comboboximage.height
                    border.color: palette.highlightColor
                    border.width: 1
                    radius: defaultSpacing.standardRadius
                    color: "transparent"
                    // Todo. Switched off for now until highlight sorted out
                    visible: false
                }
            }
        }
    }
}
