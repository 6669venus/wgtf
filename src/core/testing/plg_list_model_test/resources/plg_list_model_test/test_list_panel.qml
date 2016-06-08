import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls 1.0 as WG1
import WGControls 2.0

WG1.WGPanel {
    id: testListPanel

    property var sourceModel: useModel ? source : null
    property bool useModel: true
    property int topControlsHeight: 20

    title: "ListModel Test 2.0"
    layoutHints: { 'test': 0.1 }
    color: palette.mainWindowColor

    // TODO NGT-2493 ScrollView steals keyboard focus
    Keys.forwardTo: [listView1, listView2]
    focus: true

    Button {
        id: switchModelButton
        anchors.top: parent.top
        anchors.left: parent.left
        width: 150
        height: topControlsHeight
        text: useModel ? "Switch Model Off" : "Switch Model On"

        onClicked: {
            useModel = !useModel;
        }
    }

    Column {
        anchors.top: switchModelButton.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        ScrollView {
            anchors.left: parent.left
            anchors.right: parent.right
            height: parent.height / 2

            WGListView {
                id: listView1
                //anchors.margins: 10
                //leftMargin: 50
                //rightMargin: 50
                //topMargin: 50
                //bottomMargin: 50
                columnWidths: [70, 150]
                columnSpacing: 1
                columnDelegates: [columnDelegate, colorDelegate]
                headerDelegate: myHeaderDelegate
                footerDelegate: myFooterDelegate
                roles: ["value", "headerText", "footerText"]
                model: sourceModel
                sortIndicator: indicator
                clamp: false
            }
        }
        ScrollView {
            anchors.left: parent.left
            anchors.right: parent.right
            height: parent.height / 2

            WGListView {
                id: listView2
                //anchors.margins: 10
                //leftMargin: 50
                //rightMargin: 50
                //topMargin: 50
                //bottomMargin: 50
                columnWidths: [70, 150]
                columnSpacing: 1
                columnDelegates: [columnDelegate, colorDelegate]
                headerDelegate: myHeaderDelegate
                footerDelegate: myFooterDelegate
                roles: ["value", "headerText", "footerText"]
                model: sourceModel
                sortIndicator: indicator
                clamp: true
            }
        }

        property alias indicator: indicator
        property alias myHeaderDelegate: myHeaderDelegate
        property alias myFooterDelegate: myFooterDelegate
        property alias colorDelegate: colorDelegate

        Component {
            id: indicator
            Item {
                height: sortArrowImage.height
                Image {
                    id: sortArrowImage
                    anchors.centerIn: parent
                    source: headerSortIndex == 0 ? "icons/sort_up.png" : "icons/sort_down.png"
                }
            }
        }

        Component {
            id: myHeaderDelegate

            Text {
                id: textBoxHeader
                color: palette.textColor
                text: valid ? headerData.headerText : ""
                height: 24

                property bool valid: headerData !== null &&
                    typeof headerData !== "undefined" &&
                    typeof headerData.headerText !== "undefined"
            }
        }

        Component {
            id: myFooterDelegate

            Text {
                id: textBoxFooter
                color: palette.textColor
                text: valid ? headerData.footerText : ""
                height: 24

                property bool valid: headerData !== null &&
                    typeof headerData !== "undefined" &&
                    typeof headerData.footerText !== "undefined"
            }
        }

        Component {
            id: colorDelegate

            Item {
                width: itemWidth
                implicitWidth: Math.max(textItem.implicitWidth, 200)
                implicitHeight: 24

                Rectangle {
                    id: colorItem
                    objectName: "colorDelegate_color_" + textItem.text

                    anchors.fill: parent
                    anchors.margins: 1
                    color: {
                        if (typeof itemData.value === "string")
                        {
                            return "transparent";
                        }
                        
                        var colour = itemData.value;
                        var r = colour > 9999 ? (colour / 10000) % 100 + 156 : 0;
                        var g = colour > 99 ? (colour / 100) % 100 + 156 : 0;
                        var b = colour % 100 + 156;
                        
                        return Qt.rgba(r / 255, g / 255, b / 255, 1);
                    }
                }

                Text {
                    id: textItem
                    objectName: "colorDelegate_text_" + textItem.text

                    visible: typeof itemData.value === "string"
                    text: typeof itemData.value === "string" ? itemData.value : ""
                    color: palette.textColor
                }
            }
        }
    }
}
