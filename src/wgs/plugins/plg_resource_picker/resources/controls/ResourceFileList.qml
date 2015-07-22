import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.2

TableView {
    signal fileChosen(variant model)

    sortIndicatorVisible: true
    // TODO: Add the ability to sort the data model
    onSortIndicatorColumnChanged: model.sort(sortIndicatorColumn, sortIndicatorOrder)
    onSortIndicatorOrderChanged: model.sort(sortIndicatorColumn, sortIndicatorOrder)
    TableViewColumn {
        id: nameColumn
        role: "name"
        title: "Name"
        width: 200
        delegate: RowLayout{
            Image {
                id: image
                source: model ? "qrc:/icons/" + model.icon + ".png" : ""
                ToolTip
                {
                    anchors.fill: parent
                    text: model ? model.status : ""
                }
            }
            Text {
                Layout.fillWidth: true
                anchors.verticalCenter: parent.verticalCenter
                horizontalAlignment: Text.AlignLeft
                color: styleData.textColor
                elide: styleData.elideMode
                text: styleData.value
            }
        }
    }
    TableViewColumn {
        role: "size"
        title: "Size"
        width: 50
    }
    TableViewColumn {
        role: "date"
        title: "Date"
        width: 150
    }
    TableViewColumn {
        role: "status"
        title: "Status"
        width: 300
    }

    onDoubleClicked: {
        // TODO: emit a signal to open the clicked on file
        fileChosen(model)
    }
}
