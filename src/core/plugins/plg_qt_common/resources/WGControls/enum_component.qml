import QtQuick 2.3
import QtQuick.Controls 1.2
import WGControls 1.0
import WGControls 2.0 as WGTwo
import BWControls 1.0

WGTwo.WGDropDownBox {
    id: combobox
    objectName:  itemData != null ? itemData.IndexPath : "enum_component"
    anchors.left: parent.left
    anchors.right: parent.right

    Component.onCompleted: {
        currentIndex = Qt.binding( function() {
            var modelIndex = enumModel.find( itemData.Value, "Value" );
            return enumModel.indexRow( modelIndex ); } )
    }
    /*
    // just for the purpose of dynamically generating dropdown list when users click on the dropdownbox
    onPressedChanged: {
        if( pressed )
        {
            enumModel.source = itemData.EnumModel
        }
    }*/

    model: enumModel
    textRole: "display"

    WGListModel {
        id: enumModel
        source: itemData.EnumModel

        ValueExtension {}
    }

    Connections {
        target: combobox
        onCurrentIndexChanged: {
            if (currentIndex < 0) {
                return;
            }
            var modelIndex = enumModel.index( currentIndex );
            itemData.Value = enumModel.data( modelIndex, "Value" );
        }
    }
}
