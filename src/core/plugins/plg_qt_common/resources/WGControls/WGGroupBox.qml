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
    toggleable_ : true
    childObject_ :
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
    collapsible_: false
    transparentPanel_: true
    contentLeftMargin_: 0
    contentRightMargin_: 0
    boldHeader_: false
    hasIcon_: false
    hasSeparators_: true
}
