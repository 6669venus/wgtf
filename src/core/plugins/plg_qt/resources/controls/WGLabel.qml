import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

//A non-editable single line of text that can align to a panel wide width in panelProps
//Will appear in the left column if placed in a WGFormLayout && formLabel_ == true

Text {
    id: labelText
    objectName: "WGLabel"

    //right aligns the label and sets width to the largest label in the panel.
    property bool formLabel_: false

    //ignores the panel wide label column width
    property bool localForm_: false

    color: {
        if (enabled){
            palette.TextColor
        } else {
            palette.DisabledTextColor
        }
    }

    renderType: Text.NativeRendering

    horizontalAlignment: formLabel_ ? Text.AlignRight : Text.AlignLeft
    //implicitHeight: panelProps.rowHeight_

    width: {
        if (formLabel_ && !localForm_){
            panelProps.labelColumnWidth_
        } else {
            implicitWidth
        }
    }

    Layout.preferredWidth: {
        if (formLabel_ && !localForm_){
            panelProps.labelColumnWidth_
        } else {
            implicitWidth
        }
    }

    Component.onCompleted: {

        if (formLabel_ && paintedWidth > panelProps.labelColumnWidth_ && !localForm_){
            panelProps.labelColumnWidth_ = paintedWidth;
        }
    }
}

