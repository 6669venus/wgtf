import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls 1.0

/*!
    \brief Timeline Panel

*/
WGPanel {
    id: timelinePanel
    objectName: "TimelinePanel"

    property var title: qsTr( "Timeline Panel" )

    /*! Number of clicks.
    */
    property int clickCount: 0

    color: palette.MainWindowColor

    WGExpandingRowLayout {
        id: layoutHello

        anchors.fill: parent

        WGLabel {
            id: labelHello
            text: "Clicks " + clickCount
        }

        WGPushButton {
            id: buttonHello
            text: "Click here"
            Layout.fillWidth: true
            Layout.fillHeight: true
            onClicked: {
                clickCount += 1
            }
        }
    }
}
