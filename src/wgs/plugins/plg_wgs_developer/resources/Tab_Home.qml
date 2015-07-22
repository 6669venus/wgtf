import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Rectangle{
    id: tab_home
    color: "white"
    width: 440
    height: 760

    Dashboard{
        id: dash
    }

    Rectangle{
        anchors.top: dash.bottom
        anchors.bottom: footer.top
        anchors.left: parent.left
        anchors.right: parent.right

        //scrollview wraps the listview and provides a scroll bar as needed
        ScrollView{
            anchors.fill: parent

            //styling the scrollview
            style: ScrollViewStyle {
                transientScrollBars: false
                handle: Item {
                    implicitWidth: 14

                    Rectangle {
                        color: styleData.pressed ? "#4d4d4f" : "#7c7d80"
                        anchors.fill: parent
                    }
                }
                handleOverlap: 0
                incrementControl: Item{
                    implicitWidth: 14
                    implicitHeight: 16

                    Rectangle{
                        color: styleData.pressed ? "#4d4d4f" : "#ebedf2"
                        anchors.fill: parent

                        Image {
                            source: "images/icon_arrow_down.png"
                            height: 5
                            width: 8
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.verticalCenterOffset: 2
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                    }
                }
                decrementControl: Item{
                    implicitWidth: 14
                    implicitHeight: 16

                    Rectangle{
                        color: styleData.pressed ? "#4d4d4f" : "#ebedf2"
                        anchors.fill: parent

                        Image {
                            source: "images/icon_arrow_up.png"
                            height: 5
                            width: 8
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.verticalCenterOffset: 1
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                    }
                }
                scrollBarBackground: Item {
                    implicitWidth: 13

                    Rectangle{
                        color: "#ebedf2"
                        anchors.fill: parent
                    }
                }
            }

            //setting up the app list view
            ListView{
                id: app_list_view
                anchors.topMargin: 8
                anchors.bottomMargin: 8
                model: 5

                //setting up the app list view delegate
                delegate: Delegate_Apps{
                    width: parent.width
                }
            }
        }
    }

    Rectangle{
        id: footer
        color: "#ebedf2"
        height: 45
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        Separator{
            id: separator_top
            anchors.top: parent.top
        }

        Rectangle{
            id: search_bar
            color: "White"
            height: 30
            anchors.left: parent.left
            anchors.right: icon_update_pc.left
            anchors.verticalCenter: parent.verticalCenter
            anchors.rightMargin: 8
            anchors.leftMargin: 8
            radius: 2.5
            border.color: "#adadb2"
            border.width: 1

            Image {
                id: icon_magnifying_glass
                //switch to icon_clear_text if input text is present...
                source: {
                    if (search_text_field.text == "")
                        return "images/icon_magnifying_glass.png"
                    else
                        return "images/icon_clear_text.png"
                }
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: 8

                MouseArea{
                    id: icon_clear_text
                    anchors.fill: parent
                    //on click, clear the text field...
                    onClicked: search_text_field.text = ""
                }
            }

            TextField{
                id: search_text_field
                placeholderText: "Search programs and files"
                anchors.right: icon_magnifying_glass.left
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.margins: 4

                //styling the text field
                style: TextFieldStyle{
                    background: Rectangle{
                        color: "white"
                    }
                }
            }
        }

        Image {
            id: icon_launch_game
            source: "images/icon_launch_game.png"
            anchors.right: parent.right
            anchors.rightMargin: 8
            anchors.verticalCenter: parent.verticalCenter

            MouseArea{
                id: lauch_game_mouse_area
                anchors.fill: parent
            }
        }

        Image {
            id: icon_update_pc
            source: "images/icon_update_pc.png"
            anchors.right: icon_launch_game.left
            anchors.rightMargin: 4
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
