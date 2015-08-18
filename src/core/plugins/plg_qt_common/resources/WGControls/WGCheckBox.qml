import QtQuick 2.3
import QtQuick.Controls 1.2
import BWControls 1.0

CheckBox {
	id: checkBox
    property string label_: ""
	property bool checkState: false
    activeFocusOnTab: enabled
	activeFocusOnPress: true

    implicitHeight: {
        if (defaultSpacing.minimumRowHeight){
            defaultSpacing.minimumRowHeight
        } else {
            22
        }
    }

	onClicked: {
		setValueHelper( checkBox, "checkState", (checkedState === Qt.Checked) ? true : false );
	}
	onCheckStateChanged: {
		checked = checkState ? true : false;
	}
    property alias b_Target: dataBinding.target
    property alias b_Property: dataBinding.property
	property alias b_Value: dataBinding.value

    Binding {
		id: dataBinding

    }

	// support copy&paste
	WGCopyable {
		id: copyableControl

		BWCopyable {
			id: copyableObject

			onDataCopied : {
				setValue( checkBox.checkState )
			}

			onDataPasted : {
				setValueHelper( checkBox, "checkState", data );
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

    //Fix to stop half pixel offsets making the checkmarks look off centre
    Component.onCompleted:{
        implicitWidth = (Math.round(implicitWidth))
    }

    style: WGCheckStyle {

    }

}
