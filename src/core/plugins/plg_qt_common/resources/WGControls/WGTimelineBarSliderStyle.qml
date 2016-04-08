import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Private 1.0
import QtQuick.Layouts 1.1

/*!
    A slider style that contains a color gradient as the background
*/
WGSliderStyle {
    id: sliderStyle
    objectName: "WGColorSliderStyle"

    groove: Item {

        anchors.verticalCenter: parent.verticalCenter

        //changing between odd and even values causes pixel 'wiggling' as the center anchors move around.
        //can't use anchors.fill because the gradients need rotating
        implicitHeight: __horizontal ? control.height : control.width
        implicitWidth: __horizontal ? control.width : control.height
    }
    // There is no expanding bar that follows the handles in a ColorSlider
    bar: Item {
        Loader
        {
            sourceComponent: control.barContent
            clip: true

            anchors.fill: parent
            anchors.leftMargin: -1
            anchors.rightMargin: -1
        }
    }
}
