import QtQuick 2.3

//Use within controls like WGColumnLayout that do not allow binding to top and bottom

//Recommend NOT using scrollpanels within scrollpanels where possible.

WGScrollPanel {
    anchors.bottom: undefined
    anchors.top: undefined
}
