import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Window 2.2
import WGControls 1.0


Window {
    id: root

    minimumWidth: defaultSpacing.minimumPanelWidth
    maximumWidth: defaultSpacing.minimumPanelWidth

    flags: Qt.Window | Qt.WindowTitleHint | Qt.CustomizeWindowHint
    color: palette.MainWindowColor
    modality: Qt.ApplicationModal
    property bool accepted: false
    property variant sourceData;
    
   

    WGListModel {
		id : editModel

		source : sourceData

		ValueExtension {}
		ColumnExtension {}

	}


	WGFrame{
		id: mainFrame
		anchors.fill: parent

		WGColumnLayout {
			id: mainColumnLayout
			anchors.fill: parent
			anchors.margins: defaultSpacing.standardMargin

			WGTextBoxFrame{
				id: editFrame
				Layout.fillHeight: true
				Layout.fillWidth: true

				WGListView {
					id: editMacro
					anchors.fill: parent
					model: editModel
					spacing: 1
					defaultColumnDelegate: Component {
						Item {
							id: macroEditObject
							width: 200
							height: defaultSpacing.minimumRowHeight
							property variant path;
							property variant value;
							property variant oldPath : itemData.Value.PropertyPath;
							property variant oldValue : itemData.Value.PropertyValue;

							function onOkHandler() {
								if((oldPath == path) && (oldValue == value))
								{
									return;
								}
								
								itemData.Value.PropertyPath = path
								// TODO how to convert value from string to PropertyValue's type
								itemData.Value.PropertyValue = value;
								
								root.accepted = true
							}
							function onCancelHandler() {
								if(oldPath == null)
								{
									text1.text = ""
								}
								else
								{
									text1.text = oldPath
								}

								if(oldValue == null)
								{
									text2.text = ""
								}
								else
								{
									text2.text = oldValue.toString()
								}
							}
							Component.onCompleted: {
								okButton.onOk.connect( onOkHandler )
								cancelButton.onCancel.connect( onCancelHandler )
							}

							WGExpandingRowLayout {
								id: macroEditObjectRow
								anchors.fill: parent
								Text {
									id: label1
									Layout.preferredWidth: paintedWidth
									clip: false
									text: "Property:"
									color: palette.TextColor
								}

								WGTextBox {
									id: text1
									clip: false
									Layout.fillWidth: true
									text: oldPath
									onTextChanged: {
										path = text
									}
								}

								WGSeparator {
									vertical_: true
								}

								Text {
									id: label2
									Layout.preferredWidth: paintedWidth
									clip: false
									text: "Value:"
									color: palette.TextColor
								}


								WGTextBox {
									id: text2
									clip: false
									Layout.fillWidth: true
									text: oldValue.toString()
									onTextChanged: {
										value = text
									}
								}
							}
						}

					}
				}
			}

			WGExpandingRowLayout{
				id: buttons
				Layout.preferredHeight: defaultSpacing.minimumRowHeight + defaultSpacing.doubleBorderSize
				Layout.fillWidth: true

				WGPushButton {
        			id: okButton
       				text: "Ok"
        			Layout.preferredWidth: 60
       				checkable: false
       				signal onOk();
       				onClicked: {
						beginUndoFrame();
        				onOk();
						if(root.accepted)
						{
							endUndoFrame();
						}
						else
						{
							abortUndoFrame();
						}
						
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

