import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import WGControls 1.0

Loader
{   
    id: demo_Loader
    Component.onCompleted: {
        var count = getObjectCount();
        if(count == 0)
        {
            demo_Loader.setSource("viewport_test_window.qml");
        }
        else
        {
            demo_Loader.setSource("Framebuffer.qml");
        }
    }
}