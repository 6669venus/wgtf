import QtQuick 2.3
import QtQuick.Layouts 1.1
import BWControls 1.0

/*!
 \brief A vector2 variation of the vectorN control
*/

WGVectorN {
    property vector2d value

    vectorData: [value.x, value.y]
    vectorLabels: ["X:", "Y:"]
    onElementChanged: {
        switch (index) {
        case 0:
            value.x = value;
            break;

        case 1:
            value.y = value;
            break;
        }
    }
}
