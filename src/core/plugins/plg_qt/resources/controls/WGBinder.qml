import QtQuick 2.0
import QtQuick.Layouts 1.0

WGExpandingRowLayout {
    id: binderLayout

    property int borderWidth_: panelProps.standardBorder_

    property QtObject childObject
    property QtObject parentBinder: null

    property var childData
    property QtObject dataObject: null

    property string binderName: ""

    Component.onCompleted: {
        if(children.length > 1){
            childObject = children[1]
            if(typeof childObject.label_ != "undefined"){
                if (childObject.label_ != ""){
                    var newLabel = Qt.createComponent("WGLabel.qml");
                    if (newLabel.status === Component.Ready){
                        newLabel.createObject(binderLayout, {
                                                   "text": childObject.label_,
                                                   "formLabel_": true,
                                               });
                    }
                    childObject.parent = null
                    childObject.parent = binderLayout
                }
            }
        }
    }

    signal changeSelection()
    signal changeCopied()

    function clearSelected(){
        if(parentBinder != null){
            parentBinder.changeSelection()
        }
    }

    function clearCopy() {
        if(parentBinder != null){
            parentBinder.changeCopied()
        }
    }

    function copyData(){
        if (dataObject != null){
            panelProps.copiedData_ = ""
            panelProps.copiedObject_ = dataObject
        } else {
            panelProps.copiedObject_ = null
            if(typeof childObject.text != "undefined"){
                childData = childObject.text
            } else if (typeof childObject.value != "undefined"){
                childData = childObject.value
            } else if (typeof childObject.value_ != "undefined"){
                childData = childObject.value_
            }
        }
        panelProps.copiedData_ = childData
    }

    function pasteData(copiedData){
        if(typeof childObject.text != "undefined"){
            childObject.text = copiedData
        } else if (typeof childObject.value != "undefined"){
            childObject.value = copiedData
        } else if (typeof childObject.value_ != "undefined"){
            childObject.value_ = copiedData
        }
    }

    function pasteObject(copiedObject){
        dataObject = copiedObject
    }

    Item {

        Rectangle {
            id: copyBorder

            height: binderLayout.height + panelProps.doubleBorder_
            width: binderLayout.width + borderWidth_

            x: -borderWidth_ / 2
            y: -panelProps.doubleBorder_

            radius: panelProps.halfRadius_

            color: "transparent"
            border.width: panelProps.standardBorder_
            border.color: "transparent"

            property bool selected: false
            property bool copied: false

            Rectangle {
                id: barHandle
                width: 3
                height: parent.height - panelProps.standardMargin_
                anchors.verticalCenter: parent.verticalCenter
                x: panelProps.standardMargin_
                color: palette.DisabledTextColor
                visible: panelProps.copyEnabled_
                radius: panelProps.standardRadius_
            }

            Connections {
                target: parentBinder
                onChangeSelection:{
                    copyBorder.selected = false
                    binderLayout.changeSelection()
                }
            }

            Connections {
                target: parentBinder
                onChangeCopied:{
                    copyBorder.copied = false
                    binderLayout.changeCopied()
                }
            }

            Keys.onPressed: {
                if (selected){
                    if ((event.key == Qt.Key_C) && (event.modifiers & Qt.ControlModifier)){
                        copyData()
                        if(parentBinder != null){
                            parentBinder.changeSelection()
                            parentBinder.changeCopied()

                            parentBinder.clearCopy()
                        }
                        copyBorder.selected = false
                        copyBorder.copied = true
                    }

                    if ((event.key == Qt.Key_V) && (event.modifiers & Qt.ControlModifier)){
                        if(panelProps.copiedObject_ != null){
                            pasteObject(panelProps.copiedObject_)
                        } else if (panelProps.copiedObject_ != ""){
                            pasteData(panelProps.copiedData_)
                        }
                    }
                }
            }

            states: [
                State {
                    name: "HIGHLIGHTED"
                    when: copySelect.containsMouse && !copyBorder.selected
                    PropertyChanges {
                        target: copyBorder
                        border.color: palette.HighlightColor
                    }
                    PropertyChanges {
                        target: barHandle
                        color: palette.HighlightColor
                    }
                },
                State {
                    name: "SELECTED"
                    when: copyBorder.selected
                    PropertyChanges {
                        target: copyBorder
                        border.color: "transparent"
                        color: palette.HighlightShade
                    }
                    PropertyChanges {
                        target: barHandle
                        color: palette.HighlightColor
                    }
                },
                State {
                    name: "COPIED"
                    when: copyBorder.copied
                    PropertyChanges {
                        target: copyBorder
                        border.color: palette.LighterShade
                    }
                    PropertyChanges {
                        target: barHandle
                        color: palette.HighlightShade
                    }
                }
            ]

            MouseArea {
                id: copySelect
                anchors.fill: parent
                enabled: panelProps.copyEnabled_
                hoverEnabled: true

                onClicked: {
                    if ((mouse.button == Qt.LeftButton) && (mouse.modifiers & Qt.ControlModifier)){
                        if(copyBorder.selected){
                            copyBorder.selected = false
                            copyBorder.forceActiveFocus()
                        } else {
                            copyBorder.selected = true
                            copyBorder.forceActiveFocus()
                        }
                    } else if (mouse.button == Qt.LeftButton) {
                        if(!copyBorder.selected){
                            if(parentBinder != null){
                                parentBinder.changeSelection()
                                parentBinder.clearSelected()
                            }
                            copyBorder.selected = true
                            copyBorder.forceActiveFocus()
                        }
                    }
                }
            }
        }
    }
}
