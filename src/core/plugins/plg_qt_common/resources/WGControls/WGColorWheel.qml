import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import QtGraphicalEffects 1.0

import BWControls 1.0
import WGControls 1.0

/*!
 \A Corel Painter style HSL color triangle/wheel

Example:
\code{.js}
WGColorWheel {
    height: 360
    width: 360
}
\endcode
*/


Rectangle {
    id: colorWheel
    objectName: "WGColorWheel"

    color: palette.MainWindowColor

    /*!
        The chroma value or hue of the chosen color from 0 to 1.0
    */
    property real chroma: 0
    /*!
        The saturation ("amount" of color) of the chosen color from 0 to 1.0
    */
    property real saturation: 0
    /*!
        The lightness (how much black or white) of the chosen color from 0 to 1.0
    */
    property real lightness: 0

    /*!
        The currently chosen color.
    */
    property color currentColor: Qt.hsla(chroma,saturation,lightness,1)

    /*!
        The width of the hue ring around the color triangle.
        Changing this also affects the size of the drag handles and
        the margin of the triangle inside the wheel.

        The default value is \c width/8
    */
    property int colorWheelWidth: width/8

    /*!
        Shows circle buttons outside the hue ring to quickly
        select primary, secondary and greyscale colors.

        The default value is \c true
    */
    property bool showShortCuts: true

    /*! \internal */
    property int __shortCutButtonSize: colorWheel.width / 16

    /*! \internal */
    property int __hueRotation: chroma * 360

    property bool __allowUpdating: true

    signal updateHSL(real h, real s, real l)

    onUpdateHSL: {
        __allowUpdating = false
        chroma = h
        saturation = s
        lightness = l
        __allowUpdating = true
    }

    function updateColor()
    {
        if(__allowUpdating)
        {
            currentColor = Qt.hsla(chroma,saturation,lightness,1)
        }
    }

    //keeps Chroma within bounds
    onChromaChanged: {
        if (chroma > 1.0)
        {
            chroma -= 1.0
        }
        else if (chroma < 0)
        {
            chroma = 1.0 + chroma
        }
        updateColor()
    }

    onSaturationChanged: {
        updateColor()
    }
    onLightnessChanged: {
        updateColor()
    }

    Rectangle {
        id: ringBorder
        anchors.fill: parent
        anchors.margins: defaultSpacing.doubleBorderSize
        height: parent.height
        width: height

        color: palette.DarkColor

        radius: 180

        Rectangle {
            id: colorRing
            radius: 180

            anchors.centerIn: parent
            height: parent.height - defaultSpacing.doubleBorderSize * 2
            width: height

            ConicalGradient {
                id: rainbow
                angle: __hueRotation
                source: colorRing
                anchors.fill: parent

                //Makes 90 degrees 'zero'
                rotation: 90

                gradient: Gradient {
                    GradientStop { position: 0.0; color: "#FF0000"}
                    GradientStop { position: 0.167; color: "#FF00FF"}
                    GradientStop { position: 0.333; color: "#0000FF"}
                    GradientStop { position: 0.5; color: "#00FFFF"}
                    GradientStop { position: 0.667; color: "#00FF00"}
                    GradientStop { position: 0.833; color: "#FFFF00"}
                    GradientStop { position: 1.0; color: "#FF0000"}
                }
            }

            Rectangle {
                id: hueSelectorMark
                color: "transparent"
                border.color: "white"
                border.width: 2
                width: (colorWheelWidth - defaultSpacing.doubleBorderSize) / 2
                height: width
                radius: height

                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                anchors.rightMargin: defaultSpacing.standardBorderSize

                Rectangle {
                    color: "transparent"
                    border.color: "black"
                    border.width: 1
                    width: parent.width - defaultSpacing.doubleBorderSize
                    height: width
                    radius: height
                    anchors.centerIn: parent
                }
            }

            MouseArea {
                id: ringDrag
                anchors.fill: parent

                property real initOffset: 0
                property real initRotation: 0
                property real currRotation: 0

                //Gets the mouse location around the circle in degrees.
                // TODO fix when diffX == 0 (and diffY == 0 for some unknown reason)
                function getMouseDegrees(mx, my, object) {
                    var point =  mapToItem (object, mx, my);
                    var diffX = (point.x - object.width/2);
                    var diffY = -1 * (point.y - object.height/2);
                    var rad = Math.atan (diffY / diffX);
                    var deg = (rad * 180 / Math.PI);
                    if (diffX > 0 && diffY > 0) {
                        return 90 - Math.abs (deg);
                    }
                    else if (diffX > 0 && diffY < 0) {
                        return 90 + Math.abs (deg);
                    }
                    else if (diffX < 0 && diffY > 0) {
                        return 270 + Math.abs (deg);
                    }
                    else if (diffX < 0 && diffY < 0) {
                        return 270 - Math.abs (deg);
                    }
                }

                //change the hue on mouse wheel
                onWheel: {
                    if (wheel.angleDelta.y > 0)
                    {
                        chroma += 0.05
                    }
                    else
                    {
                        chroma -= 0.05
                    }
                }

                //set the initial values before rotating
                onPressed: {
                    initRotation = __hueRotation
                    initOffset = getMouseDegrees(mouse.x, mouse.y, ringDrag)
                }

                //jump to color on double click
                onDoubleClicked: {
                    var deg = getMouseDegrees(mouse.x, mouse.y, ringDrag)
                    deg -= 90
                    deg = initRotation - deg
                    if (deg < 0)
                    {
                        deg += 360
                    }
                    if (deg > 360)
                    {
                        deg -= 360
                    }
                    chroma = deg  / 360
                }

                //rotate the hue wheel
                onPositionChanged: {
                    currRotation = getMouseDegrees(mouse.x, mouse.y, ringDrag)
                    chroma = (initRotation + (currRotation - initOffset)) / 360
                }
            }

            Rectangle {
                id: paletteMap

                anchors.centerIn: parent

                //inner circle that obscures the rainbow gradient to form a ring
                width: parent.width - colorWheelWidth
                height: width

                radius: width

                color: palette.MainWindowColor

                MouseArea {
                    id: triangleArea
                    anchors.top: map.top
                    anchors.bottom: map.bottom
                    anchors.left: map.left

                    acceptedButtons: Qt.LeftButton

                    property bool dragging: false

                    z: 1

                    //make sure the width is setup to give an equilateral triangle
                    width: (Math.sqrt(3)/2 * map.width)

                    propagateComposedEvents: false

                    //ignore dragging if the user clicks outside the triangle to begin with
                    onPressed: {
                        if (mouse.y <= triangleArea.height / 2)
                        {
                            if (mouse.x > mouse.y * ((triangleArea.width / triangleArea.height) * 2))
                            {
                                mouse.accepted = false
                            }
                        }

                        else
                        {
                            if (mouse.x > triangleArea.width - ((((mouse.y - triangleArea.height/2) / triangleArea.height)*2) * triangleArea.width))
                            {
                                mouse.accepted = false
                            }
                        }
                    }

                    onPositionChanged: {
                        dragging = true
                        var testX = mouse.x
                        var testY = mouse.y

                        //This function keeps the x value inside the visible triangle. The y is just kept inside the square.

                        //Check if the mouse is in the top or bottom half of the square

                        if (testY <= triangleArea.height / 2)
                        {
                            //Check if the mouse is inside the triangle area of that half
                            if (testX <= testY * ((triangleArea.width / triangleArea.height) * 2))
                            {
                                colorHandle.y = testY
                                colorHandle.x = testX
                            }
                            else
                            {
                                colorHandle.y = testY
                                colorHandle.x = testY * ((triangleArea.width / triangleArea.height) * 2)
                            }
                        }

                        else
                        {
                            //Check if the mouse is inside the triangle area of that half
                            if (testX < triangleArea.width - ((((testY - triangleArea.height/2) / triangleArea.height)*2) * triangleArea.width))
                            {
                                colorHandle.y = testY
                                colorHandle.x = testX
                            }
                            else
                            {
                                colorHandle.y = testY
                                colorHandle.x = triangleArea.width - ((((testY- triangleArea.height/2) / triangleArea.height)*2) * triangleArea.width)
                            }
                        }

                        //Check that the coordinates are still inside the original bounds
                        if (colorHandle.x <= 0)
                        {
                            colorHandle.x = 0
                        }
                        // The 5 is a very slight helper to make it easier to put the handle in the very corner of the triangle
                        else if (colorHandle.x >= triangleArea.width - 5)
                        {
                            colorHandle.x = triangleArea.width
                            colorHandle.y = triangleArea.height/2
                        }

                        if (colorHandle.y <= 0)
                        {
                            colorHandle.y = 0
                        }
                        else if (colorHandle.y >= triangleArea.height)
                        {
                            colorHandle.y = triangleArea.height
                        }

                        //update the data
                        //TODO this is currently functioning like a square. Needs to be a proper triangle projection

                        saturation = colorHandle.x / triangleArea.width
                        lightness = 1 - (colorHandle.y / triangleArea.height)

                    }

                    onReleased: {
                        dragging = false
                    }

                    Item {
                        id: colorHandle
                        Rectangle {
                            color: "transparent"
                            border.color: "white"
                            border.width: 2
                            width: (colorWheelWidth - defaultSpacing.doubleBorderSize) / 2
                            height: width
                            radius: height
                            anchors.centerIn: parent

                            Rectangle {
                                color: "transparent"
                                border.color: "black"
                                border.width: 1
                                width: parent.width - defaultSpacing.doubleBorderSize
                                height: width
                                radius: height
                                anchors.centerIn: parent
                            }
                        }
                        Connections {
                            target: colorWheel
                            onSaturationChanged: {
                                if (!triangleArea.dragging)
                                {
                                    colorHandle.x = triangleArea.width * saturation
                                }
                            }
                            onLightnessChanged: {
                                if (!triangleArea.dragging)
                                {
                                    colorHandle.y = triangleArea.height * (1 - lightness)
                                }
                            }
                        }
                    }
                }

                // Shader to draw the HSL triangle.
                ShaderEffect {
                    id: map
                    width: (Math.sqrt(3)/2*parent.width) - colorWheelWidth / 2
                    height: width
                    layer.enabled: true
                    layer.smooth: true
                    anchors.verticalCenter: parent.verticalCenter
                    x: parent.width - (Math.sqrt(3)/2*width) - (colorWheelWidth / 4)

                    //set the points of the triangle
                    property var p1: Qt.vector2d(0, 0);
                    property var p2: Qt.vector2d(Math.sqrt(3)/2*width, height / 2);
                    property var p3: Qt.vector2d(0, height);

                    property real hue: chroma

                    // set the color of the points of the triangle and let the graphics card blend the values
                    // TODO: This seems off somehow, like it's not exactly HSL space.

                    fragmentShader: "
                    in vec2 qt_TexCoord0;
                    varying vec4 vColor;
                    out vec4 fragColor;

                    void main() {
                        fragColor = vColor;
                    }
                    "

                    // fancy vertex shader that draws a triangle... but probably isn't exactly right.

                    vertexShader: "
                    attribute highp vec4 qt_Vertex;
                    attribute highp vec2 qt_MultiTexCoord0;
                    varying highp vec2 qt_TexCoord0;
                    uniform highp mat4 qt_Matrix;
                    uniform highp vec2 p1;
                    uniform highp vec2 p2;
                    uniform highp vec2 p3;

                    uniform highp float hue;

                    varying vec4 vColor;

                    float hueToIntensity(float v1, float v2, float h) {
                        h = fract(h);
                        if (h < 1.0 / 6.0)
                            return v1 + (v2 - v1) * 6.0 * h;
                        else if (h < 1.0 / 2.0)
                            return v2;
                        else if (h < 2.0 / 3.0)
                            return v1 + (v2 - v1) * 6.0 * (2.0 / 3.0 - h);

                        return v1;
                    }

                    vec3 HSLtoRGB(vec3 color) {
                        float h = color.x;
                        float l = color.z;
                        float s = color.y;

                        if (s < 1.0 / 256.0)
                            return vec3(l, l, l);

                        float v1;
                        float v2;
                        if (l < 0.5)
                            v2 = l * (1.0 + s);
                        else
                            v2 = (l + s) - (s * l);

                        v1 = 2.0 * l - v2;

                        float d = 1.0 / 3.0;
                        float r = hueToIntensity(v1, v2, h + d);
                        float g = hueToIntensity(v1, v2, h);
                        float b = hueToIntensity(v1, v2, h - d);
                        return vec3(r, g, b);
                    }

                    void main() {
                        highp vec2 pos;
                        if (qt_MultiTexCoord0.y < 0.1) {
                            pos = p1;
                            vColor = vec4(1,1,1,1);
                        } else if (qt_MultiTexCoord0.x < 0.1) {
                            pos = p2;
                            vColor.rgb = HSLtoRGB(vec3 (hue,1,0.5));
                            vColor.a = 1.0;
                        } else {
                            pos = p3;
                            vColor = vec4(0,0,0,1);
                        }
                        qt_TexCoord0 = qt_MultiTexCoord0;
                        gl_Position = qt_Matrix * vec4(pos, 0, 1);
                    }"
                }
            }
        }

        // Short cut buttons
        Item {
            anchors.fill: parent
            visible: showShortCuts
            //Greyscale
            Item {
                anchors.horizontalCenter: parent.horizontalCenter
                height: parent.height / 2 + colorWheelWidth / 2 + defaultSpacing.doubleMargin
                width: colorWheelWidth / 2
                y: -colorWheelWidth / 2 - defaultSpacing.doubleMargin
                transformOrigin: Item.Bottom
                rotation: 35
                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: __shortCutButtonSize
                    width: __shortCutButtonSize
                    radius: height
                    border.width: 1
                    border.color: palette.DarkColor
                    color: Qt.hsla(chroma,0,0,1)
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            saturation = 0
                            lightness = 0
                        }
                    }
                }
            }

            Item {
                anchors.horizontalCenter: parent.horizontalCenter
                height: parent.height / 2 + colorWheelWidth / 2 + defaultSpacing.doubleMargin
                width: colorWheelWidth / 2
                y: -colorWheelWidth / 2 - defaultSpacing.doubleMargin
                transformOrigin: Item.Bottom
                rotation: 45
                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: __shortCutButtonSize
                    width: __shortCutButtonSize
                    radius: height
                    border.width: 1
                    border.color: palette.DarkColor
                    color: Qt.hsla(chroma,0,0.5,1)
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            saturation = 0
                            lightness = 0.5
                        }
                    }
                }
            }

            Item {
                anchors.horizontalCenter: parent.horizontalCenter
                height: parent.height / 2 + colorWheelWidth / 2 + defaultSpacing.doubleMargin
                width: colorWheelWidth / 2
                y: -colorWheelWidth / 2 - defaultSpacing.doubleMargin
                transformOrigin: Item.Bottom
                rotation: 55
                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: __shortCutButtonSize
                    width: __shortCutButtonSize
                    radius: height
                    border.width: 1
                    border.color: palette.DarkColor
                    color: Qt.hsla(chroma,0,1,1)
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            saturation = 0
                            lightness = 1
                        }
                    }
                }
            }
            //Greyscale end

            //Primary
            Item {
                anchors.horizontalCenter: parent.horizontalCenter
                height: parent.height / 2 + colorWheelWidth / 2 + defaultSpacing.doubleMargin
                width: colorWheelWidth / 2
                y: -colorWheelWidth / 2 - defaultSpacing.doubleMargin
                transformOrigin: Item.Bottom
                rotation: -35
                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: __shortCutButtonSize
                    width: __shortCutButtonSize
                    radius: height
                    border.width: 1
                    border.color: palette.DarkColor
                    color: Qt.hsla(0,1,0.5,1)
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            chroma = 0
                            saturation = 1
                            lightness = 0.5
                        }
                    }
                }
            }

            Item {
                anchors.horizontalCenter: parent.horizontalCenter
                height: parent.height / 2 + colorWheelWidth / 2 + defaultSpacing.doubleMargin
                width: colorWheelWidth / 2
                y: -colorWheelWidth / 2 - defaultSpacing.doubleMargin
                transformOrigin: Item.Bottom
                rotation: -45
                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: __shortCutButtonSize
                    width: __shortCutButtonSize
                    radius: height
                    border.width: 1
                    border.color: palette.DarkColor
                    color: Qt.hsla(0.333,1,0.5,1)
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            chroma = 0.333
                            saturation = 1
                            lightness = 0.5
                        }
                    }
                }
            }

            Item {
                anchors.horizontalCenter: parent.horizontalCenter
                height: parent.height / 2 + colorWheelWidth / 2 + defaultSpacing.doubleMargin
                width: colorWheelWidth / 2
                y: -colorWheelWidth / 2 - defaultSpacing.doubleMargin
                transformOrigin: Item.Bottom
                rotation: -55
                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: __shortCutButtonSize
                    width: __shortCutButtonSize
                    radius: height
                    border.width: 1
                    border.color: palette.DarkColor
                    color: Qt.hsla(0.667,1,0.5,1)
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            chroma = 0.667
                            saturation = 1
                            lightness = 0.5
                        }
                    }
                }
            }
            //Primary end

            //Secondary
            Item {
                anchors.horizontalCenter: parent.horizontalCenter
                height: parent.height / 2 + colorWheelWidth / 2 + defaultSpacing.doubleMargin
                width: colorWheelWidth / 2
                y: -colorWheelWidth / 2 - defaultSpacing.doubleMargin
                transformOrigin: Item.Bottom
                rotation: -125
                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: __shortCutButtonSize
                    width: __shortCutButtonSize
                    radius: height
                    border.width: 1
                    border.color: palette.DarkColor
                    color: Qt.hsla(0.167,1,0.5,1)
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            chroma = 0.167
                            saturation = 1
                            lightness = 0.5
                        }
                    }
                }
            }

            Item {
                anchors.horizontalCenter: parent.horizontalCenter
                height: parent.height / 2 + colorWheelWidth / 2 + defaultSpacing.doubleMargin
                width: colorWheelWidth / 2
                y: -colorWheelWidth / 2 - defaultSpacing.doubleMargin
                transformOrigin: Item.Bottom
                rotation: -135
                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: __shortCutButtonSize
                    width: __shortCutButtonSize
                    radius: height
                    border.width: 1
                    border.color: palette.DarkColor
                    color: Qt.hsla(0.5,1,0.5,1)
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            chroma = 0.5
                            saturation = 1
                            lightness = 0.5
                        }
                    }
                }
            }
            Item {
                anchors.horizontalCenter: parent.horizontalCenter
                height: parent.height / 2 + colorWheelWidth / 2 + defaultSpacing.doubleMargin
                width: colorWheelWidth / 2
                y: -colorWheelWidth / 2 - defaultSpacing.doubleMargin
                transformOrigin: Item.Bottom
                rotation: -145
                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: __shortCutButtonSize
                    width: __shortCutButtonSize
                    radius: height
                    border.width: 1
                    border.color: palette.DarkColor
                    color: Qt.hsla(0.833,1,0.5,1)
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            chroma = 0.833
                            saturation = 1
                            lightness = 0.5
                        }
                    }
                }
            }
            //Secondary end

            //Buttons
            Item {
                anchors.horizontalCenter: parent.horizontalCenter
                height: parent.height / 2 + colorWheelWidth / 2 + defaultSpacing.doubleMargin
                width: colorWheelWidth / 2
                y: -colorWheelWidth / 2 - defaultSpacing.doubleMargin
                transformOrigin: Item.Bottom
                rotation: 125
                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: __shortCutButtonSize
                    width: __shortCutButtonSize
                    radius: height
                    border.width: 1
                    border.color: palette.DarkColor
                    color: palette.LightShade
                    Text {
                        anchors.centerIn: parent

                        anchors.horizontalCenterOffset: 1
                        anchors.verticalCenterOffset: 1

                        color : palette.NeutralTextColor

                        rotation: 10

                        font.family : "Marlett"
                        font.pixelSize: Math.round(parent.width)

                        renderType: Text.QtRendering
                        text : "\uF033"
                    }
                }
            }

            Item {
                anchors.horizontalCenter: parent.horizontalCenter
                height: parent.height / 2 + colorWheelWidth / 2 + defaultSpacing.doubleMargin
                width: colorWheelWidth / 2
                y: -colorWheelWidth / 2 - defaultSpacing.doubleMargin
                transformOrigin: Item.Bottom
                rotation: 135
                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: __shortCutButtonSize
                    width: __shortCutButtonSize
                    radius: height
                    color: "transparent"
                    Image {
                        anchors.centerIn: parent
                        rotation: -135
                        source: "icons/loop_16x16.png"
                    }
                }
            }
            Item {
                anchors.horizontalCenter: parent.horizontalCenter
                height: parent.height / 2 + colorWheelWidth / 2 + defaultSpacing.doubleMargin
                width: colorWheelWidth / 2
                y: -colorWheelWidth / 2 - defaultSpacing.doubleMargin
                transformOrigin: Item.Bottom
                rotation: 145
                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: __shortCutButtonSize
                    width: __shortCutButtonSize
                    radius: height
                    border.width: 1
                    border.color: palette.DarkColor
                    color: palette.LightShade
                    Text {
                        anchors.centerIn: parent

                        anchors.horizontalCenterOffset: 1
                        anchors.verticalCenterOffset: 1

                        color : palette.NeutralTextColor

                        rotation: -10

                        font.family : "Marlett"
                        font.pixelSize: Math.round(parent.width)

                        renderType: Text.QtRendering
                        text : "\uF034"
                    }
                }
            }
            //Buttons end
        }
    }
}
