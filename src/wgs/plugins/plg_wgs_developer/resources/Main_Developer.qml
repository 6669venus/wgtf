import QtQuick 2.3
import QtQuick.Window 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

//setting up the application window
Rectangle{
    id: app_window
   
    //setting up the header
    Header{
        id: header
    }

    //setting up the content area of the app
    Rectangle{
        id: content
        anchors.top: header.bottom
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.bottom: parent.bottom

        states: [
            State {
                name: "settings"
                PropertyChanges{ target: tabs; visible: false }
                PropertyChanges{ target: settings; visible: true }
            }
        ]

        //setting up the tabview
        TabView{
            id: tabs
            anchors.fill: parent
            visible: true

            //styling the tabs
            style: TabViewStyle{
                tabsAlignment: Qt.AlignHCenter
                tabOverlap: -30
                frameOverlap: 0
                tabBar: Rectangle{
                    color: "white"
                }
                tab: Rectangle{
                    Image {
                        id: bg_tab_selected
                        source: "images/bg_tab_selected.png"
                        anchors.fill: parent
                        visible: styleData.selected ? true : false
                    }
                    color: styleData.selected ? "white" :"white"
                    implicitWidth: Math.max(text.width)
                    implicitHeight: 40

                    Text {
                        id: text
                        anchors.centerIn: parent
                        text: styleData.title
                        //font.bold: true
                        font.pointSize: 12
                        color: styleData.selected ? "#4d4d4f" : "#939598"
                    }
                }
            }

            //setting up the Home tab
            Tab{
                title: "Home"

                Tab_Home{
                }
            }

            //setting up the What's New tab
            Tab{
                title: "News"

                Rectangle{
                }
            }

            //setting up the Updates tab
            Tab{
                title: "Updates"

                Tab_Updates{
                }
            }

            //setting up the Streams tab
            Tab{
                title: "Streams"

                Rectangle{
                    color: "white"
                }
            }
        }

        //setting up the settings page
        Settings{
            id: settings
            visible: false
            anchors.fill: parent
        }
    }
}
