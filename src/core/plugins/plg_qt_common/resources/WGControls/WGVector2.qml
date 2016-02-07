import QtQuick 2.3
import QtQuick.Layouts 1.1
import BWControls 1.0

/*!
 \brief A vector2 variation of the vectorN control
*/

WGVectorN {
    id: vector3
    property vector2d value
    property vector2d temp_value

    vectorData: [value.x, value.y]
    vectorLabels: ["X:", "Y:"]

    onElementChanged: {
        switch (index) {
        case 0:
            temp_value.x = value_;
            break;
        case 1:
            temp_value.y = value_;
            break;
        }
        setValueHelper(vector2, "value", temp_value);
    }
}
