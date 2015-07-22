import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import BWControls 1.0
import BWControls 1.0
import WGControls 1.0

Rectangle 
{
	property var title: "Nodes"
	property var layoutHints: { 'tree': 1.0, 'nodes': 1.0, 'left': 0.5 }
    property variant source_ : source

    color: palette.MainWindowColor

    WGTreeModel 
	{
        id : model
        source : source_

        ValueExtension {}
        ColumnExtension {}
        ComponentExtension {}
        TreeExtension {}
        ThumbnailExtension {}
        SelectionExtension {}
    }

    Item 
	{
        anchors.top : parent.top
        anchors.left : parent.left
        anchors.right : parent.right
        anchors.bottom : parent.bottom

        TreeView
		 {
            model_ : model
            columnCount_ : 2

            property Component propertyDelegate : Loader 
			{
                clip : true
                sourceComponent : itemData_ != null ? itemData_.Component : null
            }

            columnDelegates_ : [ columnDelegate_, propertyDelegate ]
            clampWidth_ : true
        }
    }
}

