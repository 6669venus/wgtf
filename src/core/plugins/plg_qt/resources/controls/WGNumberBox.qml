import QtQuick 2.0
import QtQuick.Controls 1.1

//Text entry intended for numbers.

WGSpinBox {
    id: spinBox

    activeFocusOnTab: enabled

    implicitHeight: {
        if (panelProps.rowHeight_){
            panelProps.rowHeight_
        } else {
            22
        }
    }

}
