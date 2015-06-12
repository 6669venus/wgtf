import QtQuick 2.3

//Use within controls like WGSubPanel that allow anchoring to top but bind to the size of their contents

//Recommend NOT using scrollpanels within scrollpanels where possible.


WGScrollPanel {
    anchors.bottom: undefined
}
