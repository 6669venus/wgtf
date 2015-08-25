import QtQuick 2.3
import QtQuick.Layouts 1.1
import BWControls 1.0

//A visual, non-interactive analog bar that changes colour based on the value.

WGExpandingRowLayout {
    id: mainFrame

    property int valueBoxWidth_: valueBox.implicitWidth

    property string prefix_: ""
    property string suffix_: "%"

    property bool showValue_: true
    property int decimals_: 0
    property real minimumValue_: 0
    property real maximumValue_: 100

    property real stepSize_: 1

    property var ranges_: [50,75,100]
    property var colors_: ["#7ac943", "#ff931e", "#e23627"]

    property int rangeIndex_: 0

    property real unitWidth_: (barFrame.width - defaultSpacing.doubleBorderSize) / (maximumValue_ - minimumValue_)

    property string label_: ""

    property alias b_Target: dataBinding.target
    property alias b_Property: dataBinding.property
    property alias b_Value: dataBinding.value

    property real value_: 0

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

    function checkColor(){
        for (var i = 0; i < mainFrame.ranges_.length; i++){
            if(value_ <= mainFrame.ranges_[i]){
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
        if (loaded_){
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

        noArrows_: true
        readOnly: true

        minimumValue: mainFrame.minimumValue_
        maximumValue: mainFrame.maximumValue_
        stepSize: mainFrame.stepSize_
    }
}

