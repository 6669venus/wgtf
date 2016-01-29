import QtQuick 2.3
import QtQuick.Layouts 1.1
import BWControls 1.0

/*!
 \brief A vectorRGBA variation of the vectorN control
*/

WGVector4 {
    vectorLabels: ["R:", "G:", "B:", "A:"]
    minimumValue: 0
    maximumValue: 255
    decimals: 0
    stepsize: 1
}

