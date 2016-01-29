import QtQuick 2.3
import QtQuick.Layouts 1.1


/*!
 \brief A grid layout that left aligns all children on to a single line,
 or two columns if they don't fit.
 Intended for radio buttons and checkboxes with different widths.
 Best with 5+ controls otherwise use WGColumnLayout or WGExpandingRowLayout

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

Grid {
    id: gridFrame
    objectName: "WGBoolGridLayout"

    /*! This property is used to define the buttons label when used in a WGFormLayout
        The default value is an empty string
    */
    //TODO: This should be renamed, it does not require "_"
    property string label_: ""

    /*! This property is holds the total width of the child controls */
    //TODO: This should be an internal control and should be marked as private by "__" prefix
    property int totalWidth_: 0

    /*! This function calculates the width of all the child objects */
    function getWidth(){
        var width = 0;
        for (var i=0; i<children.length - 1; i++)
        {
            width += children[i].implicitWidth;
        }
        return width;
    }

    flow: GridLayout.TopToBottom
    rowSpacing: defaultSpacing.rowSpacing
    columnSpacing: defaultSpacing.doubleMargin
    anchors {left: parent.left; right: parent.right}

    Component.onCompleted: {

        //set columns to 2 if too wide or unlimited otherwise
        totalWidth_ = getWidth();
        if (totalWidth_ > gridFrame.width)
        {
            gridFrame.columns = 2
            gridFrame.rows = -1
        }
        else
        {
            gridFrame.columns = -1
            gridFrame.rows = 1
        }
    }

    //Handle the window being resized
    onWidthChanged: {
        totalWidth_ = getWidth();
        if (totalWidth_ > gridFrame.width)
        {
            gridFrame.columns = 2
            gridFrame.rows = -1
        }
        else
        {
            gridFrame.columns = -1
            gridFrame.rows = 1
        }
    }
}
