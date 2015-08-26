import QtQuick 2.3

/*!
 \brief A scrollable sub panel to be used within controls like WGSubPanel that allow anchoring to top
 but bind to the size of their contents.
 Recommend NOT using scrollpanels within scrollpanels where possible.

Example:
\code{.js}
WGSubScrollPanel {
    implicitHeight: 100
    childObject_:
        WGFormLayout {
        localForm_: true
            WGLabel{
                text: "Some text"
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
\endcode
*/

WGScrollPanel {
    objectName: "WGSubScrollPanel"
    anchors.bottom: undefined
}
