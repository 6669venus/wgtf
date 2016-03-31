import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import WGControls 1.0

Rectangle {
    objectName:  itemData != null ? itemData.IndexPath : "property_tree_view"
    property variant source_ : source

    color: palette.mainWindowColor

    WGTreeModel {
        id : model
        objectName: "treeViewModel"

        source : source_

        ValueExtension {}
        ColumnExtension {}
        ComponentExtension {}
        TreeExtension {}
        ThumbnailExtension {}
        SelectionExtension {}
    }

    TreeView {
        objectName: "treeView"
        anchors.fill : parent

        model_ : model
        columnCount_ : 2
        property Component propertyDelegate : Loader {
            clip : true
            sourceComponent : itemData_ != null ? itemData_.Component : null
        }
        columnDelegates_ : [ columnDelegate_, propertyDelegate ]
        clampWidth_ : true
    }
}

