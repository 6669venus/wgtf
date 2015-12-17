import QtQuick 2.3
import QtQuick.Layouts 1.1
import BWControls 1.0

/*!
 \brief A vectorRGB variation of the vectorN control
*/

WGVector3 {
    vectorLabels: ["R:", "G:", "B:"]
    minimumValue: 0
    maximumValue: 255
    decimals: 0
    stepsize: 1
}

