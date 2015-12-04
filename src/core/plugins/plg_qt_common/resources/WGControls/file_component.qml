import QtQuick 2.3
import QtQuick.Layouts 1.1
import BWControls 1.0

WGFileSelectBox {
    id: fileComponent
    anchors.left: parent.left
    anchors.right: parent.right
	text: itemData.Value
    readOnly: true
	title: itemData.UrlDialogTitle
	folder: itemData.UrlDialogDefaultFolder
    modality: itemData.UrlDialogModality
	nameFilters: {
		var filters = itemData.UrlDialogNameFilters;
		if(filters.toString() === "")
		{
			return [
            "All files (*)",
            "Image files (*.jpg *.png *.bmp *.dds)",
            "Model files (*.model *.primitives *.visual)",
            "Script files (*.txt *.xml *.py)",
            "Audio files (*.fsb *.fev)",
			]
		}
		var filterArray = filters.toString().split('|');
		return filterArray;
	}
	selectedNameFilter: {
		var filters = itemData.UrlDialogNameFilters;
		if(filters.toString() === "")
		{
			return "All files (*)";
		}
		return itemData.UrlDialogSelectedNameFilter;
		
	}

	dialog: itemData.UrlIsAssetBrowser ? assetDlg: fileDlg

	property var assetDlg : WGAssetBrowserDialog {}
	property var fileDlg : WGNativeFileDialog {}
	
    onFileChosen: {
        itemData.Value = selectedFile
    }
    onFileRejected: {
        closeDialog()
    }
}
