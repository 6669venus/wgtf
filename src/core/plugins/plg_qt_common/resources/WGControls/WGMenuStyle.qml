import QtQuick 2.3
import QtQuick.Controls.Styles 1.2

/*! \brief Provides custom styling for WGMenu.*/

MenuStyle {
    objectName: "WGMenuStyle"

    frame: Rectangle {
        color: palette.DarkHeaderColor
        WGSeparator {
            vertical_: true
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: defaultSpacing.minimumRowHeight
            height: parent.height - defaultSpacing.doubleBorderSize
        }
    }

    itemDelegate.background: Rectangle {
        color: "transparent"
        height: defaultSpacing.minimumRowHeight
        WGHighlightFrame {
            anchors.fill: parent
            visible: styleData.selected
        }
    }

    itemDelegate.label: WGLabel {
        text: "   " + styleData.text
        color: {
            if (styleData.enabled && styleData.selected)
            {
                palette.HighlightTextColor
            }
            else if (styleData.enabled && !styleData.selected)
            {
                palette.TextColor
            }
            else if (!styleData.enabled)
            {
                palette.DisabledTextColor
            }
        }
    }

    scrollIndicator: Text {
        text: styleData.scrollerDirection ==  Qt.DownArrow ? "\uF036" : "\uF035"
        font.family : "Marlett"
        color: palette.TextColor
        renderType: Text.NativeRendering
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }

    itemDelegate.submenuIndicator: Text {
        text: "\uF038"
        font.family : "Marlett"
        color: {
            if (styleData.enabled && styleData.selected)
            {
                palette.HighlightTextColor
            }
            else if (styleData.enabled && !styleData.selected)
            {
                palette.TextColor
            }
            else if (!styleData.enabled)
            {
                palette.DisabledTextColor
            }
        }
        renderType: Text.NativeRendering
        verticalAlignment: Text.AlignVCenter
        y: 3
    }

    itemDelegate.checkmarkIndicator: WGTextBoxFrame {
        id: checkboxFrame
        implicitWidth: 14
        implicitHeight: 14

        color: styleData.enabled ? palette.TextBoxColor : "transparent"

        border.color: styleData.enabled ? palette.DarkestShade : palette.DarkerShade

        Rectangle {
            visible: styleData.checked
            color: {
                if (styleData.enabled && styleData.checked)
                {
                    palette.HighlightColor
                }
                else if (styleData.enabled && !styleData.checked)
                {
                    palette.HighlightShade
                }
                else if (!styleData.enabled)
                {
                    palette.LightShade
                }
            }
            radius: defaultSpacing.halfRadius
            anchors.fill: parent
            anchors.margins: styleData.checked ? 1 : 2 //smaller dull blue square if partially checked

            Text {
                id : tickMark
                color : styleData.enabled ? palette.HighlightTextColor : palette.LightestShade
                font.family : "Marlett"
                font.pixelSize: checkboxFrame.height + defaultSpacing.standardRadius
                renderType: Text.NativeRendering
                text : "\uF061"
                visible : styleData.checked //invisible if partially checked
                anchors.fill: parent
                anchors.leftMargin: defaultSpacing.halfRadius
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }

    separator: Rectangle {
        color: "transparent"
        height: defaultSpacing.separatorWidth + defaultSpacing.doubleBorderSize

        WGSeparator {
            vertical_: false
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.left: parent.left
            anchors.leftMargin: defaultSpacing.minimumRowHeight + (defaultSpacing.separatorWidth / 2)
        }
    }
}
