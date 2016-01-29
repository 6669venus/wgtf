import QtQuick 2.3

/*!
 \brief Panel intended to be placed inside a regular SubPanel.
 Consider using WGFrame for simple panels to group objects that don't need a title or to expand.

\code{.js}
WGSubPanel {
    text: "Sub Panel"
    childObject_ :
        WGColumnLayout {

            WGInternalPanel {
                text: "Scrolling Panel of Buttons"
                clipContents_: true
                expanded_: false
                childObject_ :
                    WGSubScrollPanel {
                        implicitHeight: 100
                        childObject_:
                            WGFormLayout {
                            localForm_: true
                                WGLabel{
                                    text: "Internal scrollbars possible, but not recommended"
                                }

                                Repeater {
                                    model: 10
                                    WGPushButton {
                                        text: "Button"
                                        label_: "Repeated Label: "
                                    }
                                }
                            }
                }
            }
        }
}
\endcode
*/

WGSubPanel{
    objectName: "WGInternalPanel"
    colorHeader_: palette.MainWindowColor
    colorBody_: palette.MainWindowColor
}
