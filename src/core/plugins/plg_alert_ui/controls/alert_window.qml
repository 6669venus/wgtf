import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import QtQuick.Window 2.1
import WGControls 1.0

Rectangle {
	property var title: "Alerts"

    id: main

	color: palette.MainWindowColor

    property QtObject panelProps: WGPanelProperties{}

    /// Remove the alert message at the index passed in
    /// See alert_models.cpp ( AlertPageModel::removeAlert )
    function removeAlertAt( index ){
        currentSelectedRowIndex = index;
        removeAlert;
    }

	WGListModel {
		id : alertModel
		source : alerts

		ValueExtension {}
	}

    Component {
        id: alertDelegate

        Item {
            width: main.width
            height: 50

            Row {
                Rectangle {
                    property int itemIndex: index

                    width: main.width
                    height: 50
                    border.width: 1
                    border.color: palette.DarkestShade
                    color: palette.LightShade

                    // Remove alert when the timer is triggered
                    Timer {
                        id: removeAlertTimer
                        interval: 10000
                        running: true
                        onTriggered: {
                            removeAlertAt( index )
                        }
                    }

                    WGLabel {
                        text: Value.message
                        anchors.fill: parent
                    }

                    // Remove the alert on mouse click
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            removeAlertAt( index )
                        }
                    }
                }
            }
        }
    }

    WGScrollPanel {
        childObject_ :
        WGExpandingRowLayout {
            Layout.fillWidth: true

            WGListView {
                Layout.fillWidth: true
                model: alertModel
                interactive: true
                height: 700

                delegate: alertDelegate
            }
        }
    }
}
