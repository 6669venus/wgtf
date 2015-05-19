import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0

//Expandable Subpanel/GroupBox
//Prebuilt alternatives: WGInternalPanel, WGGroupBox
//Will not work properly in a WGFormLayout, use WGColumnLayout instead.

//Consider using WGFrame for simple panels to group objects that don't need a title or to expand.



Rectangle {
    id: mainPanel

    //General Options
    property string text: ""            //Panel Title
    property string subText: ""         //panel sub title, used to convey data ownership

    property Component childObject_     //object inside the panel. Use a layout for multiple controls eg. WGColumnLayout, WGFormLayout
    property Component headerObject_    //object inside the header. Use a layout for multiple controls eg. WGRowLayout


    //Best for Large solid color panels
    property bool collapsible_ : true
    property bool hasIcon_ : true

    property bool expanded_ : true

    property string closedIcon_: "qrc:///icons/arrow_right_16x16"
    property string openIcon_: "qrc:///icons/arrow_down_16x16"

    /* Pinning probably belongs in the pimary parent panel
    property bool pinable_ : false
    property bool pinned_ : false

    property string pinIcon_: "qrc:///icons/pin_16x16"
    property string pinnedIcon_: "qrc:///icons/pinned_16x16"
    */

    property bool boldHeader_: true

    //sub header properties
    property bool italicSubHeader_: true
    property bool boldSubHeader_: false

    property bool transparentPanel_: false

    property color colorHeader_ : palette.DarkHeaderColor
    property color colorBody_ : palette.LightPanelColor

    //best for minor group box frames

    property bool hasSeparators_ : false    //Thin lines at top and bottom of frame
    property bool toggleable_ : false       //Make header a checkbox to enable/disable contents
    property alias checked_ : headerCheck.checked
    property alias exclusiveGroup: headerCheck.exclusiveGroup

    property alias b_Target: headerCheck.b_Target
    property alias b_Property: headerCheck.b_Property
    property alias b_Value: headerCheck.b_Value

    //change to true if contains a ScrollPanel or similar childObject which needs to obscure content
    property bool clipContents_: false

    //Recommend not changing these properties:

    property int contentLeftMargin_ : panelProps.leftMargin_
    property int contentRightMargin_ : panelProps.rightMargin_
    property int contentIndent_ : 0


    property bool finishedLoading_: false

    Layout.fillWidth: true

    color: "transparent"

    radius: panelProps.standardRadius_

    //if radius < 2, these panels look awful. This adds a bit more spacing.
    property int squareModifier: radius < 2 ? 8 : 0

    //Can have child panels indent further if set in global settings
    anchors.leftMargin: panelProps.childIndent_

    height: {
        if (expanded_){
            content.height + panelProps.rowHeight_ + (radius * 4) + (panelProps.topBottomMargin_ * 4) + squareModifier
        } else if (!expanded_){
            panelProps.rowHeight_ + (radius * 2) + (panelProps.topBottomMargin_ * 2) + (squareModifier / 2)
        }
    }

    Layout.preferredHeight: {
        if (expanded_){
            content.height + panelProps.rowHeight_ + (radius * 4) + (panelProps.topBottomMargin_ * 4) + squareModifier
        } else if (!expanded_){
            panelProps.rowHeight_ + (radius * 2) + (panelProps.topBottomMargin_ * 2) + (squareModifier / 2)
        }
    }

    //delay so panels don't animate when control is created
    Component.onCompleted: {
        animationDelay.start()
        if (transparentPanel_){
            colorHeader_ = "transparent"
            colorBody_ = "transparent"
            mainPanel.radius = 0
        }
    }

    Timer {
        id: animationDelay
        interval: 100
        onTriggered: {
            finishedLoading_ = true
        }
    }

    Behavior on Layout.preferredHeight{
        id: popAnimation
        enabled: finishedLoading_ && collapsible_
        NumberAnimation {
            duration: 120
            easing {
                type: Easing.OutCirc
                amplitude: 1.0
                period: 0.5
            }
        }
    }

    //bulge that appears in collapsed panels if not GlowStyle
    Rectangle {
        id: expandingOuterFrame
        radius: panelProps.halfRadius_

        color: palette.HighlightShade

        visible: collapsible_ && !palette.GlowStyle

        anchors.top: headerPanel.top
        anchors.bottom: headerPanel.bottom
        anchors.topMargin: mainPanel.radius + (squareModifier / 2)
        anchors.bottomMargin: mainPanel.radius + (squareModifier / 2)

        anchors.horizontalCenter: mainColor.horizontalCenter

        width: expanded_ ? mainColor.width - 6 : mainColor.width + 6 + (squareModifier)

        Component.onCompleted: {
            if(expanded_){
                expandingOuterFrame.color = "transparent"
            }
        }

        Behavior on width{
            id: expandAnimation
            enabled: false
            NumberAnimation {
                 duration: 120
                 easing {
                     type: Easing.OutCirc
                     amplitude: 1.0
                     period: 0.5
                 }

                 onRunningChanged: {
                     if (!running){
                         expandAnimation.enabled = false
                         if (expanded_){
                            expandingOuterFrame.color = "transparent"
                         }
                     } else {
                         expandingOuterFrame.color = palette.HighlightShade
                     }
                 }
            }
        }
    }

    //glow that appears in collapsed panels if GlowStyle
    Rectangle {
        id: glowingSideFrame
        radius: 10

        gradient: Gradient {
                 GradientStop { position: 0.0; color: "transparent" }
                 GradientStop { position: 0.5; color: expanded_ ? palette.HighlightShade : palette.HighlightColor }
                 GradientStop { position: 1.0; color: "transparent" }
             }

        visible: collapsible_ && palette.GlowStyle

        anchors.top: mainColor.top
        anchors.bottom: mainColor.bottom

        anchors.horizontalCenter: mainColor.horizontalCenter

        width: mainColor.width + 2

    }

    //main panel behind body panel that forms the header and footer
    Rectangle {
        id: mainColor
        radius: mainPanel.radius
        color: colorHeader_
        anchors.fill: parent
        anchors.topMargin: panelProps.topBottomMargin_
        anchors.bottomMargin: panelProps.topBottomMargin_
    }

    Rectangle {
        id: headerPanel
        anchors {left: parent.left; right: parent.right}

        color: "transparent"

        anchors.top: mainColor.top
        height: panelProps.rowHeight_ + mainPanel.radius * 2 + (squareModifier / 2)

        Rectangle {
            //mouse over panel and activeFocus border

            id: mouseHighlight
            color: "transparent"
            anchors.fill: parent
            anchors.margins: panelProps.doubleBorder_
            radius: panelProps.halfRadius_

            activeFocusOnTab: collapsible_

            border.width: panelProps.standardBorder_
            border.color: activeFocus && collapsible_ ? palette.HighlightShade : "transparent"

            Keys.onPressed: {
                         if (event.key == Qt.Key_Space && collapsible_){
                             if (expanded_ && collapsible_){
                                 expandAnimation.enabled = true
                                 content.opacity = 0
                                 expanded_ = false;
                             } else if (!expanded_ && collapsible_){
                                 expandAnimation.enabled = true
                                 expanded_ = true;
                                 content.visible = true
                                 content.opacity = 1;
                             }
                         }
                     }
        }

        Image {
            id: headerIcon
            visible: hasIcon_
            width: hasIcon_ ? sourceSize.width : 0
            source: expanded_ ? mainPanel.openIcon_ : mainPanel.closedIcon_
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: contentLeftMargin_
        }

        //mouse area that expands the panel. Doesn't overlap any controls in the header
        MouseArea {
            id: expandMouseArea
            anchors.left: parent.left
            // anchors.right: headerObject_ ? headerControl.left : (pinable_ ? pinButton.left : parent.right)
            anchors.right: headerObject_ ? headerControl.left : panelMenu.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            visible: collapsible_

            activeFocusOnTab: false

            hoverEnabled: true

            onEntered: {
                if (collapsible_){
                    mouseHighlight.color = palette.LighterShade
                }
            }

            onExited: {
                if (collapsible_){
                    mouseHighlight.color = "transparent"
                }
            }

            onClicked: {
                if (expanded_ && collapsible_){
                    expandAnimation.enabled = true
                    content.opacity = 0
                    expanded_ = false;
                } else if (!expanded_ && collapsible_){
                    expandAnimation.enabled = true
                    expanded_ = true;
                    content.visible = true
                    content.opacity = 1;
                }
            }
        }

        //Panel title text or checkbox if toggleable
        Rectangle {
            id: headerBox
            anchors.left: headerIcon.right
            anchors.leftMargin: collapsible_ ? panelProps.leftMargin_ : 0
            anchors.verticalCenter: parent.verticalCenter
            width: childrenRect.width
            color: "transparent"
            Text {
                id: headerLabel
                font.bold: boldHeader_
                renderType: Text.NativeRendering
                color: palette.HighlightTextColor
                anchors.verticalCenter: parent.verticalCenter
                text: mainPanel.text
                visible: toggleable_ ? false : true
            }
            //SubPanel secondary title
            Text {
                id: headerSubLabel
                font.bold: boldSubHeader_
                font.italic: italicSubHeader_
                renderType: Text.NativeRendering
                color: palette.HighlightTextColor
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: headerLabel.right
                text: (mainPanel.subText == "") ? "" : " - " + mainPanel.subText
                visible: toggleable_ ? false : true
            }

            WGCheckBox {
                id: headerCheck
                text: mainPanel.text
                visible: toggleable_ ? true : false
                width: toggleable_ ? implicitWidth : 0
                checked: true
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        //separators only for group boxes
        WGSeparator {
            vertical_: false

            visible: hasSeparators_

            anchors.left: headerBox.right
            anchors.right: panelMenu.left

            anchors.leftMargin: panelProps.leftMargin_
            anchors.rightMargin: panelProps.standardMargin_

            anchors.verticalCenter: parent.verticalCenter
        }

        //controls that can be placed in the header itself
        Loader {
            id: headerControl

            anchors.left: headerBox.right
            anchors.right: panelMenu.left
            anchors.verticalCenter: parent.verticalCenter

            anchors.leftMargin: panelProps.leftMargin_
            anchors.rightMargin: contentRightMargin_
            enabled: headerCheck.checked

            sourceComponent: headerObject_
        }

        WGToolButton {
            id: panelMenu
            anchors.right: parent.right

            anchors.verticalCenter: parent.verticalCenter

            anchors.leftMargin: panelProps.leftMargin_
            anchors.rightMargin: panelProps.standardMargin_

            iconSource: "qrc:///icons/menu_16x16"

            menu: Menu{
                MenuItem {
                    text: "Copy Panel Data"
                    enabled: false
                }
                MenuItem {
                    text: "Paste Panel Data"
                    enabled: false
                }
            }
        }
    }

    //loads child object.
    //Highly recommended first child object is a WGColumnLayout or similar
    Loader {
        id: content

        clip: clipContents_

        anchors {left: parent.left; right: parent.right}

        anchors.top: headerPanel.bottom
        anchors.topMargin: panelProps.topBottomMargin_
        z: 1

        anchors.leftMargin: contentLeftMargin_ + contentIndent_
        anchors.rightMargin: contentRightMargin_

        sourceComponent: childObject_

        enabled: (headerCheck.checked && expanded_)

        onOpacityChanged: {
            if(opacity == 0){
                visible = false
            }
        }

        Component.onCompleted: {
            if (!expanded_){
                visible = false
                opacity = 0
            }
        }

        Behavior on opacity{
            id: fadeAnimation
            enabled: finishedLoading_ && collapsible_
            NumberAnimation {
                 duration: 120
                 easing {
                     type: Easing.OutCirc
                     amplitude: 1.0
                     period: 0.5
                 }
            }
        }
    }

    //panel that contains the content.
    Rectangle {
        id: bodyPanel
        anchors {left: parent.left; right: parent.right}

        color: parent.colorBody_

        anchors.top: headerPanel.bottom

        height: parent.expanded_ ? content.height + panelProps.doubleMargin_ - (squareModifier / 2): 0

        Behavior on height{
            id: bodyPopAnimation
            enabled: finishedLoading_ && collapsible_
            NumberAnimation {
                 duration: 120
                 easing {
                     type: Easing.OutCirc
                     amplitude: 1.0
                     period: 0.5
                 }
            }
        }
    }

    //separators only for group boxes
    WGSeparator {
        vertical_: false

        visible: hasSeparators_

        anchors.left: parent.left
        anchors.right: parent.right

        anchors.leftMargin: contentLeftMargin_
        anchors.rightMargin: contentRightMargin_

        anchors.top: content.bottom
        anchors.topMargin: panelProps.topBottomMargin_
    }
}
