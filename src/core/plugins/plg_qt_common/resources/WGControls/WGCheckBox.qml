import QtQuick 2.3
import QtQuick.Controls 1.2
import BWControls 1.0

CheckBox {
	id: checkBox
    property string label_: ""

    activeFocusOnTab: enabled

    implicitHeight: {
        if (defaultSpacing.minimumRowHeight){
            defaultSpacing.minimumRowHeight
        } else {
            22
        }
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
				setValue( checkBox.checked )
			}

			onDataPasted : {
				checkBox.checked = data
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
