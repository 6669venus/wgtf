import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2

//Button that has an variable image as an icon. Intended for textures etc.

Button {
    // Assign a iconSource or defaultText for the empty thumnbnailbutton
    property string mouseOverInfo: ""
    property string defaultText: "Default text has not been set"
    property int radius_: panelProps.standardRadius_    

    property string label_: ""

    id: thumbnailButton
    //iconSource: "qrc:///icons/defaultImage.png";
    iconSource: ""

    property alias b_Target: dataBinding.target
    property alias b_Property: dataBinding.property
    property alias b_Value: dataBinding.value

    Binding {
        id: dataBinding

    }

    implicitWidth: {
        panelProps.rowHeight_ * 4
    }

    implicitHeight: {
        panelProps.rowHeight_ * 4
    }

    onClicked: {
        if (enabled == true){
            fileDialog.visible = true
        }
    }

    style: WGButtonStyle{

    }

    Item {
        anchors.fill: parent
        Image {
            id: icon
            anchors.fill: parent
            anchors.margins: panelProps.standardMargin_
            //source: thumbnailButton.iconSource
            source: iconSource
            opacity: enabled ? 1 : 0.4

            Component.onCompleted: {
                if (icon.source == ""){
                    defaulttext1.visible = true
                }
                else{
                    thumbnailButton.iconSource
                }
            }
        }

        WGLabel{
            id: defaulttext1
            anchors.centerIn: parent
            width: (parent.width - (panelProps.leftMargin_ + panelProps.rightMargin_))
            horizontalAlignment: "AlignHCenter"
            verticalAlignment: "AlignVCenter"
            text: defaultText
            visible: false
            wrapMode: "Wrap"
        }

        // NOT WORKING, SEE WGToolTipMouseArea for explanation
        /*WGToolTipMouseArea{
            // passing mouse over information through WGToolTipMouseArea to WGTooltip
            parentControl: thumbnailButton
            parentControlsMouseOverInfo: thumbnailButton.mouseOverInfo
            //Special case for thumbnail button is passing the icon source from this Control. Usually this would be passed from main.
            parentControlsMouseOverImage: thumbnailButton.iconSource
            onClicked: {
                fileDialog.visible = true
            }            
        }*/
    }

    FileDialog {
        id: fileDialog
        title: "Choose a texture"
        visible: false
        nameFilters: [ "Image files (*.jpg *.png)", "All files (*)" ]
        onAccepted: {
            icon.source = fileDialog.fileUrl
            thumbnailButton.iconSource = icon.source
            defaulttext1.visible = false
        }
    }
}
