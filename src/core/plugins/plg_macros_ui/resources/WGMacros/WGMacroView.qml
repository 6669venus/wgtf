import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import WGControls 1.0

// This component is for displaying the macro panel
Rectangle {
    property var title: "Macros"

    property var layoutHints: { 'macros': 1.0 }

    id: root
    color: palette.MainWindowColor

    property string activeMacro: ""

    property bool playing: false
    property bool recording: false
    property bool idle: true
    property alias macroSelectionExtension: macroModelSelectionExtension

    states: [
        State {
            name: "IDLE"
            when: idle

            PropertyChanges {
                target: macroStatus
                text: "Idle"
                color: palette.DisabledTextColor
            }
            PropertyChanges {
                target: stopButton
                enabled: false
            }
            PropertyChanges {
                target: playButton
                iconSource: "icons/play_16x16.png"
                enabled: true
            }
            PropertyChanges {
                target: recordButton
                iconSource: "icons/record_off_16x16.png"
                enabled: true
            }
        },
        State {
            name: "PLAYING"
            when: playing

            PropertyChanges {
                target: macroStatus
                text: "Playing " + activeMacro + "..."
                color: palette.TextColor
            }
            PropertyChanges {
                target: stopButton
                enabled: true
            }
            PropertyChanges {
                target: playButton
                iconSource: "icons/play_on_16x16.png"
            }
            PropertyChanges {
                target: recordButton
                enabled: false
            }
        },
        State {
            name: "RECORDING"
            when: recording

            PropertyChanges {
                target: macroStatus
                text: "Recording Macro..."
                color: palette.TextColor
            }
            PropertyChanges {
                target: stopButton
                enabled: true
            }
            PropertyChanges {
                target: playButton
                enabled: false
            }
            PropertyChanges {
                target: recordButton
                iconSource: "icons/record_on_16x16.png"
            }
        }
    ]

    WGListModel {
        id : macroModel

        source : Macros

        ValueExtension {}
        ColumnExtension {}
        SelectionExtension {
            id: macroModelSelectionExtension
            onSelectionChanged: {
                setSelectedRow(macroModel.indexRow(selectedIndex));
            }
        }

    }


    WGFrame {
        anchors.fill: parent

        WGColumnLayout {
            anchors.fill: parent
            anchors.margins: defaultSpacing.standardMargin


            //Placeholder buttons add to as required
            WGExpandingRowLayout {
                Layout.preferredHeight: defaultSpacing.minimumRowHeight + defaultSpacing.doubleBorderSize
                Layout.fillWidth: true

                WGPushButton {
                    id: stopButton
                    iconSource: "icons/stop_16x16.png"
                    enabled: false
                    onClicked:{
                        playing = false
                        recording = false
                        idle = true
                        activeMacro = ""
                    }
                }

                WGPushButton {
                    id: playButton
                    iconSource: "icons/play_16x16.png"
                    onClicked:{
                        if(!playing){
                            var macro = SelectedMacro;
                            if(macro == null)
                            {
                                return;
                            }
                            var macroObject = macro.DisplayObject;
                            if(macroObject == null)
                            {
                                return;
                            }
                            idle = false
                            playing = true

                            //get Macro name
                            activeMacro = macroObject.DisplayName
                            macroObject.RunMacro;
                        }
                    }
                }

                WGPushButton {
                    id: recordButton
                    iconSource: "icons/record_off_16x16.png"

                    onClicked:{
                        if(!recording){
                            idle = false
                            recording = true
                        }
                    }
                }

                Rectangle {
                    color: "transparent"
                    Layout.preferredWidth: defaultSpacing.rowSpacing
                }

                WGLabel {
                    id: macroStatus
                    text: "Idle"
                    color: palette.DisabledTextColor
                }

                Rectangle {
                    color: "transparent"
                    Layout.fillWidth: true
                }

                WGPushButton {
                    iconSource: "icons/new_folder_16x16.png"
                    onClicked: {
                        //create a new folder
                    }
                }

                WGPushButton {
                    iconSource: "icons/close_16x16.png"

                    onClicked: {
                        var macro = SelectedMacro;
                        if(macro == null)
                        {
                            return;
                        }
                        deleteMacro( macro.Id )
                    }
                }
            }

            //Frame around the List View
            WGTextBoxFrame {
                Layout.fillHeight: true
                Layout.fillWidth: true

                // Macro list
                WGListView {
                    id: macros
                    anchors.fill: parent
                    anchors.margins: defaultSpacing.standardMargin
                    selectionExtension: root.macroSelectionExtension
                    model: macroModel
                    columnDelegates: [columnDelegate]

                    Component {
                        id: columnDelegate

                        Loader {
                            source: "plg_macros_ui/WGCompoundCommandDelegate.qml"
                        }
                    }
                }
            }
        }
    }
}

