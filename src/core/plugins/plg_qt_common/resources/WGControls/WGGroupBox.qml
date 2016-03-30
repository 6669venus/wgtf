import QtQuick 2.3

/*!
    \brief Panel with thin separators top and bottom.
    Good for toggleable_ panels that enable/disable their contents.
    Consider using WGFrame for simple panels to group objects that don't need a title or to expand or toggle.
    Not intended to be used as a collapsible panel.

Example:
\code{.js}
WGGroupBox {
    text: "Group Box"
    toggleable : true
    childObject :
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
}
\endcode
*/

WGSubPanel{
    objectName: "WGGroupBox"
    collapsible: false
    transparentPanel: true
    contentLeftMargin: 0
    contentRightMargin: 0
    boldHeader: false
    hasIcon: false
    hasSeparators: true
}
