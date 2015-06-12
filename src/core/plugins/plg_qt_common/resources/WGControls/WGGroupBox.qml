import QtQuick 2.3

//Panel with thin separators top and bottom. Good for toggleable_ panels that enable/disable their contents.
//Consider using WGFrame for simple panels to group objects that don't need a title or to expand or toggle.
//Not indended as a collapsible UI components.

WGSubPanel{
    collapsible_: false
    transparentPanel_: true
    contentLeftMargin_: 0
    contentRightMargin_: 0
    boldHeader_: false
    hasIcon_: false
    hasSeparators_: true
}
