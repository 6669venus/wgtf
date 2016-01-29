import QtQuick 2.3
import QtQuick.Controls 1.2

/*!
 \brief A WG styled radio button control, typically for selecting one of a set of predefined options.


Example:
\code{.js}
WGBoolGridLayout {
    ExclusiveGroup { id: radioPanelGroup }
    WGRadioButton {
        text: "Radio Button"
        exclusiveGroup: radioPanelGroup
        checked: true
    }
    WGRadioButton {
        text: "Radio Button"
        exclusiveGroup: radioPanelGroup
    }
    WGRadioButton {
        text: "Radio Button with a long name"
        exclusiveGroup: radioPanelGroup
    }
}
\endcode
*/

RadioButton {
    objectName: "WGRadioButton"

    /*! This property is used to define the buttons label when used in a WGFormLayout
        The default value is an empty string
    */
    //TODO: This should be renamed, it does not require "_"
    property string label_: ""

    activeFocusOnTab: enabled

    implicitHeight: defaultSpacing.minimumRowHeight ? defaultSpacing.minimumRowHeight : 22

    /*! This property holds the target control's id to be bound to this controls b_Value */
    property alias b_Target: dataBinding.target

    /*! This property determines b_Target's property which is to be bound to this controls b_Value */
    property alias b_Property: dataBinding.property

    /*! This property determines this control's value which will drive b_Target's b_Property */
    property alias b_Value: dataBinding.value

    Binding {
        id: dataBinding
    }

    style: WGRadioStyle {

    }
}
