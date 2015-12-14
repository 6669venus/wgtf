import QtQuick 2.3
import QtQuick.Layouts 1.1
import BWControls 1.0

WGVector2 {
    id: reflectedVector2

    Binding {
        target: itemData
        property: "Value.x"
        value: reflectedVector2.vector_1
    }

    Binding {
        target: reflectedVector2
        property: "vector_1"
        value: itemData.Value.x
    }

    Binding {
        target: itemData
        property: "Value.y"
        value: reflectedVector2.vector_2
    }

    Binding {
        target: reflectedVector2
        property: "vector_2"
        value: itemData.Value.y
    }
}
