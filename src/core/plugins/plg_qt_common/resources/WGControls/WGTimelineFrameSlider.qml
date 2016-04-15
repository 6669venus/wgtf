import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Private 1.0
import QtQuick.Layouts 1.1
import WGControls 2.0

/*!
 \brief Slider with a multiple handles that should really only be used in the Timeline Control

\code{.js}

**** TODO **** {
    Layout.fillWidth: true
    minimumValue: 0
    maximumValue: 100
    lowerValue: 25
    upperValue: 75
    stepSize: 1
    enabled: true
}

\endcode
*/


WGSlider {
    objectName: "slider"
    id: slider

    activeFocusOnPress: true

    style: WGTimelineFrameSliderStyle{}

    // initial handle values to apply deltas to
    property var initialValues: []

    property bool showLabel: false

    grooveClickable: false

    handleClamp: false

    clickAreaBuffer: 50

    allowMouseWheel: false

    // tell the view a drag has started
    onBeginDrag: {
        __handlePosList[index].handleDragging = true
        view.itemDragging = true
    }

    onEndDrag: {
        __handlePosList[index].handleDragging = false
        view.itemDragging = false
        initialValues = []
    }

    Connections {
        target: view
        // if anything in the view starts dragging, populate the initial values
        onItemDraggingChanged: {
            if (view.itemDragging)
            {
                for (var i = 0; i < __handlePosList.length; i++)
                {
                    initialValues[i] = __handlePosList[i].value
                }
            }
        }
    }
}
