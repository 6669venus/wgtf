import QtQuick 2.3
import QtQuick.Layouts 1.1
import BWControls 1.0

/*!
 \brief A vector3 variation of the vectorN control
*/

WGVectorN {
    id: vector3
    property vector3d temp_value
    property vector3d value

    vectorData: [value.x, value.y, value.z]
    vectorLabels: ["X:", "Y:", "Z:"]

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
        }
        setValueHelper(vector3, "value", temp_value);
    }
}
