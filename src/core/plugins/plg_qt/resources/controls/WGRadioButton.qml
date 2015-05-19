import QtQuick 2.0
import QtQuick.Controls 1.0

RadioButton {

    property string label_: ""

    activeFocusOnTab: enabled

    implicitHeight: {
        if (panelProps.rowHeight_){
            panelProps.rowHeight_
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


    style: WGRadioStyle {

    }

}
