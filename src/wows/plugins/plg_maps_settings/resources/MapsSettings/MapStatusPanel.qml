import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls 1.0

// WGPanel triggers auto-reloading when QML file is saved
WGPanel {
    color: palette.MainWindowColor
    property var title: "Map Status"
    property var layoutHints: { 'history': 0.1 }
    property var topControlsHeight: 20

    //TODO: Replace me with a proper model
    ListModel {
        id: mapsModel
        ListElement {
            mapName: "(33) spaces/00_CO_ocean"
            mapThumbnail: "icons/Ocean_Standart_second_5_1.JPG"
            mapSize: "16x16"
            mapEnabled: true
            scenarios: [
                ListElement {
                    scenarioName: "MegaBase"
                    matchGroup: 0
                    gameMode: "$GameMode$"
                    battleLogic: "Default"
                },
                ListElement {
                    scenarioName: "Domination_2_Bases"
                    matchGroup: 0
                    gameMode: "$GameMode$"
                    battleLogic: "Domination"
                },
                ListElement {
                    scenarioName: "Ranked Domination"
                    matchGroup: 2
                    gameMode: "$GameMode$"
                    battleLogic: "Domination"
                },
                ListElement {
                    scenarioName: "Skirmish_Domination_2_Bases"
                    matchGroup: 1
                    gameMode: "$GameMode$"
                    battleLogic: "Domination"
                }
            ]
        }
        ListElement {
            mapName: "(21) spaces/01_solomon_islands"
            mapThumbnail: "icons/01_solomon_islands_5-11.JPG"
            mapSize: "14x14"
            mapEnabled: true
            scenarios: [
                ListElement {
                    scenarioName: "Domination"
                    matchGroup: 0
                    gameMode: "$GameMode$"
                    battleLogic: "Domination"
                },
                ListElement {
                    scenarioName: "Skirmish Domination"
                    matchGroup: 1
                    gameMode: "$GameMode$"
                    battleLogic: "Domination"
                },
                ListElement {
                    scenarioName: "Domination_2_Bases"
                    matchGroup: 0
                    gameMode: "$GameMode$"
                    battleLogic: "Domination"
                },
                ListElement {
                    scenarioName: "Skirmish_Domination_2_Bases"
                    matchGroup: 1
                    gameMode: "$GameMode$"
                    battleLogic: "Default"
                }
            ]
        }
        ListElement {
            mapName: "(37) spaces/08_NE_passage"
            mapThumbnail: "icons/08_NE_passage_5_2.JPG"
            mapSize: "16x16"
            mapEnabled: true
            scenarios: [
                ListElement {
                    scenarioName: "Skirmish Domination"
                    matchGroup: 1
                    gameMode: "$GameMode$"
                    battleLogic: "Domination"
                },
                ListElement {
                    scenarioName: "Skirmish Single Base"
                    matchGroup: 1
                    gameMode: "$GameMode$"
                    battleLogic: "Default"
                }
            ]
        }
    }

    WGScrollPanel {

        childObject_ :
        WGDraggableColumn {
            Repeater {
                model: mapsModel
                MapStatusObject {

                }
            }
        }
    }
}
