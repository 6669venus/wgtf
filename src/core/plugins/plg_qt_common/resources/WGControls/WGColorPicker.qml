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

    property bool updateHSL: false
    property bool updateRGB: false

    property bool showHSL: true
    property bool showRGB: true
    property bool showAlpha: true

    property real hValue: 0
    property real sValue: 0
    property real lValue: 0

    property real rValue: 0
    property real gValue: 0
    property real bValue: 0

    property real aValue: 1

    color: palette.MainWindowColor

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
            h = s = 0; // achromatic
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

    Item {
        anchors.fill: parent
        anchors.margins: defaultSpacing.standardMargin
        Item {
            id: topBar
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right

            height: defaultSpacing.minimumRowHeight + defaultSpacing.doubleMargin

            RowLayout {
                id: topBarLeft
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                width: parent.width * 0.5

                Item {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }
            }

            RowLayout {
                id: topBarRight
                anchors.left: topBarLeft.right
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom

                WGPushButton {
                    text: "HSL"
                    checkable: true
                    checked: showHSL
                    onClicked: {
                        showHSL = !showHSL
                    }
                }
                WGPushButton {
                    text: "RGB"
                    checkable: true
                    checked: showRGB
                    onClicked: {
                        showRGB = !showRGB
                    }
                }
                WGPushButton {
                    text: "Alpha"
                    checkable: true
                    checked: showAlpha
                    onClicked: {
                        showAlpha = !showAlpha
                    }
                }

                Item {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }
            }
        }

        Item {
            id: leftColumn
            anchors.top: topBar.bottom
            anchors.left: parent.left
            anchors.bottom: parent.bottom

            width: parent.width * 0.5


            ColumnLayout {
                anchors.fill: parent

                Item {
                    Layout.fillWidth: true
                    Layout.preferredHeight: width

                    WGColorWheel {
                        id: colorWheel
                        anchors.fill: parent
                        anchors.margins: defaultSpacing.standardMargin

                        onCurrentColorChanged: {
                            basePanel.hValue = chroma
                            basePanel.sValue = saturation
                            basePanel.lValue = lightness
                        }

                        Connections {
                            target: basePanel
                            onCurrentColorChanged: {
                                colorWheel.updateHSL(hValue,sValue,lValue)
                            }
                        }
                    }
                }

                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    Item {
                        id: leftSubPanel
                        width: parent.width / 2
                        height: parent.height

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: defaultSpacing.standardMargin
                            WGPushButton {
                                Layout.fillWidth: true
                                Layout.preferredHeight: defaultSpacing.minimumRowHeight
                                text: "Pick from screen"
                                iconSource: "icons/pin_16x16.png"
                            }

                            WGTextBox {
                                Layout.fillWidth: true
                                Layout.preferredHeight: defaultSpacing.minimumRowHeight
                                text: currentColor
                                readOnly: true
                            }

                            Item {
                                Layout.fillHeight: true
                            }
                            WGPushButton {
                                Layout.fillWidth: true
                                Layout.preferredHeight: defaultSpacing.minimumRowHeight
                                text: "Ok"
                            }
                            WGPushButton {
                                Layout.fillWidth: true
                                Layout.preferredHeight: defaultSpacing.minimumRowHeight
                                text: "Cancel"
                            }
                        }
                    }

                    Item {
                        id: rightSubPanel
                        width: parent.width / 2
                        height: parent.height
                        anchors.left: leftSubPanel.right

                        Item {
                            anchors.centerIn: parent
                            height: parent.height - defaultSpacing.doubleMargin
                            width: height

                            Rectangle {
                                id: secondSquareBorder
                                height: parent.height * 0.75
                                width: height
                                color: palette.DarkColor
                                anchors.right: parent.right
                                anchors.bottom: parent.bottom
                            }

                            Rectangle {
                                height: parent.height * 0.75
                                width: height
                                color: palette.DarkColor

                                Rectangle {
                                    anchors.fill: parent
                                    anchors.margins: defaultSpacing.standardMargin
                                    width: height
                                    color: initialColor

                                    MouseArea {
                                        anchors.fill: parent
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
                }
            }
        }
        Item {
            id: rightColumn
            anchors.top: topBar.bottom
            anchors.left: leftColumn.right
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.leftMargin: defaultSpacing.standardMargin

            ColumnLayout {
                anchors.fill: parent

                spacing: 0

                Connections {
                    target: basePanel
                    onHValueChanged: {
                        hSlider.changeValue(basePanel.hValue, 0)
                        hBox.value = basePanel.hValue
                    }
                    onSValueChanged: {
                        sSlider.changeValue(basePanel.sValue, 0)
                        sBox.value = basePanel.sValue
                    }
                    onLValueChanged: {
                        lSlider.changeValue(basePanel.lValue, 0)
                        lBox.value = basePanel.lValue
                    }
                    onRValueChanged: {
                        rSlider.changeValue(basePanel.rValue, 0)
                        rBox.value = basePanel.rValue
                    }
                    onGValueChanged: {
                        gSlider.changeValue(basePanel.gValue, 0)
                        gBox.value = basePanel.gValue
                    }
                    onBValueChanged: {
                        bSlider.changeValue(basePanel.bValue, 0)
                        bBox.value = basePanel.bValue
                    }
                    onAValueChanged: {
                        aSlider.changeValue(basePanel.aValue, 0)
                        aBox.value = basePanel.aValue
                    }
                }

                ColumnLayout {
                    id: hslSliders
                    Layout.fillWidth: true
                    visible: showHSL

                    WGColorSlider {
                        id: hSlider
                        Layout.fillWidth: true
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight
                        minimumValue: 0
                        maximumValue: 1.0
                        stepSize: 0.001
                        colorData: [Qt.rgba(1,0,0,1), Qt.rgba(1,1,0,1), Qt.rgba(0,1,0,1), Qt.rgba(0,1,1,1), Qt.rgba(0,0,1,1), Qt.rgba(1,0,1,1), Qt.rgba(1,0,0,1)]
                        positionData: [0, 0.167,0.333,0.5,0.667,0.833,1]
                        value: basePanel.hValue
                        linkColorsToHandles: false

                        onValueChanged: {
                            if (value != basePanel.hValue)
                            {
                                basePanel.hValue = value
                            }
                        }
                    }
                    RowLayout {
                        Layout.fillWidth: true
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight

                        WGLabel {
                            text: "H: "
                            horizontalAlignment: Text.AlignRight
                        }

                        WGNumberBox {
                            id: hBox
                            Layout.fillWidth: true
                            minimumValue: 0
                            maximumValue: 1.0
                            stepSize: 0.001
                            decimals: 10
                            value: basePanel.hValue
                            onValueChanged: {
                                if (value != basePanel.hValue)
                                {
                                    basePanel.hValue = value
                                }
                            }
                        }
                    }

                    WGColorSlider {
                        id: sSlider
                        Layout.fillWidth: true
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight
                        minimumValue: 0
                        maximumValue: 1.0
                        stepSize: 0.001
                        colorData: [Qt.hsla(hValue,0,lValue,1), Qt.hsla(hValue,1,lValue,1)]
                        positionData: [0, 1]
                        value: basePanel.sValue
                        linkColorsToHandles: false

                        onValueChanged: {
                            if (value != basePanel.sValue)
                            {
                                basePanel.sValue = value
                            }
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight

                        WGLabel {
                            text: "S: "
                            horizontalAlignment: Text.AlignRight
                        }

                        WGNumberBox {
                            id: sBox
                            Layout.fillWidth: true
                            minimumValue: 0
                            maximumValue: 1.0
                            stepSize: 0.001
                            decimals: 10
                            value: basePanel.sValue
                            onValueChanged: {
                                if (value != basePanel.sValue)
                                {
                                    basePanel.sValue = value
                                }
                            }
                        }
                    }

                    WGColorSlider {
                        id: lSlider
                        Layout.fillWidth: true
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight
                        minimumValue: 0
                        maximumValue: 1.0
                        stepSize: 0.001
                        colorData: [Qt.hsla(hValue,sValue,0,1), Qt.hsla(hValue,sValue,0.5,1),Qt.hsla(hValue,sValue,1,1)]
                        positionData: [0,0.5, 1]
                        value: basePanel.lValue
                        linkColorsToHandles: false

                        onValueChanged: {
                            if (value != basePanel.lValue)
                            {
                                basePanel.lValue = value
                            }
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight

                        WGLabel {
                            text: "L: "
                            horizontalAlignment: Text.AlignRight
                        }

                        WGNumberBox {
                            id: lBox
                            Layout.fillWidth: true
                            minimumValue: 0
                            maximumValue: 1.0
                            stepSize: 0.001
                            decimals: 10
                            value: basePanel.lValue
                            onValueChanged: {
                                if (value != basePanel.lValue)
                                {
                                    basePanel.lValue = value
                                }
                            }
                        }
                    }
                }

                Item {
                    visible: (showHSL && showRGB) || (showHSL && showAlpha && !showRGB)
                    Layout.fillWidth: true
                    Layout.fillHeight: if (showHSL && showRGB && showAlpha) {true} else {false}
                    Layout.preferredHeight: if (showHSL && showRGB && showAlpha) {-1} else {defaultSpacing.minimumRowHeight}
                }

                ColumnLayout {
                    id: rgbSliders
                    Layout.fillWidth: true
                    visible: showRGB

                    WGColorSlider {
                        id: rSlider
                        Layout.fillWidth: true
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight
                        minimumValue: 0
                        maximumValue: 1.0
                        stepSize: 0.001
                        colorData: [Qt.rgba(0,basePanel.gValue,basePanel.bValue,1), Qt.rgba(1,basePanel.gValue,basePanel.bValue,1)]
                        positionData: [0, 1]
                        value: basePanel.rValue
                        linkColorsToHandles: false

                        onValueChanged: {
                            if (value != basePanel.rValue)
                            {
                                basePanel.rValue = value
                            }
                        }
                    }
                    RowLayout {
                        Layout.fillWidth: true
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight

                        WGLabel {
                            text: "R: "
                            horizontalAlignment: Text.AlignRight
                        }

                        WGNumberBox {
                            id: rBox
                            Layout.fillWidth: true
                            minimumValue: 0
                            maximumValue: 1.0
                            stepSize: 0.001
                            decimals: 10
                            value: basePanel.rValue
                            onValueChanged: {
                                if (value != basePanel.rValue)
                                {
                                    basePanel.rValue = value
                                }
                            }
                        }
                    }

                    WGColorSlider {
                        id: gSlider
                        Layout.fillWidth: true
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight
                        minimumValue: 0
                        maximumValue: 1.0
                        stepSize: 0.001
                        colorData: [Qt.rgba(basePanel.rValue,0,basePanel.bValue,1), Qt.rgba(basePanel.rValue,1,basePanel.bValue,1)]
                        positionData: [0, 1]
                        value: basePanel.gValue
                        linkColorsToHandles: false

                        onValueChanged: {
                            if (value != basePanel.gValue)
                            {
                                basePanel.gValue = value
                            }
                        }
                    }
                    RowLayout {
                        Layout.fillWidth: true
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight

                        WGLabel {
                            text: "G: "
                            horizontalAlignment: Text.AlignRight
                        }

                        WGNumberBox {
                            id: gBox
                            Layout.fillWidth: true
                            minimumValue: 0
                            maximumValue: 1.0
                            stepSize: 0.001
                            decimals: 10
                            value: basePanel.gValue
                            onValueChanged: {
                                if (value != basePanel.gValue)
                                {
                                    basePanel.gValue = value
                                }
                            }
                        }
                    }

                    WGColorSlider {
                        id: bSlider
                        Layout.fillWidth: true
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight
                        minimumValue: 0
                        maximumValue: 1.0
                        stepSize: 0.001
                        colorData: [Qt.rgba(basePanel.rValue,basePanel.gValue,0,1), Qt.rgba(basePanel.rValue,basePanel.gValue,1,1)]
                        positionData: [0, 1]
                        value: basePanel.bValue
                        linkColorsToHandles: false

                        onValueChanged: {
                            if (value != basePanel.bValue)
                            {
                                basePanel.bValue = value
                            }
                        }
                    }
                    RowLayout {
                        Layout.fillWidth: true
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight

                        WGLabel {
                            text: "B: "
                            horizontalAlignment: Text.AlignRight
                        }

                        WGNumberBox {
                            id: bBox
                            Layout.fillWidth: true
                            minimumValue: 0
                            maximumValue: 1.0
                            stepSize: 0.001
                            decimals: 10
                            value: basePanel.bValue
                            onValueChanged: {
                                if (value != basePanel.bValue)
                                {
                                    basePanel.bValue = value
                                }
                            }
                        }
                    }

                }

                Item {
                    visible: (showRGB && showAlpha)
                    Layout.fillWidth: true
                    Layout.fillHeight: if (showHSL && showRGB && showAlpha) {true} else {false}
                    Layout.preferredHeight: if (showHSL && showRGB && showAlpha) {-1} else {defaultSpacing.minimumRowHeight}
                }

                ColumnLayout {
                    id: alphaSlider
                    Layout.fillWidth: true
                    visible: showAlpha

                    WGColorSlider {
                        id: aSlider
                        Layout.fillWidth: true
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight
                        minimumValue: 0
                        maximumValue: 1.0
                        stepSize: 0.001
                        colorData: [Qt.hsla(basePanel.hValue,basePanel.sValue,basePanel.lValue,0), Qt.hsla(basePanel.hValue,basePanel.sValue,basePanel.lValue,1)]
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

                    RowLayout {
                        Layout.fillWidth: true
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight

                        WGLabel {
                            text: "A: "
                            horizontalAlignment: Text.AlignRight
                        }

                        WGNumberBox {
                            id: aBox
                            Layout.fillWidth: true
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
                }

                Item {
                    visible: if (showHSL && showRGB && showAlpha) {false} else {true}
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }
            }
        }
    }
}
