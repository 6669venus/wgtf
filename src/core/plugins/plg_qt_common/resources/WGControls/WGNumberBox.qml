import QtQuick 2.3
import QtQuick.Controls 1.2

//Text entry intended for numbers.

WGSpinBox {
    id: spinBox

    activeFocusOnTab: enabled

    implicitHeight: {
        if (defaultSpacing.minimumRowHeight){
            defaultSpacing.minimumRowHeight
        } else {
            22
        }
    }

}
