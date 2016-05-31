import QtQuick 2.3


WGTextBox {
    id: textField
    objectName:  itemData != null ? itemData.IndexPath : "string_component"
    anchors.left: parent.left
    anchors.right: parent.right
    text: itemData.Value

    onEditAccepted: {
        itemData.Value = text;
    }
}

