import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import BWControls 1.0

/*!
 \brief Button that has an variable image as an icon.
 Intended for textures etc.

\code{.js}
WGThumbnailButton {
    iconSource: "icons/file"
    defaultText: "Click to Load an Image"
}\endcode
*/

Button {
    id: thumbnailButton
    objectName: "WGThumbnailButton"

    /*! This property contains mouse over information string intended for WGToolTip, currently not working.
        The default value is an empty string */
    //TODO: Determine if valid approach for tooltips
    property string mouseOverInfo: ""

    /*! This property contains the default text string that will be shown when \c iconSource: is an empty string.
        The default value is \c "Default text has not been set"*/
    property string defaultText: "Default text has not been set"

    /*! This property is not used by anything */
    //TODO: remove this property it appears to be used by nothing
    property int radius: defaultSpacing.standardRadius

    /*! This property is used to define the buttons label when used in a WGFormLayout
        The default value is an empty string
    */
    //TODO: This should be renamed, it does not require "_"
    property string label_: ""

    /*! This property holds the url of the thumbnail displayed on the button
        The default value is an empty string
    */
    iconSource: ""

    property alias source: icon.source

    /*! This property holds the target control's id to be bound to this control's b_Value */
    property alias b_Target: dataBinding.target

    /*! This property determines b_Target's property which is to be bound to this control's b_Value */
    property alias b_Property: dataBinding.property

    /*! This property determines this control's value which will drive b_Target's b_Property */
    property alias b_Value: dataBinding.value


    Binding {
        id: dataBinding

    }

    // support copy&paste
    WGCopyable {
        id: copyableControl

        BWCopyable {
            id: copyableObject

            onDataCopied : {
                setValue( thumbnailButton.iconSource )
            }

            onDataPasted : {
                thumbnailButton.iconSource = data
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

    implicitWidth: {
        defaultSpacing.minimumRowHeight * 4
    }

    implicitHeight: {
        defaultSpacing.minimumRowHeight * 4
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
            anchors.margins: defaultSpacing.standardMargin
            opacity: enabled ? 1 : 0.4

            fillMode: Image.PreserveAspectFit

            Component.onCompleted: {
                if (icon.source == ""){
                    defaulttext1.visible = true
                }
            }
        }

        WGLabel{
            id: defaulttext1
            anchors.centerIn: parent
            width: (parent.width - (defaultSpacing.leftMargin + defaultSpacing.rightMargin))
            horizontalAlignment: "AlignHCenter"
            verticalAlignment: "AlignVCenter"
            text: defaultText
            visible: false
            wrapMode: "Wrap"
        }
    }

    FileDialog {
        id: fileDialog
        title: "Choose a texture"
        visible: false
        nameFilters: [ "Image files (*.jpg *.png)", "All files (*)" ]
        onAccepted: {
            icon.source = fileDialog.fileUrl
            defaulttext1.visible = false
        }
    }
}
