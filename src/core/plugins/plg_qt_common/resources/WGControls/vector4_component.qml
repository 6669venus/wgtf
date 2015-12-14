import QtQuick 2.3
import QtQuick.Layouts 1.1
import BWControls 1.0

WGVector3 {
    id: reflectedVector4

    Binding {
        target: itemData
        property: "Value.x"
        value: reflectedVector4.vector_1
    }

    Binding {
        target: reflectedVector4
        property: "vector_1"
        value: itemData.Value.x
    }

    Binding {
        target: itemData
        property: "Value.y"
        value: reflectedVector4.vector_2
    }

    Binding {
        target: reflectedVector4
        property: "vector_2"
        value: itemData.Value.y
    }

    Binding {
        target: itemData
        property: "Value.z"
        value: reflectedVector4.vector_3
    }

    Binding {
        target: reflectedVector4
        property: "vector_3"
        value: itemData.Value.z
    }

    Binding {
        target: itemData
        property: "Value.w"
        value: reflectedVector4.vector_4
    }

    Binding {
        target: reflectedVector4
        property: "vector_4"
        value: itemData.Value.w
    }
}
