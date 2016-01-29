import QtQuick 2.3
import QtQuick.Layouts 1.1
import BWControls 1.0

/*!
 \brief A vector4 variation of the vectorN control
*/

WGVectorN {
    property vector4d value

    vectorData: [value.x, value.y, value.z, value.w]
    vectorLabels: ["X:", "Y:", "Z:", "W:"]
    onElementChanged: {
        switch (index) {
        case 0:
            value.x = value_;
            break;

        case 1:
            value.y = value_;
            break;

        case 2:
            value.z = value_;
            break;

        case 3:
            value.w = value_;
            break;
        }
    }
}
