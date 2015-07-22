import QtQuick 2.4


ListModel {

    Component.onCompleted: createModel();

    function createModel() {
        append([{
            "icon": "Folder",
            "name": "Assets",
            "size": "",
            "date": "",
            "status": "Local Folder",
            "level": 0,
            "nodes": [
                {
                    "icon": "Search",
                    "name": "Dir",
                    "size": "0",
                    "date": "3/25/2015 6:18:09 AM",
                    "status": "Old Version",
                    "level": 1,
                    "nodes": [
                        {
                            "icon": "File",
                            "name": "Three",
                            "size": "0",
                            "date": "3/25/2015 6:18:09 AM",
                            "status": "Old Version",
                            "level": 2,
                            "nodes": []
                        }]
                },
                {
                    "icon": "File",
                    "name": "test2.txt",
                    "size": "0",
                    "date": "3/25/2015 8:18:09 AM",
                    "status": "Old Version",
                    "level": 1,
                    "nodes": []
                }
            ]
        },{
            "icon": "File",
            "name": ".p4ignore",
            "size": "34",
            "date": "3/10/2015 6:18:09 PM",
            "status": "Old Version",
            "level": 0,
            "nodes":[
                {
                    "icon": "File",
                    "name": "test2.txt",
                    "size": "0",
                    "date": "3/25/2015 8:18:09 AM",
                    "status": "Old Version",
                    "level": 1,
                    "nodes": []
                }]
        },{
            "icon": "File",
            "name": ".p4ignore",
            "size": "34",
            "date": "3/10/2015 6:18:09 PM",
            "status": "Old Version",
            "level": 0,
            "nodes":[
                {
                    "icon": "File",
                    "name": "test2.txt",
                    "size": "0",
                    "date": "3/25/2015 8:18:09 AM",
                    "status": "Old Version",
                    "level": 1,
                    "nodes": []
                }]
        }]);
    }
}
