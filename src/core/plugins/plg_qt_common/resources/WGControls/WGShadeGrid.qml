import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

import BWControls 1.0
import WGControls 1.0

/*!
 \A 7x7 grid of color swatches displays variations in lightness and hue for the current color
 allowing the user to make small changes that are easily reversed.

 This is best paired with some other method of making larger color changes and editing the saturation.

 The grid will occupy the height and width of its parent object with some rounding on the width to make the grid
 squares and gaps even pixel ratios if equalizeGrid = true.

Example:
\code{.js}
WGShadeGrid {
    anchors.centerIn: parent
    hue: myColorHue
    saturation: myColorSaturation
    lightness: myColorLightness
    alpha: myColorAlpha

    onUpdateColor: {
        myColorHue = h
        myColorSaturation = s
        myColorLightness = l
    }
}
\endcode
*/

GridLayout {
    id: shadeGrid

    /*!
        The number of rows in the grid. This is best set to an odd number.

        The default is 7.
    */
    columns: 7

    /*!
        The number of rows in the grid. This is best set to an odd number.

        The default is 7.
    */
    rows: 7

    /*!
        The hue value or chroma of the chosen color from 0 to 1.0
    */
    property real hue: 0
    /*!
        The saturation ("amount" of color) of the chosen color from 0 to 1.0
    */
    property real saturation: 0
    /*!
        The lightness (how much black or white) of the chosen color from 0 to 1.0
    */
    property real lightness: 0
    /*!
        The transparency of the chosen color from 0 (invisible) to 1.0 (opaque)
    */
    property real alpha: 0

    /*!
        Makes sure the cells and gaps are equal at the expense of some margins/rounding widthwise.

        The default is true.
    */
    property bool equalizeGrid: true

    /*!
        This signal is fired when one of the swatches is clicked.

        It returns the new hue, saturation and lightness values.
    */
    signal updateColor(real h, real s, real l)

    /*!
        The amount to offset the hue by.

        The default is 0.05
    */
    property real hueOffsetAmount: 0.05

    /*!
        The amount to offset the lightness by.

        The default is 0.1
    */
    property real lightOffsetAmount: 0.1

    height: {
        if (Component.status == Component.Ready)
        {
            if (equalizeGrid)
            {
                (parent.height) - parent.height % rows
            }
            else
            {
                parent.height
            }
        }
    }
    width: height

    columnSpacing: 0
    rowSpacing: 0

    Repeater {
        model: columns * rows
        Item {
            id: shadeDelegate
            Layout.fillWidth:  equalizeGrid ? false : true
            Layout.fillHeight:  equalizeGrid ? false : true
            Layout.preferredWidth: equalizeGrid ? shadeGrid.width / columns : -1
            Layout.preferredHeight:  equalizeGrid ? shadeGrid.height / rows : -1

            property int rowIndex: index / rows
            property int columnIndex: index % columns

            property real hueOffset: {
                var hOff = hue + ((rowIndex - (Math.floor(rows / 2))) * hueOffsetAmount)
                if (hOff > 1.0)
                {
                    hOff -= 1.0
                }
                if (hOff < 0)
                {
                    hOff = 1.0 + hOff
                }
                return hOff
            }

            property real lightOffset: {
                var lOff = lightness + ((columnIndex - (Math.floor(columns / 2))) * lightOffsetAmount)
                if (lOff > 1.0)
                {
                    lOff = 1.0
                }
                if (lOff < 0)
                {
                    lOff = 0.0
                }
                return lOff
            }

            Rectangle {
                id: shadeBox
                anchors.fill: parent
                anchors.margins: clickArea.containsMouse ? 0 : 2

                //give the center square a white border
                border.width: index == Math.floor((columns * rows) / 2) ? 1 : 0
                border.color: index == Math.floor((columns * rows) / 2) ? "white" : "transparent"
                color: Qt.hsla(hueOffset,saturation,lightOffset,alpha)

                Image {
                    source: "icons/bw_check_6x6.png"
                    fillMode: Image.Tile
                    anchors.fill: parent
                    z: -1
                    visible: alpha < 1.0
                }
            }

            MouseArea {
                id: clickArea
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onClicked: {
                    shadeGrid.updateColor(shadeDelegate.hueOffset,saturation,shadeDelegate.lightOffset)
                }
            }
        }
    }
}
