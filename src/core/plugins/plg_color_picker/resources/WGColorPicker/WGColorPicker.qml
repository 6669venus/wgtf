import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

import BWControls 1.0
import WGControls 1.0
import WGColorPicker 1.0

/*!
 \A fully featured color picker that can be used in a dialog or as a stand alone control.

 Includes an HSL triangle and color wheel, shade control, customiseable palette, screen picking, Hex entry and more.

 Is intended to support more methods of picking color in the left column

Example:
\code{.js}
WGColorPicker {
    id: colorPicker
    anchors.fill: parent
    initialColor: myStartingColor
    onOkClicked: {
        myNewColor = colorPicker.currentColor
    }
    onCancelClicked: {
        myNewColor = myOldColor
    }
}
\endcode

TODO: Make the customiseable color palette use appwide persistent data.
TODO: Make left column/drop down box expandable with more/optional color picking methods.
TODO: Performance pass
*/


Rectangle {
    id: basePanel
    objectName: "WGColorPicker"

    /*!
        The color before any changes are made or accepted with the color picker.
    */
    property color initialColor: Qt.hsla(1,1,1,1)

    /*!
        The currently selected color using the color picker and typically color that will be applied if it is accepted as a dialog.

        This is not intended to be set manually and should be altered by changing the HSLA and/or RGBA values.
    */
    readonly property color currentColor: Qt.hsla(hValue,sValue,lValue,aValue)

    /*!
        The color based off the R, G and B values. This property is primarily to trigger updating the currentColor when the RGB values change.

        This is not intended to be set manually and should be altered by changing the RGBA and/or HSLA values.
    */
    readonly property color rgbColor: Qt.rgba(rValue,gValue,bValue,aValue)

    /*!
        This property holds the temporary color that is displayed in the second preview square if tempColorActive = true

        It is intended to be used when the user is previewing a potential change to currentColor such as when picking from the screen.
    */
    property color tempColor: currentColor

    /*!
        This property will change the second preview square's color to tempColor when true

        It is intended to be used when the user is previewing a potential change to currentColor such as when picking from the screen.
    */
    property bool tempColorActive: false

    /*!
        The array of saved palette colors.

        Currently this is only saved per instance of the color picker.
    */
    property var savedColors: ["#000000","#FFFFFF","#959595","#FF0000","#00FF00","#0000FF","#00FFFF","#FF00FF","#FFFF00"]

    /*!
        This property determines if the color picker allows editing the transparency or not.

        If false, the alpha value will be locked at 1.0 and the alpha slider disabled.

        The default is true
    */
    property bool useAlpha: true

    /*!
        This property determines if the Ok and Cancel Dialog buttons are displayed.

        The default is true
    */
    property bool showDialogButtons: true

    /*!
        The hue value or chroma of the currentColor from 0 to 1.0
    */
    property real hValue: 0

    /*!
        The saturation ("amount" of color) of the currentColor from 0 to 1.0
    */
    property real sValue: 0

    /*!
        The lightness (how much black or white) of the currentColor from 0 to 1.0
    */
    property real lValue: 0

    /*!
        The red value the currentColor from 0 to 1.0
    */
    property real rValue: 0

    /*!
        The green value the currentColor from 0 to 1.0
    */
    property real gValue: 0

    /*!
        The blue value the currentColor from 0 to 1.0
    */
    property real bValue: 0

    /*!
        The transparency of the currentColor from 0 (invisible) to 1.0 (opaque)
    */
    property real aValue: 1

    /*! \internal */
    property bool __updateHSL: false

    /*! \internal */
    property bool __updateRGB: false

    /*! \internal */
    color: palette.MainWindowColor

    /*!
        This signal is fired when the Ok dialog button is clicked.
    */
    signal okClicked()

    /*!
        This signal is fired when the Cancel dialog button is clicked.
    */
    signal cancelClicked()

    /*!
        This function updates the currentColor with new HSLA values
    */
    function setColorHSLA(h,s,l,a)
    {
        __updateHSL = false
        basePanel.hValue = h
        basePanel.sValue = s
        basePanel.lValue = l
        basePanel.aValue = a
        __updateHSL = true
    }

    /*!
        This function updates the currentColor with new RGBA values
    */
    function setColorRGBA(r,g,b,a)
    {
        __updateRGB = false
        basePanel.rValue = r
        basePanel.gValue = g
        basePanel.bValue = b
        basePanel.aValue = a
        __updateRGB = true
    }

    onRgbColorChanged: {
        if (__updateHSL)
        {
            __updateRGB = false
            var tempColor = rgbToHsl(rValue,gValue,bValue)
            hValue = tempColor[0]
            sValue = tempColor[1]
            lValue = tempColor[2]
            __updateRGB = true
        }
    }

    onCurrentColorChanged: {
        if (__updateRGB)
        {
            __updateHSL = false
            rValue = currentColor.r
            gValue = currentColor.g
            bValue = currentColor.b
            __updateHSL = true
        }
    }

    onInitialColorChanged: {
        __updateHSL = false
        __updateRGB = false

        var tempColor = rgbToHsl(initialColor.r,initialColor.g,initialColor.b)

        hValue = tempColor[0]
        sValue = tempColor[1]
        lValue = tempColor[2]

        rValue = initialColor.r
        gValue = initialColor.g
        bValue = initialColor.b

        __updateHSL = true
        __updateRGB = true
    }

    Component.onCompleted: {
        var tempColor = rgbToHsl(initialColor.r,initialColor.g,initialColor.b)

        hValue = tempColor[0]
        sValue = tempColor[1]
        lValue = tempColor[2]

        rValue = initialColor.r
        gValue = initialColor.g
        bValue = initialColor.b

        __updateHSL = true
        __updateRGB = true
    }

    function hueToIntensity(v1, v2, h)
    {
        h = h - Math.floor(h);
        if (h < 1.0 / 6.0)
        {
            return v1 + (v2 - v1) * 6.0 * h;
        }
        else if (h < 1.0 / 2.0)
        {
            return v2;
        }
        else if (h < 2.0 / 3.0)
        {
            return v1 + (v2 - v1) * 6.0 * (2.0 / 3.0 - h);
        }
        return v1;
    }

    //Not currently used but left here for now
    function hslToRgb(h,s,l) {

        if (s < 1.0 / 256.0)
        {
            return Qt.rgba(1, 1, 1,1);
        }

        var v1;
        var v2;
        if (l < 0.5)
        {
            v2 = l * (1.0 + s);
        }
        else
        {
            v2 = (l + s) - (s * l);
        }

        v1 = 2.0 * l - v2;

        var d = 1.0 / 3.0;
        var r = hueToIntensity(v1, v2, h + d);
        var g = hueToIntensity(v1, v2, h);
        var b = hueToIntensity(v1, v2, h - d);
        return Qt.rgba(r, g, b,1);
    }

    function rgbToHsl(r, g, b){
        var max = Math.max(r, g, b), min = Math.min(r, g, b);
        var h, s, l = (max + min) / 2;

        if(max == min){
            h = s = 0; // ahuetic
        }else{
            var d = max - min;
            s = l > 0.5 ? d / (2 - max - min) : d / (max + min);
            switch(max){
                case r: h = (g - b) / d + (g < b ? 6 : 0); break;
                case g: h = (b - r) / d + 2; break;
                case b: h = (r - g) / d + 4; break;
            }
            h /= 6;
        }

        return [h, s, l];
    }

    RowLayout {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: defaultSpacing.minimumRowHeight * 17

        spacing: 0

        Item {
            id: leftColumn
            Layout.preferredHeight: defaultSpacing.minimumRowHeight * 17 - defaultSpacing.doubleMargin
            Layout.preferredWidth: defaultSpacing.minimumRowHeight * 13

            ColumnLayout {
                anchors.fill: parent
                anchors.leftMargin: defaultSpacing.standardMargin
                anchors.rightMargin: defaultSpacing.standardMargin
                spacing: 0

                Item {
                    Layout.fillWidth: true
                    Layout.preferredHeight: width

                    //Loads color picking control in left column depending on leftColumnMenu
                    Loader {
                        id: wheelLoader
                        anchors.fill: parent
                        sourceComponent: wheelComponent
                    }

                    Component {
                        //HSL Color Triangle/Wheel
                        id: wheelComponent
                        WGColorWheel {
                            id: colorWheel
                            showShortCuts: false

                            onCurrentColorChanged: {
                                if (wheelLoader.status == Loader.Ready)
                                {
                                    setColorHSLA(hue,saturation,lightness,basePanel.aValue)
                                }
                            }

                            Connections {
                                target: basePanel
                                onCurrentColorChanged: {
                                    colorWheel.updateHSL(hValue,sValue,lValue)
                                }
                            }

                            Component.onCompleted: {
                                colorWheel.updateHSL(hValue,sValue,lValue)
                            }
                        }
                    }
                    Component {
                        id: shadeComponent
                        //Hue & Lightness shade grid
                        WGShadeGrid {
                            anchors.centerIn: parent

                            hue: basePanel.hValue
                            saturation: basePanel.sValue
                            lightness: basePanel.lValue
                            alpha: basePanel.aValue

                            onUpdateColor: {
                                setColorHSLA(h,s,l,basePanel.aValue)
                            }

                            onHoveredColorChanged: {
                                if (swatchHovered)
                                {
                                    basePanel.tempColor = hoveredColor
                                }
                            }
                            onSwatchHoveredChanged: {
                                basePanel.tempColorActive = swatchHovered
                            }
                        }
                    }
                }

                Item {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }

                //Picks which control is visible in left column
                WGDropDownBox {
                    id: leftColumnMenu
                    Layout.fillWidth: true
                    Layout.preferredHeight: defaultSpacing.minimumRowHeight

                    model: ListModel {
                        ListElement { text: "HSL Triangle" }
                        ListElement { text: "Color Shades" }
                    }

                    onCurrentIndexChanged: {
                        if (currentIndex == 0)
                        {
                            wheelLoader.sourceComponent = wheelComponent
                        }
                        else if (currentIndex == 1)
                        {
                            wheelLoader.sourceComponent = shadeComponent
                        }
                    }
                }

                Item {
                    Layout.minimumHeight: defaultSpacing.standardMargin
                    Layout.fillWidth: true
                }

                //Customiseable color palette.
                //TODO make this use appwide persistent data
                Item {
                    id: colorPalette
                    Layout.fillWidth: true
                    Layout.preferredHeight: defaultSpacing.minimumRowHeight * 2 + defaultSpacing.standardMargin

                    signal updatePalette

                    Rectangle {
                        id: innerBorder
                        anchors.horizontalCenter: parent.horizontalCenter
                        height: parent.height - defaultSpacing.standardRadius
                        width: parent.width
                        color: palette.DarkColor

                        GridLayout {
                            id: paletteGrid
                            anchors.fill: parent
                            columnSpacing: 0
                            rowSpacing: 0

                            columns: 12
                            rows: 2

                            Repeater {
                                model: 23
                                Item {
                                    id: swatchBorder
                                    Layout.preferredWidth: Math.floor((innerBorder.width - defaultSpacing.doubleBorderSize) / 12)
                                    Layout.preferredHeight: width

                                    property color swatchColor: Qt.rgba(0,0,0,0)
                                    property bool containsColor: index < savedColors.length

                                    Connections {
                                        target: colorPalette
                                        onUpdatePalette: {
                                            containsColor = index < savedColors.length
                                            if (containsColor)
                                            {
                                                swatchColor = savedColors[index]
                                            }
                                            else
                                            {
                                                swatchColor = Qt.rgba(0,0,0,0)
                                            }
                                        }
                                    }

                                    Rectangle {
                                        id: swatch
                                        anchors.centerIn: parent
                                        height: parent.height - defaultSpacing.standardRadius
                                        width: height
                                        color: typeof swatchColor != "undefined" ? swatchColor : "transparent"
                                        border.width: containsColor ? 0 : 1
                                        border.color: containsColor ? "transparent" : palette.MidLightColor

                                        Image {
                                            source: "icons/bw_check_6x6.png"
                                            fillMode: Image.Tile
                                            anchors.fill: parent
                                            visible: containsColor
                                            z: -1
                                        }

                                        MouseArea {
                                            anchors.fill: parent
                                            cursorShape: containsColor ? Qt.PointingHandCursor : Qt.ArrowCursor
                                            enabled: containsColor
                                            acceptedButtons: Qt.LeftButton | Qt.RightButton
                                            onClicked: {
                                                // pick as new currentColor
                                                if (mouse.button == Qt.LeftButton)
                                                {
                                                    if (useAlpha)
                                                    {
                                                        setColorRGBA(swatchColor.r,swatchColor.g,swatchColor.b,swatchColor.a)
                                                    }
                                                    else
                                                    {
                                                        setColorRGBA(swatchColor.r,swatchColor.g,swatchColor.b,1.0)
                                                    }
                                                }
                                                // if Right Mouse button, delete color from palette
                                                else if (mouse.button == Qt.RightButton)
                                                {
                                                    savedColors.splice(index,1)
                                                    colorPalette.updatePalette()
                                                }

                                            }
                                        }
                                    }

                                    Component.onCompleted: {
                                        colorPalette.updatePalette()
                                    }
                                }
                            }

                            Item {
                                Layout.preferredWidth: Math.floor((innerBorder.width - defaultSpacing.doubleBorderSize) / 12)
                                Layout.preferredHeight: width

                                WGPushButton {
                                    anchors.centerIn: parent
                                    height: parent.height - defaultSpacing.standardRadius
                                    width: height
                                    iconSource: "icons/add_16x16.png"

                                    tooltip: "Add current color to palette."

                                    onClicked: {
                                        if (savedColors.length < 23)
                                        {
                                            savedColors.push(Qt.rgba(rValue,gValue,bValue,aValue))
                                        }
                                        else
                                        {
                                            savedColors.shift()
                                            savedColors.push(Qt.rgba(rValue,gValue,bValue,aValue))
                                        }
                                        colorPalette.updatePalette()
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        Item {
            id: rightColumn
            Layout.preferredHeight: defaultSpacing.minimumRowHeight * 17 - defaultSpacing.doubleMargin
            Layout.fillWidth: true

            // Preview squares column
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: defaultSpacing.standardMargin
                spacing: 0

                RowLayout {
                    Layout.fillWidth: true
                    Layout.preferredHeight: childrenRect.height

                    // Overlaid squares for comparing initialColor and currentColor
                    Item {
                        id: previewSquares
                        Layout.maximumHeight: defaultSpacing.minimumRowHeight * 4
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight * 4
                        Layout.preferredWidth: height
                        Layout.alignment: Qt.AlignTop | Qt.AlignHCenter

                        Item {
                            anchors.centerIn: parent
                            height: parent.height
                            width: height

                            Rectangle {
                                id: secondSquareBorder
                                height: Math.round(parent.height * 0.75)
                                width: height
                                color: palette.DarkColor
                                anchors.right: parent.right
                                anchors.bottom: parent.bottom
                            }

                            Rectangle {
                                height: Math.round(parent.height * 0.75)
                                width: height
                                color: palette.DarkColor

                                // Initial square. Can be clicked to reset currentColor to initialColor
                                Rectangle {
                                    id: initialPreviewSquare
                                    anchors.fill: parent
                                    anchors.margins: defaultSpacing.standardMargin
                                    width: height
                                    color: initialColor

                                    MouseArea {
                                        anchors.fill: parent
                                        cursorShape: Qt.PointingHandCursor
                                        onClicked: {
                                            var tempColor = rgbToHsl(initialColor.r,initialColor.g,initialColor.b)
                                            setColorHSLA(tempColor[0],tempColor[1],tempColor[2],basePanel.aValue)
                                        }
                                    }

                                    Image {
                                        source: "icons/bw_check_6x6.png"
                                        fillMode: Image.Tile
                                        anchors.fill: parent
                                        z: -1
                                    }
                                }
                            }

                            // currentColor preview square
                            Rectangle {
                                id: currentPreviewSquare
                                anchors.fill: secondSquareBorder
                                anchors.margins: defaultSpacing.standardMargin
                                color: tempColorActive ? tempColor : currentColor

                                z: 5

                                // Dummy mouseover to mask cursor change on inital square
                                MouseArea {
                                    anchors.fill: parent
                                    propagateComposedEvents: false
                                }

                                Image {
                                    source: "icons/bw_check_6x6.png"
                                    fillMode: Image.Tile
                                    anchors.fill: parent
                                    z: -1
                                }
                            }
                        }
                    }

                    // Pick from screen, hex values and alpha toggle column.
                    ColumnLayout {
                        Layout.fillWidth: true
                        Layout.maximumHeight: defaultSpacing.minimumRowHeight * 4
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight * 4

                        WGPushButton {
                            id: pickButton
                            Layout.fillWidth: true
                            Layout.preferredHeight: defaultSpacing.minimumRowHeight
                            text: "Pick from screen"
                            iconSource: "icons/dropper_16x16.png"
                            checkable: true

                            MouseArea {
                                anchors.fill: parent

                                onPressed: {
                                    tempColorActive = true
                                    pickButton.checked = true
                                    startObservingColor();
                                }

                                onReleased: {
                                    tempColorActive = false
                                    pickButton.checked = false

                                    var sampledColor = pixelColor;

                                    endObservingColor();

                                    setColorRGBA(sampledColor.x, sampledColor.y, sampledColor.z, 1)

                                    tempColor = currentColor
                                }
                            }

                            Connections {
                                target : self
                                onPixelColorChanged : {
                                    var sampledColor = pixelColor;
                                    tempColor = Qt.rgba(sampledColor.x, sampledColor.y, sampledColor.z, sampledColor.w)
                                }
                            }

                        }

                        WGLabel {
                            text: "(Press & hold to start, release to pick)"
                            enabled: false
                            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                        }

                        Item {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            Layout.preferredHeight: defaultSpacing.minimumRowHeight

                            WGPushButton {
                                id: alphaToggle
                                text: "Use Alpha"
                                checkable: true
                                checked: useAlpha
                                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                                Layout.fillWidth: true
                                Layout.preferredHeight: defaultSpacing.minimumRowHeight
                                onClicked: {
                                    useAlpha = !useAlpha
                                    if (!useAlpha)
                                    {
                                        basePanel.aValue = 1.0
                                    }
                                }
                            }

                            WGTextBox {
                                id: hexValue
                                Layout.preferredWidth: 105
                                Layout.preferredHeight: defaultSpacing.minimumRowHeight
                                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                                property color validatedColor
                                text: currentColor

                                validator: RegExpValidator {
                                    regExp: /^#([A-Fa-f0-9]{6}|[A-Fa-f0-9]{3})$/
                                }

                                onAccepted: {
                                    validatedColor = text
                                    setColorRGBA(validatedColor.r, validatedColor.g, validatedColor.b, 1)
                                }

                                Connections {
                                    target: basePanel
                                    onCurrentColorChanged: {
                                        hexValue.text = currentColor
                                    }
                                }
                            }
                        }
                    }
                }

                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    WGSeparator {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }

                WGHslSlider {
                    id: hslSlider
                    Layout.fillWidth: true
                    hVal: basePanel.hValue
                    sVal: basePanel.sValue
                    lVal: basePanel.lValue

                    Connections {
                        target: basePanel
                        onHValueChanged: {
                            hslSlider.hVal = basePanel.hValue
                        }
                        onSValueChanged: {
                            hslSlider.sVal = basePanel.sValue
                        }
                        onLValueChanged: {
                            hslSlider.lVal = basePanel.lValue
                        }
                    }

                    onHValChanged: {
                        if (basePanel.hValue != hVal)
                        {
                            basePanel.hValue = hVal
                        }
                    }
                    onSValChanged: {
                        if (basePanel.sValue != sVal)
                        {
                            basePanel.sValue = sVal
                        }
                    }
                    onLValChanged: {
                        if (basePanel.lValue != lVal)
                        {
                            basePanel.lValue = lVal
                        }
                    }
                }

                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    WGSeparator {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }

                WGRgbSlider {
                    id: rgbSlider
                    Layout.fillWidth: true
                    rVal: basePanel.rValue
                    gVal: basePanel.gValue
                    bVal: basePanel.bValue

                    Connections {
                        target: basePanel
                        onRValueChanged: {
                            rgbSlider.rVal = basePanel.rValue
                        }
                        onGValueChanged: {
                            rgbSlider.gVal = basePanel.gValue
                        }
                        onBValueChanged: {
                            rgbSlider.bVal = basePanel.bValue
                        }
                    }

                    onRValChanged: {
                        if (basePanel.rValue != rVal)
                        {
                            basePanel.rValue = rVal
                        }
                    }
                    onGValChanged: {
                        if (basePanel.gValue != gVal)
                        {
                            basePanel.gValue = gVal
                        }
                    }
                    onBValChanged: {
                        if (basePanel.bValue != bVal)
                        {
                            basePanel.bValue = bVal
                        }
                    }

                }

                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    WGSeparator {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    Layout.preferredHeight: defaultSpacing.minimumRowHeight
                    enabled: useAlpha

                    WGLabel {
                        text: "A:"
                        horizontalAlignment: Text.AlignRight
                        Layout.preferredWidth: defaultSpacing.doubleMargin
                    }

                    Connections {
                        target: basePanel
                        onAValueChanged: {
                            aSlider.changeValue(basePanel.aValue, 0)
                            aBox.value = basePanel.aValue
                        }
                    }

                    WGColorSlider {
                        id: aSlider
                        Layout.fillWidth: true
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight
                        minimumValue: 0
                        maximumValue: 1.0
                        stepSize: 0.001
                        colorData: {
                            if (useAlpha)
                            {
                                [Qt.hsla(basePanel.hValue,basePanel.sValue,basePanel.lValue,0), Qt.hsla(basePanel.hValue,basePanel.sValue,basePanel.lValue,1)]
                            }
                            else
                            {
                                [palette.MainWindowColor,palette.MainWindowColor]
                            }
                        }
                        positionData: [0, 1]
                        value: basePanel.aValue
                        linkColorsToHandles: false

                        onValueChanged: {
                            if (value != basePanel.aValue)
                            {
                                basePanel.aValue = value
                            }
                        }
                    }

                    WGNumberBox {
                        id: aBox
                        Layout.preferredWidth: 105
                        minimumValue: 0
                        maximumValue: 1.0
                        stepSize: 0.001
                        decimals: 10
                        value: basePanel.aValue

                        onValueChanged: {
                            if (value != basePanel.aValue)
                            {
                                basePanel.aValue = value
                            }
                        }
                    }
                }

                Item {
                    visible: showDialogButtons
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }

                RowLayout {
                    visible: showDialogButtons
                    Layout.fillWidth: true
                    Layout.preferredHeight: defaultSpacing.minimumRowHeight

                    WGPushButton {
                        Layout.fillWidth: true
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight
                        Layout.preferredWidth: rightColumn.width / 2
                        text: "Ok"

                        onClicked: {
                            basePanel.okClicked()
                        }
                    }

                    WGPushButton {
                        Layout.fillWidth: true
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight
                        Layout.preferredWidth: rightColumn.width / 2
                        text: "Cancel"

                        onClicked: {
                            basePanel.cancelClicked()
                        }
                    }
                }
            }
        }
    }
}
