import QtQuick 2.4
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import WGControls 1.0

RowLayout {
    visible: true

    property QtObject panelProps: WGPanelProperties{}

    ColumnLayout
    {
        anchors.fill: parent
        id: content
        spacing: 0
        Layout.fillHeight: true
        Layout.fillWidth: true

        MainToolBar {
            id:toolbar
            togglePreview.onClicked: {
                resourceView.showPreview = !resourceView.showPreview
            }
        }

        ResourceView {
            id: resourceView
            explorerModel: TestModel{}
            fileListModel: ListModel{}
            showToolbar: false

            onShowPreviewChanged: {
                toolbar.togglePreview.checked = resourceView.showPreview
            }
        }
    }
}
