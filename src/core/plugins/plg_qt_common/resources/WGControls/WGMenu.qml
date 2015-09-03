import QtQuick 2.3
import QtQuick.Controls 1.2

/*!
 \brief A WG stylised menu

Example:
\code{.js}
    menuBar: MenuBar {
        WGMenu {
            title: qsTr("File")

            MenuItem {
                text: qsTr("Exit")
                onTriggered: Qt.quit();
            }
        }
    }
\endcode
*/

Menu {
    objectName: "WGMenu"
    style: WGMenuStyle{}
}
