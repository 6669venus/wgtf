import QtQuick 2.3
import QtQuick.Controls 1.2

//Text entry intended for numbers.

WGSpinBox {
    id: spinBox

    activeFocusOnTab: enabled
	property real number: 0

	onEditingFinished: {
		setValueHelper( spinBox, "number", value );
	}
	onNumberChanged: {
		value = number;
	}

    implicitHeight: {
        if (defaultSpacing.minimumRowHeight){
            defaultSpacing.minimumRowHeight
        } else {
            22
        }
    }

}
