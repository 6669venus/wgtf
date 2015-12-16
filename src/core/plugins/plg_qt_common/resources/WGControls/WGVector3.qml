import QtQuick 2.3
import QtQuick.Layouts 1.1
import BWControls 1.0

/*!
 \brief A vector3 variation of the vectorN control
*/

WGVectorN {
    property vector3d value

    vectorData: [value.x, value.y, value.z]
    vectorLabels: ["X:", "Y:", "Z:"]
    onElementChanged: {
        switch (index) {
        case 0:
            value.x = value;
            break;

        case 1:
            value.y = value;
            break;

        case 2:
            value.z = value;
            break;
        }
    }
}
