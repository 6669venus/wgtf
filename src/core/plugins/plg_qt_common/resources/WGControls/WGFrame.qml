import QtQuick 2.3
import QtQuick.Layouts 1.1

/*!
    \brief Stripped back dark or light frame. Not expandable, not toggleable, no title.

    These will match WGSubPanel

    If at lvl 0 recommend:
    dark_: false
    shade_: false

    If at lvl 1 recommend:
    dark_: true
    shade_: false

    Use shade_: true sparingly for a subtle darker or lighter frame to group objects

Example:
\code{.js}
WGFrame {
    dark_: false
    shade_: true

    label_: "File Options:"

    childObject_:
    WGColumnLayout {

        WGCheckBox {
            text: "File Option 1"
        }
        WGCheckBox {
            text: "File Option 2"
        }
        WGCheckBox {
            enabled: false
            text: "Disabled File Option"
        }
    }
}
\endcode
*/

Rectangle {
    objectName: "WGFrame"

    /*! This property sets the frame colour to dark
        The default value is \c true
    */
    property bool dark_: true

    /*! This property alters the frame colour
        The default value is \c false
    */
    property bool shade_: false

    // TODO: This should be renamed and marked as internal by "__" prefix
    /*! \internal */
    property alias color_: frame.color

    /*! This property is used to define the buttons label when used in a WGFormLayout
        The default value is an empty string
    */
    //TODO: This should be renamed, it does not require "_"
    property string label_: ""

    property Component childObject_

    color: "transparent"

    height: content.height + defaultSpacing.doubleMargin + (defaultSpacing.topBottomMargin * 2)

    Layout.preferredHeight: content.height + defaultSpacing.doubleMargin + (defaultSpacing.topBottomMargin * 2)

    Layout.fillWidth: true

    Rectangle {
        id: frame
        color: {
            if (shade_)
            {
                if (dark_)
                {
                    palette.DarkShade
                }
                else
                {
                    palette.LightShade
                }
            }
            else
            {
                if (dark_)
                {
                    palette.MainWindowColor
                }
                else
                {
                    palette.LightPanelColor
                }
            }
        }

        radius: defaultSpacing.standardRadius

        height: parent.height - (defaultSpacing.topBottomMargin * 2)

        Layout.preferredHeight: parent.height - (defaultSpacing.topBottomMargin * 2)

        anchors {left: parent.left; right: parent.right}

        anchors.verticalCenter: parent.verticalCenter
    }

    Loader {
        id: content
        anchors {left: parent.left; right: parent.right}

        anchors.leftMargin: defaultSpacing.leftMargin
        anchors.rightMargin: defaultSpacing.rightMargin

        anchors.verticalCenter: parent.verticalCenter

        sourceComponent: childObject_

        z: 1
    }
}

