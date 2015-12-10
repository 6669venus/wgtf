import QtQuick 2.3
import QtQuick.Layouts 1.1
import BWControls 1.0

WGVector3 {
    id: reflectedVector3

    Binding {
        target: itemData
        property: "Value.x"
        value: reflectedVector3.vector_1
    }

    Binding {
        target: reflectedVector3
        property: "vector_1"
        value: itemData.Value.x
    }

    Binding {
        target: itemData
        property: "Value.y"
        value: reflectedVector3.vector_2
    }

    Binding {
        target: reflectedVector3
        property: "vector_2"
        value: itemData.Value.y
    }


    Binding {
        target: itemData
        property: "Value.z"
        value: reflectedVector3.vector_3
    }

    Binding {
        target: reflectedVector3
        property: "vector_3"
        value: itemData.Value.z
    }
}
