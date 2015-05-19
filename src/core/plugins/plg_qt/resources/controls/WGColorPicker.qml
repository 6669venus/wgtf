import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1

//WIP Color picker
//This is more to allow editing of the themes on the fly than as a genuine color picker
//Known isses: Need to choose the Light style from the menu to make a proper light style
//HSL values don't update as much

Rectangle {
    id: mainFrame
    property color combinedColor : "#999999"
    property variant hslValues: []

    height: childrenRect.height

    color: "transparent"

    //javascript coverts RGB values (0 > rgb > 1) to HSL (0 > hsl > 1)
    function rgbToHsl(r, g, b){

        r /= 255;
        g /= 255;
        b /= 255;

        var max = Math.max(r, g, b);
        var min = Math.min(r, g, b);
        var h = (max + min) / 2;
        var s = (max + min) / 2;
        var l = (max + min) / 2;

        if(max == min){
            h = 0;
            s = 0; // achromatic
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

    onCombinedColorChanged: {
        if (mainWindowToggle.checked){

            palette.MainWindowColor = combinedColor

            if (palette.TextColor == "#ffffff"){
                palette.DarkHeaderColor = Qt.darker(palette.MainWindowColor, 1.25)
                palette.LightPanelColor = Qt.lighter(palette.MainWindowColor, 1.32)

                palette.LightShade = "#10FFFFFF"
                palette.LighterShade = "#20FFFFFF"
                palette.LightestShade = "#30FFFFFF"

                palette.DarkShade = "#20000000"
                palette.DarkerShade = "#40000000"
                palette.DarkestShade = "#60000000"

            } else if (palette.TextColor == "#000000"){
                palette.DarkHeaderColor = Qt.darker(palette.MainWindowColor, 1.5)
                palette.LightPanelColor = Qt.lighter(palette.MainWindowColor, 1.15)

                palette.LightShade = "#20FFFFFF"
                palette.LighterShade = "#50FFFFFF"
                palette.LightestShade = "#80FFFFFF"

                palette.DarkShade = "#20000000"
                palette.DarkerShade = "#35000000"
                palette.DarkestShade = "#50000000"
            }

            palette.MidDarkColor = Qt.darker(palette.MainWindowColor, 1.1)
            palette.MidLightColor = Qt.lighter(palette.MainWindowColor, 1.2)
            palette.DarkColor = Qt.darker(palette.MainWindowColor, 1.5)
        }

        if (highlightToggle.checked){
            palette.HighlightColor = combinedColor
            palette.HighlightShade = Qt.rgba(palette.HighlightColor.r,palette.HighlightColor.g,palette.HighlightColor.b,0.5)
        }
    }
    WGColumnLayout {
        id: colorPickerPanel
        rowSpacing: 5
        WGExpandingRowLayout {
            Layout.fillWidth: true
            spacing: 10
            WGColorButton {
                color_: combinedColor
                Layout.preferredWidth: implicitWidth
            }

            WGTextBox {
                id: colorHex
                text: combinedColor
                Layout.fillWidth: true
                Layout.preferredWidth: implicitWidth

                //Validator to only allow #XXXXXX values
                validator: RegExpValidator {
                    regExp: /^#([A-Fa-f0-9]{6}|[A-Fa-f0-9]{3})$/
                }
                onAccepted: {

                    if (rgbToggle.checked){

                        redSlider.value = Math.round(combinedColor.r * 255)
                        greenSlider.value = Math.round(combinedColor.g * 255)
                        blueSlider.value = Math.round(combinedColor.b * 255)

                        combinedColor = Qt.rgba(redSlider.value,greenSlider.value,blueSlider.value,1);

                    } else if (hslToggle.checked) {
                        hslValues = rgbToHsl(combinedColor.r,combinedColor.g,combinedColor.b);

                        redSlider.hue_ = Math.round(hslValues[0] * 360)
                        greenSlider.hue_ = redSlider.hue_
                        blueSlider.hue_ = redSlider.hue_

                        redSlider.sat_ = Math.round(hslValues[1] * 100)
                        greenSlider.sat_ = redSlider.sat_
                        blueSlider.sat_ = redSlider.sat_

                        redSlider.light_ = Math.round(hslValues[2] * 100)
                        greenSlider.light_ = redSlider.light_
                        blueSlider.light_ = redSlider.light_

                        redSlider.value = redSlider.hue_
                        greenSlider.value = greenSlider.sat_
                        blueSlider.value = blueSlider.light_

                        combinedColor = Qt.hsla(redSlider.value,greenSlider.value,blueSlider.value,1);
                    }
                }
            }
            ExclusiveGroup {
                id: colorGroup
            }
            WGRadioButton {
                id: rgbToggle
                text: "RGB"
                exclusiveGroup: colorGroup
                checked: true
                Layout.preferredWidth: implicitWidth
                onClicked: {
                    if (redSlider.channel_ != "r"){

                        redSlider.channel_ = "r"
                        greenSlider.channel_ = "g"
                        blueSlider.channel_ = "b"

                        redSlider.maximumValue = 255
                        greenSlider.maximumValue = 255
                        blueSlider.maximumValue = 255

                        redSlider.value = combinedColor.r * 255
                        greenSlider.value = combinedColor.g * 255
                        blueSlider.value = combinedColor.b * 255

                        redSlider.text = "R"
                        greenSlider.text = "G"
                        blueSlider.text = "B"
                    }
                }
            }
            WGRadioButton {
                id: hslToggle
                text: "HSL"
                exclusiveGroup: colorGroup
                Layout.preferredWidth: implicitWidth
                onClicked: {
                    if (redSlider.channel_ != "h") {
                        hslValues = rgbToHsl(redSlider.value,greenSlider.value,blueSlider.value);

                        redSlider.channel_ = "h"
                        greenSlider.channel_ = "s"
                        blueSlider.channel_ = "l"

                        redSlider.maximumValue = 360
                        greenSlider.maximumValue = 100
                        blueSlider.maximumValue = 100

                        redSlider.hue_ = Math.round(hslValues[0] * 360)
                        greenSlider.hue_ = redSlider.hue_
                        blueSlider.hue_ = redSlider.hue_

                        redSlider.sat_ = Math.round(hslValues[1] * 100)
                        greenSlider.sat_ = redSlider.sat_
                        blueSlider.sat_ = redSlider.sat_

                        redSlider.light_ = Math.round(hslValues[2] * 100)
                        greenSlider.light_ = redSlider.light_
                        blueSlider.light_ = redSlider.light_

                        redSlider.value = redSlider.hue_
                        greenSlider.value = greenSlider.sat_
                        blueSlider.value = blueSlider.light_

                        redSlider.text = "H"
                        greenSlider.text = "S"
                        blueSlider.text = "L"
                    }
                }
            }

            //resets color to black or white
            WGColorButton {
                color_: "#FFFFFF"
                Layout.preferredWidth: implicitWidth
                onClicked: {
                    if (rgbToggle.checked){
                        redSlider.value = redSlider.maximumValue
                        greenSlider.value = greenSlider.maximumValue
                        blueSlider.value = blueSlider.maximumValue
                    } else if (hslToggle.checked){
                        redSlider.value = redSlider.minimumValue
                        greenSlider.value = greenSlider.minimumValue
                        blueSlider.value = blueSlider.maximumValue
                    }
                }
            }

            WGColorButton {
                color_: "#000000"
                Layout.preferredWidth: implicitWidth
                onClicked: {
                    redSlider.value = redSlider.minimumValue
                    greenSlider.value = greenSlider.minimumValue
                    blueSlider.value = blueSlider.minimumValue
                }
            }

        }

        WGColorSlider {
            id: redSlider
            Layout.fillWidth: true
            labelBoxWidth: 10
            text: "R"
            color_: combinedColor
            channel_: "r"

            onValueChanged: {
                if (rgbToggle.checked) {
                    combinedColor = Qt.rgba(value/255,combinedColor.g,combinedColor.b,1);

                    redSlider.color_ = combinedColor
                    greenSlider.color_ = combinedColor
                    blueSlider.color_ = combinedColor

                } else if (hslToggle.checked) {

                    combinedColor = Qt.hsla(redSlider.hue_/360,greenSlider.sat_/100,blueSlider.light_/100,1)

                    redSlider.hue_ = value
                    greenSlider.hue_ = value
                    blueSlider.hue_ = value

                }
            }
        }

        WGColorSlider {
            id: greenSlider

            Layout.fillWidth: true
            labelBoxWidth: 10
            text: "G"
            color_: combinedColor
            channel_: "g"

            onValueChanged: {
                if (rgbToggle.checked) {
                    combinedColor = Qt.rgba(combinedColor.r,value/255,combinedColor.b,1);

                    redSlider.color_ = combinedColor
                    greenSlider.color_ = combinedColor
                    blueSlider.color_ = combinedColor

                } else if (hslToggle.checked) {
                    combinedColor = Qt.hsla(redSlider.hue_/360,sat_/100,blueSlider.light_/100,1)

                    redSlider.sat_ = value
                    greenSlider.sat_ = value
                    blueSlider.sat_ = value

                }

            }
        }

        WGColorSlider {
            id: blueSlider

            Layout.fillWidth: true
            labelBoxWidth: 10
            text: "B"
            color_: combinedColor
            channel_: "b"

            onValueChanged: {
                if (rgbToggle.checked) {
                    combinedColor = Qt.rgba(combinedColor.r,combinedColor.g,value/255,1);

                    redSlider.color_ = combinedColor
                    greenSlider.color_ = combinedColor
                    blueSlider.color_ = combinedColor

                } else if (hslToggle.checked) {
                    combinedColor = Qt.hsla(redSlider.hue_/360,greenSlider.sat_/100,light_/100,1)

                    redSlider.light_ = value
                    greenSlider.light_ = value
                    blueSlider.light_ = value

                }

            }
        }
        ExclusiveGroup {
            id: editColorGroup
        }

        Row {
            Layout.fillWidth: true
            spacing: 10

            WGLabel {
                text: "Edit Color:"
            }

            Rectangle {
                width: 30
                height: 5
                color: "transparent"
            }

            WGRadioButton{
                exclusiveGroup: editColorGroup
                checked: true
                text: "Nothing"
            }

            WGRadioButton{
                id: mainWindowToggle
                exclusiveGroup: editColorGroup
                text: "Main Window"
                onClicked: {
                    if(combinedColor != palette.MainWindowColor){
                        combinedColor = palette.MainWindowColor
                        if (rgbToggle.checked){
                            redSlider.value = combinedColor.r * 255
                            greenSlider.value = combinedColor.g * 255
                            blueSlider.value = combinedColor.b * 255
                        }
                    }
                }
            }
            WGRadioButton{
                id: highlightToggle
                exclusiveGroup: editColorGroup
                text: "Highlight"
                onClicked: {
                    if(combinedColor != palette.HighlightColor){
                        combinedColor = palette.HighlightColor
                        if (rgbToggle.checked){
                            redSlider.value = combinedColor.r * 255
                            greenSlider.value = combinedColor.g * 255
                            blueSlider.value = combinedColor.b * 255
                        }
                    }
                }
            }
        }
    }
}
