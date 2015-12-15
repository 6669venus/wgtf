import QtQuick 2.3
import QtQuick.Layouts 1.1
import BWControls 1.0

/*!
 \brief A vector3 component variation of the vectorN control which is bound to reflected data
*/

WGVectorN {
    id: reflectedVector3

    vectorData: [itemData.Value.x, itemData.Value.y, itemData.Value.z]
    vectorLabels: ["X:", "Y:", "Z:"]
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
        }
    }
}
