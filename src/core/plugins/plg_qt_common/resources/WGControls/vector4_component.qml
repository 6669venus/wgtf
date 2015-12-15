import QtQuick 2.3
import QtQuick.Layouts 1.1
import BWControls 1.0

/*!
 \brief A vector4 component variation of the vectorN control which is bound to reflected data
*/

WGVectorN {
    id: reflectedVector4

    vectorData: [itemData.Value.x, itemData.Value.y, itemData.Value.z, itemData.Value.w]
    vectorLabels: ["X:", "Y:", "Z:", "W:"]
    onElementChanged: {
        switch (index) {
        case 0:
            itemData.Value.x = value;
            break;

        case 1:
            itemData.Value.y = value;
            break;

        case 2:
            itemData.Value.z = value;
            break;

        case 3:
            itemData.Value.w = value;
            break;
        }
    }
}
