import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.3

ColumnLayout
{
    id: view
    spacing: 0
    Layout.fillHeight: true
    Layout.fillWidth: true

    property ListModel explorerModel
    property ListModel fileListModel
    property bool showPreview: true
    property real previewHeight: 200
    property bool showToolbar: true

    property ResourceExplorer explorer: ResourceExplorer {
        treeModel: explorerModel
        objectName: "explorer"
        onNodeSelected: {
            fileListModel.clear();
            if(model.nodes){
                for(var i = 0; i < model.nodes.count; ++i){
                    fileListModel.set(i, model.nodes.get(i));
                }
                view.updatePreview();
            }
        }
    }

    property ResourceFileList filelist : ResourceFileList {
        model: fileListModel
        anchors.fill: parent
        objectName: "filelist"

        onCurrentRowChanged: {
            view.updatePreview();
        }

        onFileChosen: {
            console.log(model)
        }
    }

    property TagSearch tagSearch : TagSearch {}

    property Scan scan : Scan {}

    property alias preview: preview

    function updatePreview()
    {
        // Destroy the current preview
        for(var i = 0; i < preview.children.length; ++i)
        {
            preview.children[i].destroy();
        }
        if(filelist.currentRow > -1 && filelist.currentRow < filelist.model.count){
            // TODO: Create the proper type of preview based on file type
            var textPreview = Qt.createComponent("TextPreview.qml");
            textPreview.createObject(preview, {"text":  filelist.model.get(filelist.currentRow).name});
        }
    }

    onShowPreviewChanged: {
        if(showPreview){
            preview.height = previewHeight
        } else if(preview.height != 0) {
            previewHeight = preview.height
            preview.height = 0
        }

        toolbar.togglePreview.checked = showPreview
    }

    onShowToolbarChanged:{
        toolbar.visible = showToolbar
    }

    ResourceViewToolBar {
        id:toolbar
        togglePreview.onClicked: {
            resourceView.showPreview = !resourceView.showPreview
        }
    }

    SplitView {
        id: vsplitter
        orientation: Qt.Horizontal
        Layout.fillHeight: true
        Layout.fillWidth: true

        TabView {
            id: tabview
            tabPosition: 8
            frameVisible: false
            width: .45 * parent.width

            Tab{
                title: "Directory"
                data: explorer
            }
            Tab{
                title: "Tag Search"
                data: tagSearch
            }
            Tab{
                title: "Scan"
                data: scan
            }
        }

        SplitView {
            id: hsplitter
            orientation: Qt.Vertical

            property alias preview:preview

            Item
            {
                data: filelist
                Layout.fillHeight: true
            }

            Item {
                id: preview
                objectName: "preview"
                height: previewHeight
                onHeightChanged:{
                    if(height == 0 && showPreview)
                        showPreview = false;
                    else if(height != 0 && !showPreview)
                    {
                        previewHeight = height;
                        showPreview = true;
                    }
                }
            }
        }
    }
}
