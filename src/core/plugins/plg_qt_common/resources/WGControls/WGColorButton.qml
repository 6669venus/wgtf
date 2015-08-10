import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import BWControls 1.0

//Raised solid color button

WGPushButton {
    id: colorButton
	property color color: "#999999"
	property bool defaultColorDialog: true
    //Auto-sized widths

    implicitWidth: 40

    implicitHeight: {
        if (defaultSpacing.minimumRowHeight){
            defaultSpacing.minimumRowHeight
        } else {
            22
        }
    }

	// support copy&paste
	WGCopyable {
        id: copyableControl

        BWCopyable {
            id: copyableObject

            onDataCopied : {
                setValue( colourButton.colourVec )
            }

            onDataPasted : {
                colourButton.colourVec = data
            }
        }

        onSelectedChanged : {
            if(selected)
            {
                selectControl( copyableObject )
            }
            else
            {
                deselectControl( copyableObject )
            }
        }
    }

    onClicked: {
		if(defaultColorDialog)
		{
			colorDialog.visible = true
		}
    }

    //colour square over the top of the standard button frame.
    Rectangle {
        id: colorSquare
        anchors.fill: parent
        anchors.margins: defaultSpacing.rowSpacing

        opacity: enabled ? 1 : 0.4

		color: parent.color
    }

    ColorDialog {
        id: colorDialog
        title: "Choose a Color"
        visible: false
        onAccepted: {
			colorButton.color = colorDialog.color
        }

    }
}
