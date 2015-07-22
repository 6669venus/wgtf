import QtQuick 2.3
import QtQuick.Controls 1.2

RadioButton {

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


    style: WGRadioStyle {

    }

}
