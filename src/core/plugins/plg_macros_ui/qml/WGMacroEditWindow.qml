import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Window 2.2
import BWControls 1.0
import "qrc:/controls"


Window {
    id: root

     property QtObject panelProps: WGPanelProperties{}

    minimumWidth: panelProps.minPanelWidth_
    maximumWidth: panelProps.minPanelWidth_

    flags: Qt.Window | Qt.WindowTitleHint | Qt.CustomizeWindowHint
    color: palette.MainWindowColor
    modality: Qt.ApplicationModal
    property bool accepted: false
    property variant sourceData;
    
   

    BWListModel {
		id : editModel

		source : sourceData

		ValueExtension {}
		ColumnExtension {}
		SelectionExtension {}

	}


	WGFrame{
		id: mainFrame
		anchors.fill: parent

		WGColumnLayout {
			id: mainColumnLayout
			anchors.fill: parent
			anchors.margins: panelProps.standardMargin_

			WGTextBoxFrame{
				id: editFrame
				Layout.fillHeight: true
				Layout.fillWidth: true

    			WGMultiColumnListView {
					id: editMacro
					anchors.fill: parent
					model: editModel
					defaultColumnDelegate:
						"qrc:///plg_macros_ui/WGMacroEditObjectDelegate.qml"
				}
			}

			WGExpandingRowLayout{
				id: buttons
				Layout.preferredHeight: panelProps.rowHeight + panelProps.doubleBorder_
				Layout.fillWidth: true

				WGPushButton {
        			id: okButton
       				text: "Ok"
        			Layout.preferredWidth: 60
       				checkable: false
       				signal onOk();
       				onClicked: {
        				onOk();
            			root.closing( root.close )
            			root.close()
        			}
   				}

    			WGPushButton {
        			id: cancelButton
        			text: "Cancel"
        			Layout.preferredWidth: 60
        			checkable: false
        			signal onCancel();
        			onClicked: {
        				onCancel();
            			root.closing( root.close )
            			root.close()
        			}
    			}
    		}
    	}
    }
}

