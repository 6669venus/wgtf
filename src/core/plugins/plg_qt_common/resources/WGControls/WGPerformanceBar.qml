import QtQuick 2.3
import QtQuick.Layouts 1.1
import BWControls 1.0

/*!
 \brief A visual, non-interactive analog bar that changes colour based on the value.

Example:
\code{.js}
WGExpandingRowLayout {
    WGNumberBox {
        width: 120
        value: 25
        minimumValue: 0
        maximumValue: 100

        b_Target: perfBar3
        b_Property: "value_"
        b_Value: value
    }

    WGPerformanceBar {
        id: perfBar3
    }
}
\endcode
*/

WGExpandingRowLayout {
    id: mainFrame
    objectName: "WGPerformanceBar"

    /*! This property sets the implicitWidth of the text box that displays the percentage value
        The default value is derived from WGNumberbox
    */
    property int valueBoxWidth_: valueBox.implicitWidth

    /*! This property sets the prefix string used in the textbox of the performance bar.
        The default value is an empty string
    */
    property string prefix_: ""

    /*! This property sets the suffix string used in the textbox of the performance bar.
        The default value is \c %
    */
    property string suffix_: "%"

    /*! This property toggles the visibility of the text box displayed in this control
        The default value is \c true
    */
    property bool showValue_: true

    /*! This property defines the number of decimal places displayed in the textbox
        The default value is \c 0
    */
    property int decimals_: 0

    /*! This property sets the minimum value or start point of the performance bar
        The default value is \c 0
    */
    property real minimumValue_: 0

    /*! This property sets the maximum value or end point of the performance bar
        The default value is \c 100
    */
    property real maximumValue_: 100

    //TODO: This appears to have no affect.
    /*! This property sets the incremental step size of the performance bar
        The default value is \c 1
    */
    property real stepSize_: 1

    /*! This property sets the points at which the performance bar transitions between colours
        The default value is \c [50,75,100]
    */
    property var ranges_: [50,75,100]

    /*! This property sets the colour transitions used by the performance bar
        The default value is \c ["#7ac943", "#ff931e", "#e23627"]
    */
    property var colors_: ["#7ac943", "#ff931e", "#e23627"]

    //TODO: This should be renamed and marked as internal by "__" prefix
    /*! \internal */
    property int rangeIndex_: 0 //holds current position in ranges_

    //TODO: This should be renamed and marked as internal by "__" prefix
    /*! \internal */
    property real unitWidth_: (barFrame.width - defaultSpacing.doubleBorderSize) / (maximumValue_ - minimumValue_)

    /*! This property is used to define the buttons label when used in a WGFormLayout
        The default value is an empty string
    */
    //TODO: This should be renamed, it does not require "_"
    property string label_: ""

    /*! This property holds the target control's id to be bound to this controls b_Value */
    property alias b_Target: dataBinding.target

    /*! This property determines b_Target's property which is to be bound to this controls b_Value */
    property alias b_Property: dataBinding.property

    /*! This property determines this control's value which will drive b_Target's b_Property */
    property alias b_Value: dataBinding.value

    //TODO: This should be renamed and marked as internal by "__" prefix
    /*! \internal */
    property real value_: 0

    //TODO: This should be renamed and marked as internal by "__" prefix
    /*! \internal */
    property bool loaded_: false

    Binding {
        id: dataBinding
    }

    // support copy&paste
    WGCopyable {
        id: copyableControl

        BWCopyable {
            id: copyableObject

            onDataCopied : {
                setValue( mainFrame.value_ )
            }

            onDataPasted : {
                // readonly control
                console.log("ReadOnly Control " + label_);
                //mainFrame.value_ = data
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

    /*! \internal */
    function checkColor(){
        for (var i = 0; i < mainFrame.ranges_.length; i++)
        {
            if (value_ <= mainFrame.ranges_[i])
            {
                mainFrame.rangeIndex_ = i
                i = mainFrame.ranges_.length
            }
        }
    }

    Component.onCompleted: {
        loaded_ = true
        checkColor();
        copyableControl.disableChildrenCopyable( mainFrame );
    }

    //If the value_ is less than a ranges_ index, set the colour to the same index.
    onValue_Changed: {
        if (loaded_)
        {
            checkColor()
        }
    }

    WGTextBoxFrame{
        id: barFrame
        Layout.fillWidth: true
        Layout.preferredHeight: parent.height - defaultSpacing.doubleMargin

        Rectangle {
            id: bar
            radius: defaultSpacing.halfRadius
            height: parent.height - defaultSpacing.doubleBorderSize
            anchors.verticalCenter: parent.verticalCenter
            x: 1
            color: colors_[rangeIndex_]

            width: (value_ - mainFrame.minimumValue_) * unitWidth_
        }
    }

    WGNumberBox {
        id: valueBox
        visible: mainFrame.showValue_
        Layout.preferredWidth: visible ? valueBoxWidth_ : 0
        prefix: mainFrame.prefix_
        suffix: mainFrame.suffix_
        decimals: mainFrame.decimals_
        value: mainFrame.value_

        hasArrows: false
        readOnly: true

        minimumValue: mainFrame.minimumValue_
        maximumValue: mainFrame.maximumValue_
        stepSize: mainFrame.stepSize_
    }
}
