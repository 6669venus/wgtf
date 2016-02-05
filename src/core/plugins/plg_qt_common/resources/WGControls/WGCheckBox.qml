import QtQuick 2.3
import QtQuick.Controls 1.2
import BWControls 1.0

/*!
 \brief A WG styled control that allows the user to make a binary choice.

Example:
\code{.js}
WGCheckBox {
    label_: "Option"
    text: "Save Automatically?"
}
\endcode
*/

CheckBox {
    id: checkBox
    objectName: "WGCheckBox"

    /*! This property is used to define the buttons label when used in a WGFormLayout
        The default value is an empty string
    */
    //TODO: This should be renamed, it does not require "_"
    property string label_: ""

    /*! This property determines the checked state of the control
        The default value is false
    */
    property bool checked: false

    checkedState: Qt.Unchecked

    activeFocusOnTab: enabled

    implicitHeight: defaultSpacing.minimumRowHeight ? defaultSpacing.minimumRowHeight : 22

    /*! This property holds the target control's id to be bound to this control's b_Value */
    property alias b_Target: dataBinding.target

    /*! This property determines b_Target's property which is to be bound to this control's b_Value */
    property alias b_Property: dataBinding.property

    /*! This property determines this control's value which will drive b_Target's b_Property */
    property alias b_Value: dataBinding.value

    onCheckedStateChanged: {
        if (checkedState === Qt.PartiallyChecked) {
            partiallyCheckedEnabled = true;
            setValueHelper( checkBox, "checked", false);
        } else {
            setValueHelper( checkBox, "checked", checkedState === Qt.Checked);
        }
    }

    onCheckedChanged: {
        if (!partiallyCheckedEnabled)
        {
            checkedState = checked ? Qt.Checked : Qt.Unchecked
        }
        else if (checked)
        {
            checkedState = Qt.Checked
        }
    }

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
                setValueHelper( checkBox, "checked", data );
            }
        }

        onSelectedChanged : {
            if (selected)
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
