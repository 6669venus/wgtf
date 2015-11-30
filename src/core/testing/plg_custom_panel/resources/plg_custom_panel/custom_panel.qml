import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

import BWControls 1.0
import WGControls 1.0

WGPanel {

    title: qsTr("Custom Testing Panel")
    layoutHints: { 'test': 0.1 }

    color: palette.MainWindowColor

    focus: true

    property QtObject draggedObject: null
    property QtObject highlightedObject: null
    property bool draggingPanel: false
    property int draggedPanelDepth: -1

    function findFilteredObject(parentObject){
        for(var i=0; i<parentObject.children.length; i++)
        {
            if(typeof parentObject.children[i].filtered != "undefined")
            {
                if(parentObject.children[i].filtered)
                {
                    highlightedObject = parentObject.children[i]
                }
                else
                {
                    findFilteredObject(parentObject.children[i])
                }
            }
            else
            {
                findFilteredObject(parentObject.children[i])
            }
        }
    }

    WGColumnLayout {
        id: baseLayout
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        WGMainPanel {
            id: mainPanel

            text: "Test Panel"
            subText: "Selected Object"

            panelDepth: 1
            choosePinned: false

            Keys.onPressed: {
                if (event.key == Qt.Key_Slash)
                {
                    controlFilter.selectAll()
                    controlFilter.forceActiveFocus()
                }
            }


            headerObject_ :
            WGExpandingRowLayout {

                Rectangle {
                    color: "transparent"
                    Layout.fillWidth: true
                }

                WGToolButton {
                    id: pinButton
                    Layout.preferredHeight: 18
                    Layout.preferredWidth: 18

                    iconSource: mainPanel.expanded_ == 1 ? "icons/pinned_16x16.png" : "icons/pin_16x16.png"

                    checkable: true
                    checked: mainPanel.choosePinned

                    onCheckedChanged: {
                        mainPanel.choosePinned = checked
                        if(checked)
                        {
                            lockButton.checked = false
                        }
                    }
                }

                WGToolButton {
                    id: lockButton
                    Layout.preferredHeight: 18
                    Layout.preferredWidth: 18

                    iconSource: checked ? "icons/unlock_16x16.png" : "icons/lock_16x16.png"

                    checkable: true

                    enabled: mainPanel.expanded_ > 0

                    onCheckedChanged: {
                        mainPanel.chunkDragEnabled = checked
                        if(checked)
                        {
                            pinButton.checked = false
                        }
                    }
                }

                WGToolButton {
                    id: panelMenu
                    Layout.preferredHeight: 18
                    Layout.preferredWidth: 18

                    iconSource: "icons/menu_16x16.png"

                    menu: WGMenu{
                        MenuItem {
                            text: "Copy Panel Data"
                            enabled: false
                        }
                        MenuItem {
                            text: "Paste Panel Data"
                            enabled: false
                        }
                    }
                }

                WGToolButton {
                    Layout.preferredHeight: 18
                    Layout.preferredWidth: 18

                    iconSource: "icons/close_16x16.png"

                    onClicked: {
                        mainWindow.close()
                    }
                }
            }

            childObject_:

            ColumnLayout {
                id: rootFrame

                anchors.left: parent.left
                anchors.right: parent.right

                WGTextBox {
                    id: searchBox
                    visible: mainPanel.expanded_ > 0
                    Layout.fillWidth: true

                    WGToolButton {
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        anchors.right: parent.right
                        width: height
                        opacity: 0.2
                        activeFocusOnTab: false

                        iconSource: "icons/close_16x16.png"

                        onClicked: {
                            searchBox.text = ""
                        }
                    }
                    WGLabel {
                        text: "Search..."
                        font.italic: true
                        color: "#666666"
                        anchors.fill: parent
                        anchors.leftMargin: 5
                        visible: searchBox.text == "" && !searchBox.focus
                    }

                    Component.onCompleted: {
                        mainPanel.controlFilter = this
                    }

                    Keys.onTabPressed: {
                        findFilteredObject(rootFrame)
                        if(highlightedObject != null)
                        {
                            highlightedObject.nextItemInFocusChain(true).forceActiveFocus()
                            searchBox.text = ""
                        }
                        else
                        {
                            searchBox.nextItemInFocusChain(true).forceActiveFocus()
                        }
                    }
                    onTextChanged: {
                        highlightedObject = null
                    }
                }

                WGControlChunk {
                    id: panelChunkOne
                    parentPanel: mainPanel
                    panelChunk: true
                    tags: "sub panel 1 one"
                    panelDepth: 1
                    layoutRow: 0
                    WGColumnLayout {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        WGProtoPanel {
                            text: "Sub Panel 1"
                            pinned: panelChunkOne.pinned
                            panelDepth: 2
                            childObject_:
                            WGColumnLayout {
                                id: panelOneFrame
                                anchors.left: parent.left
                                anchors.right: parent.right
                                property bool pinned: false
                                WGControlChunk {
                                    tags: "text enter"
                                    WGColumnLayout {
                                        anchors.left: parent.left
                                        anchors.right: parent.right

                                        WGExpandingRowLayout {
                                            Layout.fillWidth: true
                                            WGLabel {
                                                text: "Text:"
                                                horizontalAlignment: Text.AlignRight
                                                Layout.preferredWidth: 60
                                            }
                                            WGTextBox {
                                                Layout.fillWidth: true
                                            }
                                        }

                                        WGExpandingRowLayout {
                                            Layout.fillWidth: true
                                            Rectangle {
                                                color: "transparent"
                                                Layout.preferredWidth: 60
                                            }
                                            WGPushButton {
                                                text: "Enter"
                                            }
                                        }
                                    }
                                }

                                WGControlChunk {
                                    tags: "slider"
                                    WGExpandingRowLayout {
                                        anchors.left: parent.left
                                        anchors.right: parent.right
                                        WGLabel {
                                            text: "Slider:"
                                            horizontalAlignment: Text.AlignRight
                                            Layout.preferredWidth: 60
                                        }
                                        WGSliderControl {
                                            Layout.fillWidth: true
                                            value: 50
                                            minimumValue: 0
                                            maximumValue: 100
                                        }
                                    }
                                }

                                WGControlChunk {
                                    tags: "booleans checkboxes option"
                                    WGColumnLayout {
                                        anchors.left: parent.left
                                        anchors.right: parent.right

                                        WGExpandingRowLayout {
                                            Layout.fillWidth: true
                                            WGLabel {
                                                text: "Booleans:"
                                                horizontalAlignment: Text.AlignRight
                                                Layout.preferredWidth: 60
                                            }
                                            WGCheckBox {
                                                text: "Option 1"
                                                checked: true
                                            }
                                        }

                                        WGExpandingRowLayout {
                                            Layout.fillWidth: true
                                            Rectangle {
                                                color: "transparent"
                                                Layout.preferredWidth: 60
                                            }
                                            WGCheckBox {
                                                text: "Option 2"
                                                checked: false
                                            }
                                        }

                                        WGExpandingRowLayout {
                                            Layout.fillWidth: true
                                            Rectangle {
                                                color: "transparent"
                                                Layout.preferredWidth: 60
                                            }
                                            WGCheckBox {
                                                text: "Option 3"
                                                checked: false
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }


                WGControlChunk {
                    id: panelChunkTwo
                    parentPanel: mainPanel
                    panelChunk: true
                    tags: "sub panel 2 two"
                    panelDepth: 1
                    layoutRow: 1
                    WGColumnLayout {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        WGProtoPanel {
                            text: "Sub Panel 2"
                            pinned: panelChunkTwo.pinned
                            panelDepth: 3
                            childObject_:
                            WGColumnLayout {
                                id: panelTwoFrame
                                anchors.left: parent.left
                                anchors.right: parent.right
                                property bool pinned: false
                                WGControlChunk {
                                    tags: "data performancebar number value"
                                    WGColumnLayout {
                                        anchors.left: parent.left
                                        anchors.right: parent.right

                                        WGExpandingRowLayout {
                                            Layout.fillWidth: true
                                            WGLabel {
                                                text: "Data:"
                                                horizontalAlignment: Text.AlignRight
                                                Layout.preferredWidth: 60
                                            }
                                            WGPerformanceBar {
                                                id: perfBar
                                                Layout.fillWidth: true
                                            }
                                        }

                                        WGExpandingRowLayout {
                                            Layout.fillWidth: true
                                            Rectangle {
                                                color: "transparent"
                                                Layout.preferredWidth: 60
                                            }
                                            WGNumberBox {
                                                Layout.preferredWidth: 80
                                                value: 25
                                                minimumValue: 0
                                                maximumValue: 100

                                                b_Target: perfBar
                                                b_Property: "value_"
                                                b_Value: value
                                            }
                                        }
                                    }
                                }



                                WGControlChunk {
                                    tags: "radio boolean feature on off"
                                    WGColumnLayout {
                                        anchors.left: parent.left
                                        anchors.right: parent.right

                                        ExclusiveGroup {
                                            id: radioGroup
                                        }

                                        WGExpandingRowLayout {
                                            Layout.fillWidth: true
                                            WGLabel {
                                                text: "Feature:"
                                                horizontalAlignment: Text.AlignRight
                                                Layout.preferredWidth: 60
                                            }
                                            WGRadioButton {
                                                text: "On"
                                                checked: true
                                                exclusiveGroup: radioGroup
                                            }
                                        }

                                        WGExpandingRowLayout {
                                            Layout.fillWidth: true
                                            Rectangle {
                                                color: "transparent"
                                                Layout.preferredWidth: 60
                                            }
                                            WGRadioButton {
                                                text: "Off"
                                                checked: false
                                                exclusiveGroup: radioGroup
                                            }
                                        }
                                    }
                                }

                                WGControlChunk {
                                    tags: "image click thumbnail load picture"
                                    WGExpandingRowLayout {
                                        anchors.left: parent.left
                                        anchors.right: parent.right
                                        Rectangle {
                                            color: "transparent"
                                            Layout.preferredWidth: 60
                                        }
                                        WGThumbnailButton {
                                            id: openImage
                                            defaultText: "Click to Load an Image"
                                            Layout.preferredWidth: implicitWidth
                                        }
                                        Rectangle {
                                            color: "transparent"
                                            Layout.fillWidth: true
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        } //first panel

        WGMainPanel {
            id: customPanel
            text: "Custom Panel"

            panelDepth: 4

            Component.onCompleted: {
                y = mainPanel.y + mainPanel.height
                pinChildren(customPanel,false)
            }

            choosePinned: false

            Keys.onPressed: {
                if (event.key == Qt.Key_Slash)
                {
                    controlFilter.selectAll()
                    controlFilter.forceActiveFocus()
                }
            }

            function pinChildren(parentObject, pin)
            {
                for(var i=0; i<parentObject.children.length; i++)
                {
                    if(typeof parentObject.children[i].pinned !== "undefined")
                    {
                        if(pin)
                        {
                            parentObject.children[i].pinned = true
                            if(parentObject.children[i].clonedControl)
                            {
                                parentObject.children[i].cloned = true
                            }
                        }
                        else
                        {
                            parentObject.children[i].pinned = false
                            if(parentObject.children[i].clonedControl)
                            {
                                parentObject.children[i].cloned = false
                            }
                        }
                    }
                    pinChildren(parentObject.children[i], pin)
                }
            }

            headerObject_ :
            WGExpandingRowLayout {

                Rectangle {
                    color: "transparent"
                    Layout.fillWidth: true
                }



                WGToolButton {
                    id: pinButtonClone
                    Layout.preferredHeight: 18
                    Layout.preferredWidth: 18

                    iconSource: customPanel.expanded_ == 1 ? "icons/pinned_16x16.png" : "icons/pin_16x16.png"

                    checkable: true
                    checked: customPanel.choosePinned

                    onCheckedChanged: {
                        customPanel.choosePinned = checked
                        if(checked)
                        {
                            lockButtonClone.checked = false
                        }
                    }
                }

                WGToolButton {
                    id: lockButtonClone
                    Layout.preferredHeight: 18
                    Layout.preferredWidth: 18

                    iconSource: checked ? "icons/unlock_16x16.png" : "icons/lock_16x16.png"

                    checkable: true

                    enabled: customPanel.expanded_ == 2

                    onCheckedChanged: {
                        customPanel.chunkDragEnabled = checked
                        if(checked)
                        {
                            pinButtonClone.checked = false
                        }
                    }
                }

                WGToolButton {
                    id: panelMenuClone
                    Layout.preferredHeight: 18
                    Layout.preferredWidth: 18

                    iconSource: "icons/menu_16x16.png"

                    menu: WGMenu{
                        MenuItem {
                            text: "Copy Panel Data"
                            enabled: false
                        }
                        MenuItem {
                            text: "Paste Panel Data"
                            enabled: false
                        }
                    }
                }

                WGToolButton {
                    Layout.preferredHeight: 18
                    Layout.preferredWidth: 18

                    iconSource: "icons/close_16x16.png"

                    onClicked: {
                        mainWindow.close()
                    }
                }
            }

            childObject_:
            ColumnLayout {
                id: customFrame

                anchors.top: mainPanel.bottom
                anchors.left: parent.left
                anchors.right: parent.right

                WGTextBox {
                    id: searchBoxClone
                    visible: customPanel.expanded_ > 0
                    Layout.fillWidth: true

                    WGToolButton {
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        anchors.right: parent.right
                        width: height
                        opacity: 0.2
                        activeFocusOnTab: false

                        iconSource: "icons/close_16x16.png"

                        onClicked: {
                            searchBoxClone.text = ""
                        }
                    }
                    WGLabel {
                        text: "Search..."
                        font.italic: true
                        color: "#666666"
                        anchors.fill: parent
                        anchors.leftMargin: 5
                        visible: searchBoxClone.text == "" && !searchBoxClone.focus
                    }

                    Component.onCompleted: {
                        customPanel.controlFilter = this
                    }

                    Keys.onTabPressed: {
                        findFilteredObject(customFrame)
                        if(highlightedObject != null)
                        {
                            highlightedObject.nextItemInFocusChain(true).forceActiveFocus()
                            searchBoxClone.text = ""
                        }
                        else
                        {
                            searchBoxClone.nextItemInFocusChain(true).forceActiveFocus()
                        }
                    }
                    onTextChanged: {
                        highlightedObject = null
                    }
                }

                property int copiedControls: 0
                Rectangle {
                    id: cloneFrame
                    Layout.fillWidth: true
                    Layout.preferredHeight: 50

                    visible: draggedObject != null && !draggedObject.clonedControl

                    color: "transparent"

                    border.width: defaultSpacing.standardBorderSize
                    border.color: "transparent"

                    states: [
                        State {
                            name: "DRAG_NORMAL"
                            when: draggedObject != null
                            PropertyChanges{ target: cloneFrame; border.color: palette.LighterShade}
                        },
                        State {
                            name: "DRAG_OVER"
                            when: draggedObject != null
                            PropertyChanges{ target: cloneFrame; border.color: palette.HighlightShade}
                        }
                    ]

                    DropArea {
                        id: cloneTarget
                        anchors.fill: parent
                        onEntered: {
                            cloneFrame.state = "DRAG_OVER"
                        }
                        onExited: {
                            cloneFrame.state = "DRAG_NORMAL"
                        }
                        onDropped: {
                            drop.acceptProposedAction()
                        }
                    }
                    Rectangle {
                        z: 10
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter

                        width: defaultSpacing.minimumRowHeight
                        height: defaultSpacing.minimumRowHeight

                        visible: draggedObject != null
                        color: cloneFrame.state == "DRAG_OVER" ? palette.HighlightShade : palette.LighterShade
                        radius: defaultSpacing.minimumRowHeight

                        Image {
                            anchors.centerIn: parent
                            source: "icons/add_16x16.png"
                        }
                    }
                }

                WGControlChunk {
                    id: panelChunkOneClone
                    parentPanel: customPanel
                    panelChunk: true
                    tags: "sub panel 1 one"
                    clonedControl: true
                    panelDepth: 4
                    layoutRow: 0
                    Connections {
                        target: cloneTarget
                        onDropped: {
                            if(panelChunkOneClone.tags == draggedObject.tags)
                            {
                                panelChunkOneClone.pinned = true
                                panelChunkOneClone.cloned = true
                                copiedControls += 1
                                clonedPanelOne.collectChildren()
                                customPanel.pinChildren(panelChunkOneClone, true)
                                panelChunkOneClone.rootPanel.showPinned()
                                clonedPanelOne.resetPanelOrder()
                            }
                        }
                    }
                    WGColumnLayout {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        WGProtoPanel {
                            id: clonedPanelOne
                            text: "Sub Panel 1"
                            pinned: panelChunkOneClone.pinned
                            panelDepth: 5
                            childObject_:
                            WGColumnLayout {
                                id: panelOneFrameClone
                                anchors.left: parent.left
                                anchors.right: parent.right
                                property bool pinned: false
                                WGControlChunk {
                                    id: clonedText
                                    clonedControl: true
                                    Connections {
                                        target: cloneTarget
                                        onDropped: {
                                            if(clonedText.tags == draggedObject.tags)
                                            {
                                                clonedText.pinned = true
                                                clonedText.cloned = true
                                                copiedControls += 1
                                                clonedText.rootPanel.showPinned()
                                            }
                                        }
                                    }
                                    tags: "text enter"
                                    WGColumnLayout {
                                        anchors.left: parent.left
                                        anchors.right: parent.right

                                        WGExpandingRowLayout {
                                            Layout.fillWidth: true
                                            WGLabel {
                                                text: "Text:"
                                                horizontalAlignment: Text.AlignRight
                                                Layout.preferredWidth: 60
                                            }
                                            WGTextBox {
                                                Layout.fillWidth: true
                                            }
                                        }

                                        WGExpandingRowLayout {
                                            Layout.fillWidth: true
                                            Rectangle {
                                                color: "transparent"
                                                Layout.preferredWidth: 60
                                            }
                                            WGPushButton {
                                                text: "Enter"
                                            }
                                        }
                                    }
                                }

                                WGControlChunk {
                                    id: clonedSlider
                                    clonedControl: true
                                    Connections {
                                        target: cloneTarget
                                        onDropped: {
                                            if(clonedSlider.tags == draggedObject.tags)
                                            {
                                                clonedSlider.pinned = true
                                                clonedSlider.cloned = true
                                                copiedControls += 1
                                                clonedSlider.rootPanel.showPinned()
                                            }
                                        }
                                    }
                                    tags: "slider"
                                    WGExpandingRowLayout {
                                        anchors.left: parent.left
                                        anchors.right: parent.right
                                        WGLabel {
                                            text: "Slider:"
                                            horizontalAlignment: Text.AlignRight
                                            Layout.preferredWidth: 60
                                        }
                                        WGSliderControl {
                                            Layout.fillWidth: true
                                            value: 50
                                            minimumValue: 0
                                            maximumValue: 100
                                        }
                                    }
                                }

                                WGControlChunk {
                                    id: clonedBooleans
                                    clonedControl: true
                                    Connections {
                                        target: cloneTarget
                                        onDropped: {
                                            if(clonedBooleans.tags == draggedObject.tags)
                                            {
                                                clonedBooleans.pinned = true
                                                clonedBooleans.cloned = true
                                                copiedControls += 1
                                                clonedBooleans.rootPanel.showPinned()
                                            }
                                        }
                                    }
                                    tags: "booleans checkboxes option"
                                    WGColumnLayout {
                                        anchors.left: parent.left
                                        anchors.right: parent.right

                                        WGExpandingRowLayout {
                                            Layout.fillWidth: true
                                            WGLabel {
                                                text: "Booleans:"
                                                horizontalAlignment: Text.AlignRight
                                                Layout.preferredWidth: 60
                                            }
                                            WGCheckBox {
                                                text: "Option 1"
                                                checked: true
                                            }
                                        }

                                        WGExpandingRowLayout {
                                            Layout.fillWidth: true
                                            Rectangle {
                                                color: "transparent"
                                                Layout.preferredWidth: 60
                                            }
                                            WGCheckBox {
                                                text: "Option 2"
                                                checked: false
                                            }
                                        }

                                        WGExpandingRowLayout {
                                            Layout.fillWidth: true
                                            Rectangle {
                                                color: "transparent"
                                                Layout.preferredWidth: 60
                                            }
                                            WGCheckBox {
                                                text: "Option 3"
                                                checked: false
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                WGControlChunk {
                    id: panelChunkTwoClone
                    parentPanel: customPanel
                    panelChunk: true
                    tags: "sub panel 2 two"
                    panelDepth: 4
                    layoutRow: 1
                    clonedControl: true
                    Connections {
                        target: cloneTarget
                        onDropped: {
                            if(panelChunkTwoClone.tags == draggedObject.tags)
                            {
                                panelChunkTwoClone.pinned = true
                                panelChunkTwoClone.cloned = true
                                copiedControls += 1
                                clonedPanelTwo.collectChildren()
                                customPanel.pinChildren(panelChunkTwoClone,true)
                                panelChunkTwoClone.rootPanel.showPinned()
                                clonedPanelTwo.resetPanelOrder()
                            }
                        }
                    }
                    WGColumnLayout {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        WGProtoPanel {
                            id: clonedPanelTwo
                            text: "Sub Panel 2"
                            pinned: panelChunkTwoClone.pinned

                            panelDepth: 6
                            childObject_:
                            WGColumnLayout {
                                id: panelTwoFrameClone
                                anchors.left: parent.left
                                anchors.right: parent.right
                                property bool pinned: false
                                WGControlChunk {
                                    id: clonedBar
                                    clonedControl: true
                                    Connections {
                                        target: cloneTarget
                                        onDropped: {
                                            if(clonedBar.tags == draggedObject.tags)
                                            {
                                                clonedBar.pinned = true
                                                clonedBar.cloned = true
                                                copiedControls += 1
                                                clonedBar.rootPanel.showPinned()
                                            }
                                        }
                                    }
                                    tags: "data performancebar number value"
                                    WGColumnLayout {
                                        anchors.left: parent.left
                                        anchors.right: parent.right

                                        WGExpandingRowLayout {
                                            Layout.fillWidth: true
                                            WGLabel {
                                                text: "Data:"
                                                horizontalAlignment: Text.AlignRight
                                                Layout.preferredWidth: 60
                                            }
                                            WGPerformanceBar {
                                                id: perfBarClone
                                                Layout.fillWidth: true
                                            }
                                        }

                                        WGExpandingRowLayout {
                                            Layout.fillWidth: true
                                            Rectangle {
                                                color: "transparent"
                                                Layout.preferredWidth: 60
                                            }
                                            WGNumberBox {
                                                Layout.preferredWidth: 80
                                                value: 25
                                                minimumValue: 0
                                                maximumValue: 100

                                                b_Target: perfBarClone
                                                b_Property: "value_"
                                                b_Value: value
                                            }
                                        }
                                    }
                                }



                                WGControlChunk {
                                    id: clonedRadio
                                    clonedControl: true
                                    Connections {
                                        target: cloneTarget
                                        onDropped: {
                                            if(clonedRadio.tags == draggedObject.tags)
                                            {
                                                clonedRadio.pinned = true
                                                clonedRadio.cloned = true
                                                copiedControls += 1
                                                clonedRadio.rootPanel.showPinned()
                                            }
                                        }
                                    }
                                    tags: "radio boolean feature on off"
                                    WGColumnLayout {
                                        anchors.left: parent.left
                                        anchors.right: parent.right

                                        ExclusiveGroup {
                                            id: radioGroupClone
                                        }

                                        WGExpandingRowLayout {
                                            Layout.fillWidth: true
                                            WGLabel {
                                                text: "Feature:"
                                                horizontalAlignment: Text.AlignRight
                                                Layout.preferredWidth: 60
                                            }
                                            WGRadioButton {
                                                text: "On"
                                                checked: true
                                                exclusiveGroup: radioGroupClone
                                            }
                                        }

                                        WGExpandingRowLayout {
                                            Layout.fillWidth: true
                                            Rectangle {
                                                color: "transparent"
                                                Layout.preferredWidth: 60
                                            }
                                            WGRadioButton {
                                                text: "Off"
                                                checked: false
                                                exclusiveGroup: radioGroupClone
                                            }
                                        }
                                    }
                                }

                                WGControlChunk {
                                    id: clonedThumb
                                    clonedControl: true
                                    Connections {
                                        target: cloneTarget
                                        onDropped: {
                                            if(clonedThumb.tags == draggedObject.tags)
                                            {
                                                clonedThumb.pinned = true
                                                clonedThumb.cloned = true
                                                copiedControls += 1
                                                clonedThumb.rootPanel.showPinned()
                                            }
                                        }
                                    }
                                    tags: "image click thumbnail load picture"
                                    WGExpandingRowLayout {
                                        anchors.left: parent.left
                                        anchors.right: parent.right
                                        Rectangle {
                                            color: "transparent"
                                            Layout.preferredWidth: 60
                                        }
                                        WGThumbnailButton {
                                            id: openImageClone
                                            defaultText: "Click to Load an Image"
                                            Layout.preferredWidth: implicitWidth
                                        }
                                        Rectangle {
                                            color: "transparent"
                                            Layout.fillWidth: true
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        } //second panel
    } //column layout

    Rectangle {
        id: tooltip
        z: 100
        height: 60
        implicitWidth: 360
        radius: defaultSpacing.standardRadius
        border.width: 1
        border.color: palette.DarkestShade
        color: "#DD222222"
        visible: false

        WGExpandingRowLayout {
            // the anchors will work for width but we'll need to keep using childrenRect.height or we'll get loops. In this case with some extra padding from defaultSpacing.doubleMargin
            anchors.left: parent.left
            anchors.right: parent.right
            height: 60
            anchors.verticalCenter: parent.verticalCenter
            anchors.margins: defaultSpacing.doubleMargin

            // As this is written the icon will be centred inside the alert. This looks fine if the message is only one line but odd if it gets larger.
            // Changing the Layout alignment for the icon to Qt::AlignTop might be better.

            Rectangle {
                id: info
                Layout.preferredWidth: 32
                Layout.preferredHeight: 32
                color: "#ffffff"
                radius: 16
                Layout.alignment: Qt.AlignVCenter

                Rectangle {
                    id: info2
                    anchors.centerIn: parent
                    width: 26
                    height: 26
                    color: "#99c65d"
                    radius: 13

                    Text {
                        id: text1
                        anchors.centerIn: parent
                        color: "#ffffff"
                        text: qsTr("?")
                        font.family: "Courier"
                        font.pixelSize: 24
                        font.bold: true
                    }
                }
            }

            // Changed this to a WGMultiLineText so it will word wrap if the message is too long.
            // Text is an odd exception where it doesn't need a specific height or width - it gets this from the font.pixelSize and the text itself.
            WGMultiLineText {
                id: message
                text: "This is a new control added in the last update. It has some new features."
                font.pixelSize: 14
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignBottom
                Layout.fillWidth: true
            }
        }
    }
}
