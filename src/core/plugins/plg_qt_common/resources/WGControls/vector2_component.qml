import QtQuick 2.3
import QtQuick.Layouts 1.1
import BWControls 1.0

/*!
 \brief A vector2 variation of the vectorN control for reflected data
*/

WGVector2 {
    id: vector2
    value: itemData.Value

    Binding {
        target: itemData
        property: "Value"
        value: vector2.value
    }
}
