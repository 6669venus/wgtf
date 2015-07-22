import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Rectangle{
    id: tab_updates
    color: "white"
    width: 440
    height: 760

    Separator{
        id: separator_top
    }

    Item{
        id: updates_container
        anchors.top: separator_top.bottom
        width: parent.width
        height: updates.height + updates_list_container.height
        opacity: 1


        //Setting up the updates greybar
        Rectangle{
            id: updates
            color: "#ebedf2"
            height: 35
            width: parent.width
            anchors.top: updates_container.top

            Text {
                id: label
                text: "UPDATES (3)"
                anchors.left: parent.left
                anchors.leftMargin: 24
                anchors.verticalCenter: parent.verticalCenter
                color: "#939598"
            }

            Text {
                id: label_update_all
                text: "Update All"
                anchors.right: parent.right
                anchors.rightMargin: 24
                anchors.verticalCenter: parent.verticalCenter
                color: "red" //"#939598"
                font.underline: label_update_all_mouse_area.containsMouse ? true : false

                MouseArea{
                    id: label_update_all_mouse_area
                    anchors.fill: parent
                    hoverEnabled: true
                }
            }
        }

        //Setting up the updates list view
        Item{
            id:updates_list_container
            height: updates_list_view.contentHeight + 24
            anchors.top: updates.bottom
            anchors.topMargin: 8
            anchors.left: parent.left
            anchors.right: parent.right

            ListView{
                id: updates_list_view
                anchors.fill: parent
                interactive: false
                model: 3

                //setting up the updates list view delegate
                delegate: Delegate_Updates{
                }
            }
        }
    }



    //Setting up the tools greybar
    Rectangle{
        id: tools
        color: "#ebedf2"
        height: 35
        width: parent.width
        anchors.top: updates_container.bottom

        Text {
            id: label_tools
            text: "TOOLS (5)"
            anchors.left: parent.left
            anchors.leftMargin: 24
            anchors.verticalCenter: parent.verticalCenter
            color: "#939598"
        }

    }

    //Setting up the tools list view
    Item{
        id:tools_list_container
        height: tools_list_view.contentHeight + 24
        anchors.top: tools.bottom
        anchors.topMargin: 8
        anchors.left: parent.left
        anchors.right: parent.right

        ListView{
            id: tools_list_view
            anchors.fill: parent
            interactive: false
            model: 5

            //setting up the updates list view delegate
            delegate: Delegate_Tools{
            }
        }
    }
}
