import QtQuick 2.3
import QtQuick.Layouts 1.1
import BWControls 1.0

/*!
 \brief A vector2 component variation of the vectorN control which is bound to reflected data
*/

WGVectorN {
    id: reflectedVector2

    vectorData: [itemData.Value.x, itemData.Value.y]
    vectorLabels: ["X:", "Y:"]
    onElementChanged: {
        switch (index) {
        case 0:
            itemData.Value.x = value;
            break;

        case 1:
            itemData.Value.y = value;
            break;
    }
}
