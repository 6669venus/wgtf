import QtQuick 2.0

Rectangle{
    id: app_delegate
    width: 440
    height: app_delegate_header.height + app_delegate_expanded_view.height

    Rectangle{
        id: app_delegate_header
        width: app_delegate.width
        height: 65
        color: "white"

        Image {
            id: app_icon
            source: "images/logo_WG.png"
            height: 48
            width: 48
            anchors.left: parent.left
            anchors.leftMargin: 16
            anchors.verticalCenter: parent.verticalCenter
        }

        Text {
            id: app_label
            text: "Despair Application"
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: app_icon.right
            anchors.leftMargin: 16
            color: "#4d4d4f"
            font.pointSize: 12
        }

        Image {
            id: app_delegate_expand
            source: "images/icon_arrow_forward.png"
            height: 12
            width: 8
            rotation: 0
            z: 1
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 24

            MouseArea{
                id: app_delegate_expand_mouse_area
                anchors.fill: parent
                anchors.margins: -10
                onClicked: if (app_delegate_expanded_view.height == 0)
                               app_delegate_expand.state = "expand_delegate"
                           else
                               app_delegate_expand.state = "collapse_delegate"
            }

            states: [
                State {
                    name: "expand_delegate"
                    PropertyChanges { target: app_delegate_expand; rotation: 90}
                    PropertyChanges { target: app_delegate_expanded_view; height: listview_recent_files.contentHeight + 10}
                    PropertyChanges { target: app_delegate_expanded_view; opacity: 1}
                },
                State {
                    name: "collapse_delegate"
                    PropertyChanges { target: app_delegate_expand; rotation: 0}
                    PropertyChanges { target: app_delegate_expanded_view; height: 0}
                    PropertyChanges { target: app_delegate_expanded_view; opacity: 0}
                }
            ]

            transitions: [
                Transition {
                    to: "expand_delegate"; reversible: false
                    RotationAnimation{ duration: 100; direction: RotationAnimation.Clockwise }
                    NumberAnimation{ target: app_delegate_expanded_view; property: "height"; duration: 100 }
                    NumberAnimation{ target: app_delegate_expanded_view; property: "opacity"; duration: 100 }
                },
                Transition {
                    to: "collapse_delegate"
                    RotationAnimation{ duration: 100; direction: RotationAnimation.Counterclockwise }
                    NumberAnimation{ target: app_delegate_expanded_view; property: "height"; duration: 100 }
                    NumberAnimation{ target: app_delegate_expanded_view; property: "opacity"; duration: 100 }
                }
            ]
        }

        MouseArea{
            id: app_delegate_mouse_area
            anchors.fill: parent
            hoverEnabled: true
            onEntered: [parent.color = "#adadb2", app_label.color = "#4d4d4f"]
            onExited: [parent.color = "white", app_label.color = "#4d4d4f"]
            onPressed: [parent.color = "#4d4d4f", app_label.color = "#adadb2"]
            onReleased: app_delegate_mouse_area.containsMouse ? [parent.color = "#adadb2", app_label.color = "#4d4d4f"] : [parent.color = "white", app_label.color = "#4d4d4f"]
        }
    }

    Rectangle{
        id: app_delegate_expanded_view
        gradient: Gradient {
            GradientStop {
                position: .5
                color: "white"
            }
            GradientStop {
                position: 1
                color: "#ebedf2"
            }
        }

        anchors.top: app_delegate_header.bottom
        width: app_delegate.width
        height: 0
        opacity: 0

        //setting up the recent files list view
        ListView{
            id: listview_recent_files
            anchors.fill: parent
            anchors.topMargin: 4
            anchors.leftMargin: 24
            anchors.rightMargin: 24
            interactive: false
            spacing: 2
            model: 5
            header: Component{
                id: header_recent

                Item{
                    height: 24
                    width: parent.width

                    Text {
                        id: text_recent
                        text: "Open Recent"
                        color: "#4d4d4f"
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    Separator{
                        anchors.left: text_recent.right
                        anchors.right: parent.right
                        anchors.leftMargin: 8
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.verticalCenterOffset: 2
                    }
                }
            }

            //setting up the delegate
            delegate: Rectangle{
                id: delegate_recent_files
                height: 24
                width: parent.width
                color: "transparent"
                radius: 2.5

                Image {
                    id: icon_recent_files
                    source: "images/logo_WG.png"
                    height: 16
                    width: 16
                    anchors.left: parent.left
                    anchors.leftMargin: 4
                    anchors.verticalCenter: parent.verticalCenter
                }

                Text {
                    id: text_recent_files
                    text: "//javelin/main/resources/.../.../.../.../.../.../.../.../.../.../.../.../recent_file.dsobject"
                    elide: Text.ElideLeft
                    color: "#4d4d4f"
                    //width: parent.width //- 28
                    anchors.right: parent.right
                    anchors.rightMargin: 4
                    anchors.left: icon_recent_files.right
                    anchors.leftMargin: 4
                    anchors.verticalCenter: parent.verticalCenter
                }

                MouseArea{
                    id: recent_files_mouse_area
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: [parent.color = "#adadb2", text_recent_files.color = "#4d4d4f"]
                    onExited: [parent.color = "transparent", text_recent_files.color = "#4d4d4f"]
                    onPressed: [parent.color = "#4d4d4f", text_recent_files.color = "#adadb2"]
                    onReleased: recent_files_mouse_area.containsMouse ? [parent.color = "#adadb2", text_recent_files.color = "#4d4d4f"] : [parent.color = "transparent", text_recent_files.color = "#4d4d4f"]
                }
            }
        }

        Separator{
            anchors.bottom: parent.bottom
        }
    }
}

