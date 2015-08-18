import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

/*!
 \brief A non-editable single line of text that can align to a panel wide width in defaultSpacing
 Will appear in the left column if placed in a WGFormLayout && formLabel_ == true

\qml
WGLabel {
    text: "Example text"
}
\endqml
*/

Text {
    id: labelText
    objectName: "WGLabel"

    /*! This property right aligns the label and sets width to the largest label in the panel.
        The default value is false
    */
    property bool formLabel_: false

    /*! This property ignores the panel wide label column width
        The default is false
    */
    property bool localForm_: false

    /*! property only for the copy/paste prototype
        The default value is null
    */

    //TODO If this is used exclusively for copy/paste prototype is it still required?
    property QtObject formControlCopyable_: null

    color: {
        if (enabled){
            palette.TextColor
        } else {
            palette.DisabledTextColor
        }
    }

    renderType: Text.NativeRendering

    horizontalAlignment: formLabel_ ? Text.AlignRight : Text.AlignLeft

    /*!
        Links the label to it's control object and then finds the copyable inside it.
        Only works with form labels.
        @param type:object parentObject The parent control object
    */
    function selectLabelControl(parentObject){
        for(var i=0; i<parentObject.children.length; i++){
            if(parentObject.children[i].label_ == labelText.text){
                selectControlCopyable(parentObject.children[i])
                break;
            } else {
                selectLabelControl(parentObject.children[i])
            }
        }
    }

    /*!
        TODO document this
    */
    function selectControlCopyable(parentObject){
        for(var i=0; i<parentObject.children.length; i++){
            if(typeof parentObject.children[i].rootCopyable != "undefined"){
                formControlCopyable_ = parentObject.children[i]
            }
        }
    }

    width: {
        if (formLabel_ && !localForm_){
            defaultSpacing.labelColumnWidth
        } else {
            implicitWidth
        }
    }

    Layout.preferredWidth: {
        if (formLabel_ && !localForm_){
            defaultSpacing.labelColumnWidth
        } else {
            implicitWidth
        }
    }

    Component.onCompleted: {

        if (formLabel_ && paintedWidth > defaultSpacing.labelColumnWidth && !localForm_){
            defaultSpacing.labelColumnWidth = paintedWidth;
        }

        if(formLabel_){
            selectLabelControl(labelText.parent)
        }
    }

    MouseArea {
        anchors.fill: parent
        enabled: labelText.formLabel_
        hoverEnabled: labelText.formLabel_
        cursorShape: labelText.formLabel_ ? Qt.PointingHandCursor : Qt.ArrowCursor

        onClicked:{
            if((formControlCopyable_ === null) || (!formControlCopyable_.enabled))
            {
                return;
            }
            if(!globalSettings.wgCopyableEnabled)
            {
                return;
            }

            if ((mouse.button == Qt.LeftButton) && (mouse.modifiers & Qt.ControlModifier)){
                if(formControlCopyable_.selected){
                    formControlCopyable_.deSelect()
                } else {
                    formControlCopyable_.select()
                }
            } else if (mouse.button == Qt.LeftButton){
                formControlCopyable_.rootCopyable.deSelect();
                formControlCopyable_.select()
            }
        }
    }
}

