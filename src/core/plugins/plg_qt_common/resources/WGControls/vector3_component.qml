import QtQuick 2.3
import QtQuick.Layouts 1.1
import WGControls 1.0

/*!
 \brief A vector3 variation of the vectorN control which is bound to reflected data
*/

WGVector3 {
    id: vector3
    objectName: itemData != null ? itemData.IndexPath : "vector3_component"
    value: itemData.Value

    Binding {
        target: itemData
        property: "Value"
        value: vector3.value
    }
}
