import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

import BWControls 1.0
import WGControls 1.0

// WIP Color Picker


Rectangle {
    id: basePanel
    objectName: "WGColorPicker"

    property color initialColor: Qt.hsla(1,1,1,1)
    property color currentColor: Qt.hsla(hValue,sValue,lValue,aValue)
    property color rgbColor: Qt.rgba(rValue,gValue,bValue,aValue)

    property var savedColors: ["#000000","#FFFFFF","#959595","#FF0000","#00FF00","#0000FF","#00FFFF","#FF00FF","#FFFF00"]

    property bool updateHSL: false
    property bool updateRGB: false

    property bool useAlpha: true

    property real hValue: 0
    property real sValue: 0
    property real lValue: 0

    property real rValue: 0
    property real gValue: 0
    property real bValue: 0

    property real aValue: 1

    color: palette.MainWindowColor

    signal okClicked()

    signal cancelClicked()

    onRgbColorChanged: {
        if (updateHSL)
        {
            updateRGB = false
            var tempColor = rgbToHsl(rValue,gValue,bValue)
            hValue = tempColor[0]
            sValue = tempColor[1]
            lValue = tempColor[2]
            updateRGB = true
        }
    }

    onCurrentColorChanged: {
        if (updateRGB)
        {
            updateHSL = false
            rValue = currentColor.r
            gValue = currentColor.g
            bValue = currentColor.b
            updateHSL = true
        }
    }

    onInitialColorChanged: {
        updateHSL = false
        updateRGB = false

        var tempColor = rgbToHsl(initialColor.r,initialColor.g,initialColor.b)

        hValue = tempColor[0]
        sValue = tempColor[1]
        lValue = tempColor[2]

        rValue = initialColor.r
        gValue = initialColor.g
        bValue = initialColor.b

        updateHSL = true
        updateRGB = true
    }

    Component.onCompleted: {
        var tempColor = rgbToHsl(initialColor.r,initialColor.g,initialColor.b)

        hValue = tempColor[0]
        sValue = tempColor[1]
        lValue = tempColor[2]

        rValue = initialColor.r
        gValue = initialColor.g
        bValue = initialColor.b

        updateHSL = true
        updateRGB = true
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

                    Loader {
                        id: wheelLoader
                        anchors.fill: parent
                        sourceComponent: wheelComponent
                    }

                    Component {
                        id: wheelComponent
                        WGColorWheel {
                            id: colorWheel
                            showShortCuts: false

                            onCurrentColorChanged: {
                                if (wheelLoader.status == Loader.Ready)
                                {
                                    basePanel.hValue = hue
                                    basePanel.sValue = saturation
                                    basePanel.lValue = lightness
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
                        WGShadeGrid {
                            anchors.centerIn: parent

                            hue: basePanel.hValue
                            saturation: basePanel.sValue
                            lightness: basePanel.lValue
                            alpha: basePanel.aValue

                            onUpdateColor: {
                                basePanel.hValue = h
                                basePanel.sValue = s
                                basePanel.lValue = l
                            }
                        }
                    }
                }

                Item {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }

                WGDropDownBox {
                    Layout.fillWidth: true
                    Layout.preferredHeight: defaultSpacing.minimumRowHeight
                    //enabled: false

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
                                                if (mouse.button == Qt.LeftButton)
                                                {
                                                    rValue = swatchColor.r
                                                    gValue = swatchColor.g
                                                    bValue = swatchColor.b
                                                    if (useAlpha)
                                                    {
                                                        aValue = swatchColor.a
                                                    }
                                                }
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

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: defaultSpacing.standardMargin
                spacing: 0

                RowLayout {
                    Layout.fillWidth: true
                    Layout.preferredHeight: childrenRect.height

                    Item {
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

                                Rectangle {
                                    anchors.fill: parent
                                    anchors.margins: defaultSpacing.standardMargin
                                    width: height
                                    color: initialColor

                                    MouseArea {
                                        anchors.fill: parent
                                        cursorShape: Qt.PointingHandCursor
                                        onClicked: {
                                            updateRGB = false
                                            var tempColor = rgbToHsl(initialColor.r,initialColor.g,initialColor.b)
                                            hValue = tempColor[0]
                                            sValue = tempColor[1]
                                            lValue = tempColor[2]
                                            updateRGB = true
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

                            Rectangle {
                                anchors.fill: secondSquareBorder
                                anchors.margins: defaultSpacing.standardMargin
                                color: currentColor

                                z: 5

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

                    ColumnLayout {
                        Layout.fillWidth: true
                        Layout.maximumHeight: defaultSpacing.minimumRowHeight * 4
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight * 4

                        WGPushButton {
                            Layout.fillWidth: true
                            Layout.preferredHeight: defaultSpacing.minimumRowHeight
                            text: "Pick from screen"
                            enabled: false
                            iconSource: "icons/pin_16x16.png"
                        }


                        RowLayout {
                            Layout.fillWidth: true
                            Layout.preferredHeight: defaultSpacing.minimumRowHeight

                            WGPushButton {
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
                                Layout.preferredWidth: 105
                                Layout.preferredHeight: defaultSpacing.minimumRowHeight
                                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                                property color validatedColor
                                text: currentColor
                                //readOnly: true

                                validator: RegExpValidator {
                                    regExp: /^#([A-Fa-f0-9]{6}|[A-Fa-f0-9]{3})$/
                                }

                                onAccepted: {
                                    validatedColor = text
                                    rValue = validatedColor.r
                                    gValue = validatedColor.g
                                    bValue = validatedColor.b
                                }
                            }
                        }

                        Item{
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                        }

                    }
                }


                Item {
                    Layout.fillWidth: true
                    Layout.preferredHeight: defaultSpacing.doubleMargin
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
                    Layout.preferredHeight: defaultSpacing.doubleMargin
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
                    Layout.preferredHeight: defaultSpacing.doubleMargin
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
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }

                RowLayout {
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
