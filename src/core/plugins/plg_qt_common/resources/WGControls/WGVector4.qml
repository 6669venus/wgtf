import QtQuick 2.3
import QtQuick.Layouts 1.1
import BWControls 1.0

/*!
 \brief A vector4 variation of the vectorN control
*/

WGVectorN {
    id: vector4
    property vector4d temp_value
    property vector4d value

    vectorData: [value.x, value.y, value.z, value.w]
    vectorLabels: ["X:", "Y:", "Z:", "W:"]

    onElementChanged: {
        switch (index) {
        case 0:
            temp_value.x = value_;
            break;
        case 1:
            temp_value.y = value_;
            break;
        case 2:
            temp_value.z = value_;
            break;

        case 3:
            temp_value.w = value_;
            break;
        }
        setValueHelper(vector4, "value", temp_value);
    }
}
