import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls 1.0

/*!
 \brief A collapsible panel that shows the status of a map and it's scenarios
 Currently requires a list model in the format:

Example:
\code{.js}
ListModel {
    id: mapsModel
    ListElement {
        mapName: "Map Name"
        mapThumbnail: "icons/map_image.JPG"
        mapSize: "16x16"
        mapEnabled: true
        scenarios: [
            ListElement {
                scenarioName: "Scenario Name"
                matchGroup: 0
                gameMode: "$GameMode$"
                battleLogic: "Default"
            }
        ]
    }
}
\endcode
*/


WGSubPanel {

    text: mapName

    /*! This property determines how many scenarios are visible before a scroll bar appears.
        The default value is \c 5
    */
    property int scenarioVisibleLines: 5

    /*! This property determines how wide the map thumbnails will be and also affects the width
      of the left column in the panel.
        The default value is \c 128
    */
    property int thumbnailSize: 128

    /*! \internal */
    property int scenarioCount: scenarios.count

    titleFontSize: 12
    subtitleFontSize: 10

    subText: scenarioCount + " scenarios"

    //TODO Make sure WGContextMenu has checked states
    menu: WGMenu {
        MenuItem {
            text: "Rearrange Panels"
            checkable: true
            checked: globalSettings.dragLocked
            onTriggered: {
                globalSettings.dragLocked = !globalSettings.dragLocked
            }
        }
    }

    headerObject_ :
        WGExpandingRowLayout {
            clip: true
            Item {
                Layout.fillWidth: true
            }

            Item {
                Layout.preferredWidth: thumbnailSize
                Layout.preferredHeight: defaultSpacing.minimumRowHeight
                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight


                Rectangle {
                    anchors.fill: parent
                    anchors.margins: 1

                    color: palette.DarkColor

                    Item {
                        anchors.fill: parent
                        anchors.margins: 1
                        clip: true

                        Image {
                            source: mapThumbnail
                            anchors.centerIn: parent
                            sourceSize.height: thumbnailSize

                            Rectangle {
                                anchors.fill: parent
                                color: "#44FF0000"
                                visible: !mapEnabled

                                Text {
                                    id: enabledStatus
                                    anchors.centerIn: parent
                                    font.bold: true
                                    horizontalAlignment: Text.AlignHCenter
                                    text: "Disabled"
                                    color: "red"
                                }
                            }

                            //TODO Set enabled/disabled state properly
                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                propagateComposedEvents: false

                                onClicked: {
                                    mapsModel.set(index, {"mapEnabled": !mapEnabled})
                                }
                            }
                        }
                    }
                }
            }
        }
    childObject_:
    Item {
        height: ((defaultSpacing.minimumRowHeight + defaultSpacing.rowSpacing) * scenarioVisibleLines) + defaultSpacing.minimumRowHeight + defaultSpacing.doubleMargin

        WGColumnLayout {
            id: leftPanel
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: thumbnailSize
            Image {
                source: mapThumbnail
                fillMode: Image.PreserveAspectCrop
                sourceSize.height: thumbnailSize
            }

            Item {
                Layout.fillHeight: true
            }

            Item {
                Layout.preferredHeight: defaultSpacing.minimumRowHeight
                Layout.fillWidth: true
                Text {
                    id: mapSizeText
                    anchors.left: parent.left
                    text: mapSize
                    font.pointSize: 10
                    font.bold: true
                    color: palette.NeutralTextColor
                }

                Text {
                    id: scenariosText
                    anchors.left: mapSizeText.right
                    anchors.right: parent.right
                    text: scenarioCount + " scenarios"
                    font.pointSize: 10
                    font.bold: true
                    color: palette.TextColor
                    horizontalAlignment: Text.AlignRight
                    elide: Text.ElideRight
                }
            }
        }
        WGColumnLayout {
            id: rightPanel
            anchors.left: leftPanel.right
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                WGScrollPanel {
                    anchors.fill: parent
                    childObject_:
                    WGColumnLayout {
                        anchors.left: parent.left
                        anchors.top: parent.top
                        anchors.right: parent.right
                        Repeater {
                            model: scenarios

                            //TODO Set scenario data properly
                            //TODO Make some readonly???

                            WGExpandingRowLayout {
                                WGTextBox {
                                    Layout.preferredWidth: rightPanel.width / 3
                                    Layout.fillWidth: true
                                    text: scenarioName
                                }
                                WGDropDownBox {
                                    Layout.fillWidth: true
                                    currentIndex: matchGroup
                                    model:ListModel {
                                        ListElement { text: "PvP" }
                                        ListElement { text: "PvE" }
                                        ListElement { text: "PvP Ranked" }
                                    }
                                }
                                WGTextBox {
                                    Layout.fillWidth: true
                                    text: gameMode
                                }

                                WGTextBox {
                                    Layout.fillWidth: true
                                    text: battleLogic
                                }

                                //TODO Delete scenario properly
                                WGToolButton {
                                    iconSource: "icons/delete_sml_16x16.png"
                                    onClicked: {
                                        scenarios.remove(index)
                                    }
                                }
                            }
                        }
                    }
                }
            }

            WGExpandingRowLayout {
                Layout.fillWidth: true
                Layout.preferredHeight: defaultSpacing.minimumRowHeight
                Item {
                    Layout.fillWidth: true
                }

                //TODO Enable/Disable map properly
                WGPushButton {
                    Layout.preferredWidth: 80
                    text: mapEnabled ? "Enabled" : "Disabled"
                    checkable: true
                    checked: mapEnabled

                    //TODO This is a bit hacky but I currently can't check when mapEnabled changes
                    onTextChanged: {
                        checked = mapEnabled
                    }

                    onClicked: {
                        mapsModel.set(index, {"mapEnabled": !mapEnabled})
                    }
                }

                //TODO Add scenario properly
                WGPushButton {
                    text: "Add Scenario"
                    onClicked: {
                        scenarios.append({"scenarioName": "Scenario Name",
                                                       "matchGroup": 0,
                                                       "gameMode": "$GameMode$",
                                                       "battleLogic": "Default"
                                                   })
                    }
                }
            }
        }
    }
}
