import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

//the settings view of the app

Item{
    //color: "white"
    height: 700
    width: 440

    //scrollview wraps the listview and provides a scroll bar as needed
//    ScrollView{
//        //anchors.fill: parent
//        width: parent.width
//        anchors.top: parent.top
//        anchors.bottom: settings_footer.top

//        //styling the scrollview
//        style: ScrollViewStyle {
//            transientScrollBars: false
//            handle: Item {
//                implicitWidth: 14

//                Rectangle {
//                    color: styleData.pressed ? "#4d4d4f" : "#7c7d80"
//                    anchors.fill: parent
//                }
//            }
//            handleOverlap: 0
//            incrementControl: Item{
//                implicitWidth: 14
//                implicitHeight: 16

//                Rectangle{
//                    color: styleData.pressed ? "#4d4d4f" : "#ebedf2"
//                    anchors.fill: parent

//                    Image {
//                        source: "images/icon_arrow_down.png"
//                        height: 5
//                        width: 8
//                        anchors.verticalCenter: parent.verticalCenter
//                        anchors.verticalCenterOffset: 2
//                        anchors.horizontalCenter: parent.horizontalCenter
//                    }
//                }
//            }
//            decrementControl: Item{
//                implicitWidth: 14
//                implicitHeight: 16

//                Rectangle{
//                    color: styleData.pressed ? "#4d4d4f" : "#ebedf2"
//                    anchors.fill: parent

//                    Image {
//                        source: "images/icon_arrow_up.png"
//                        height: 5
//                        width: 8
//                        anchors.verticalCenter: parent.verticalCenter
//                        anchors.verticalCenterOffset: 1
//                        anchors.horizontalCenter: parent.horizontalCenter
//                    }
//                }
//            }
//            scrollBarBackground: Item {
//                implicitWidth: 13

//                Rectangle{
//                    color: "#ebedf2"
//                    anchors.fill: parent
//                }
//            }
//        }

//        //setting up the app list view
//        ListView{
//            id: app_list_view
//            anchors.topMargin: 8
//            anchors.bottomMargin: 8
//            model: 15

//            //setting up the app list view delegate
//            delegate: Delegate_Apps{
//                width: parent.width
//            }
//        }
//    }

    Rectangle{
        id: settings_footer
        height: 45
        width: parent.width
        anchors.bottom: parent.bottom
        color: "#ebedf2"

        Separator{
            anchors.top: parent.top
        }

        Button_Simple{
            id: button_cancel
            label: "Cancel"
            anchors.right: parent.right
            anchors.rightMargin: 8
            anchors.verticalCenter: parent.verticalCenter
            onButtonClick: content.state = ""
        }

        Button_Simple{
            id: button_ok
            label: "OK"
            anchors.right: button_cancel.left
            anchors.rightMargin: 8
            anchors.verticalCenter: parent.verticalCenter
            onButtonClick: content.state = ""
        }
    }
}
