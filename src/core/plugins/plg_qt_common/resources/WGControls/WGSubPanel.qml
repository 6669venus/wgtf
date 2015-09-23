import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

/*!
 \brief An Expandable Subpanel/GroupBox
 Prebuilt alternatives: WGInternalPanel, WGGroupBox.
 Will not work properly in a WGFormLayout, use WGColumnLayout instead.
 Consider using WGFrame for simple panels to group objects that don't need a title or to expand.

Example:
\code{.js}
WGSubPanel {
    text: "Example Title"
    childObject_ :
        WGColumnLayout {
            WGLodSlider {
                Layout.fillWidth: true
            }
    }
}
\endcode
*/

Rectangle {
    id: mainPanel
    objectName: "WGSubPanel"

    /*! This property holds the panel title text
        The default value is an empty string
    */
    property string text: ""

    /*!
        This property defines what objects are inside the panel.
        Use a layout for multiple controls eg. WGColumnLayout, WGFormLayout.
    */
    //TODO: This should be renamed, it does not require "_"
    property Component childObject_

    /*!
        This property defines what objects might appear in the header of the sub panel.
        Use a layout for multiple controls eg. WGRowLayout
        Current designs have not included this functionality.
    */
    //TODO: This should be renamed, it does not require "_"
    property Component headerObject_

    /*! This property toggles holds a link to the text inside the header so it can be attached to a copyable */
    //TODO: This should be an internal control and should be marked as private by "__" prefix
    property QtObject headerLabelObject_: headerLabel

    /*! This property defines whether a panel should be collapsible by the user
        The default value is \c true
    */
    //TODO: This should be renamed, it does not require "_"
    property bool collapsible_ : true

    /*! This property determines whether a panel has an icon in its title
        The default value is \c true
    */
    //TODO: This should be renamed, it does not require "_"
    property bool hasIcon_ : true

    /*! This property determines if a panel is expanded by default
        The default value is \c true
    */
    //TODO: This should be renamed, it does not require "_"
    property bool expanded_ : true

    /*! This property defines the location of the icon displayed when a panel is closed */
    //TODO: This should be renamed and marked as internal by "__" prefix
    property string closedIcon_: "qrc:///icons/arrow_right_16x16"

    /*! This property defines the location of the icon displayed when a panel is open */
    //TODO: This should be renamed and marked as internal by "__" prefix
    property string openIcon_: "qrc:///icons/arrow_down_16x16"

    /*! This property determines if the sub panel header will be in bold text
        The default value is \c true
    */
    //TODO: This should be renamed, it does not require "_"
    property bool boldHeader_: true

    //sub header properties

    /*! This property toggles holds defines panel sub title, used to convey data ownership.
        For example, a Property panel may be displaying a chicken's properties. text would be "property", subtext would be "chicken".
        The default value is an empty string
    */
    property string subText: ""

    /*! This property toggles determines if the sub header will be in italic text.
        The default value is \c true
    */
    //TODO: This should be renamed, it does not require "_"
    property bool italicSubHeader_: true

    /*! This property toggles determines if the sub header will be in bold text.
        The default value is \c false
    */
    //TODO: This should be renamed, it does not require "_"
    property bool boldSubHeader_: false

    /*! This property toggles the visibility of the background frame that contains the sub panels contents.
        The default value is \c true
    //TODO: When set true this creates undesirable UI unless used in conjuction with collapsible_:false.
    //This property needs to be made internal and a new sub class (WGStaticPanel) made with default transparentPanel:true and collapsible_:false
    */property bool transparentPanel_: false

    /*! This property determines the colour of the panel header.*/
    //TODO: This should be renamed, it does not require "_"
    property color colorHeader_ : palette.DarkHeaderColor

    /*! This property determines the colour of the panel body.*/
    //TODO: This should be renamed, it does not require "_"
    property color colorBody_ : palette.LightPanelColor

    //best for minor group box frames

    /*! This property toggles the visibility of thin lines at the top and bottom of the frame
        The default value is \c false
    */
    //TODO The separator in the title does not work well when the panel has headerObject_'s.
    //If headerObjects are going to be used this should be fixed.
    property bool hasSeparators_ : false

    /*! This property adds a checkbox to a panels title bar that enables/disables the panel contents.
        The default value is \c false
    */
    //TODO: This should be renamed, it does not require "_"
    property bool toggleable_ : false

    /*! This property sets the checked state of the header checkbox enabled by toggleable_:true */
    //TODO: This should be renamed, it does not require "_"
    property alias checked_ : headerCheck.checked

    //TODO: This should be renamed and marked as internal by "__" prefix
    property alias exclusiveGroup: headerCheck.exclusiveGroup

    /*! This property holds the target control's id to be bound to this controls b_Value
        The default settings are designed to bind Header Objects to the duplicate control within the panel*/
    //TODO: This should be renamed and marked as internal by "__" prefix
    property alias b_Target: headerCheck.b_Target

    /*! This property determines b_Target's property which is to be bound to this controls b_Value
        The default settings are designed to bind Header Objects to the duplicate control within the panel
    */
    //TODO: This should be renamed and marked as internal by "__" prefix
    property alias b_Property: headerCheck.b_Property

    /*! This property determines this control's value which will drive b_Target's b_Property
        The default settings are designed to bind Header Objects to the duplicate control within the panel
    */
    //TODO: This should be renamed and marked as internal by "__" prefix
    property alias b_Value: headerCheck.b_Value

    /*! This property will clip child components such as WGScrollPanel or similar which may extend outside the bounds of the scroll panel
        The default value is \c false
    */
    //TODO: This should be renamed, it does not require "_"
    property bool clipContents_: false

    //Recommend not changing these properties:

    //TODO: This should be renamed and marked as internal by "__" prefix
    /*! \internal */
    property int contentLeftMargin_ : defaultSpacing.leftMargin

    //TODO: This should be renamed and marked as internal by "__" prefix
    /*! \internal */
    property int contentRightMargin_ : defaultSpacing.rightMargin

    //TODO: This should be renamed and marked as internal by "__" prefix
    /*! \internal */
    property int contentIndent_ : 0

    //TODO: This should be renamed and marked as internal by "__" prefix
    /*! \internal */
    property bool finishedLoading_: false

    //if radius < 2, these panels look awful. This adds a bit more spacing.
    //TODO: This should be renamed and marked as internal by "__" prefix
    /*! \internal */
    property int squareModifier: radius < 2 ? 8 : 0

    Layout.fillWidth: true

    color: "transparent"

    radius: defaultSpacing.standardRadius

    //Can have child panels indent further if set in global settings
    anchors.leftMargin: defaultSpacing.childIndentation

    height: {
        if (expanded_)
        {
            content.height + defaultSpacing.minimumRowHeight + (radius * 4) + (defaultSpacing.topBottomMargin * 4) + squareModifier
        }
        else if (!expanded_)
        {
            defaultSpacing.minimumRowHeight + (radius * 2) + (defaultSpacing.topBottomMargin * 2) + (squareModifier / 2)
        }
    }

    Layout.preferredHeight: {
        if (expanded_)
        {
            content.height + defaultSpacing.minimumRowHeight + (radius * 4) + (defaultSpacing.topBottomMargin * 4) + squareModifier
        }
        else if (!expanded_)
        {
            defaultSpacing.minimumRowHeight + (radius * 2) + (defaultSpacing.topBottomMargin * 2) + (squareModifier / 2)
        }
    }

    WGCopyable{
        id: subPanel_HeaderLaber_WGCopyable
    }

    //delay so panels don't animate when control is created
    Component.onCompleted: {
        animationDelay.start()
        if (transparentPanel_)
        {
            colorHeader_ = "transparent"
            colorBody_ = "transparent"
            mainPanel.radius = 0
        }
        subPanel_HeaderLaber_WGCopyable.setParentCopyable( mainPanel )
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
        radius: defaultSpacing.halfRadius
        color: palette.HighlightShade
        visible: collapsible_ && !palette.GlowStyle
        anchors.top: headerPanel.top
        anchors.bottom: headerPanel.bottom
        anchors.topMargin: mainPanel.radius + (squareModifier / 2)
        anchors.bottomMargin: mainPanel.radius + (squareModifier / 2)
        anchors.horizontalCenter: mainColor.horizontalCenter

        width: expanded_ ? mainColor.width - 6 : mainColor.width + 6 + (squareModifier)

        Component.onCompleted: {
            if(expanded_)
            {
                expandingOuterFrame.color = "transparent"
            }
        }

        Behavior on width {
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
                     if (!running)
                     {
                         expandAnimation.enabled = false
                         if (expanded_)
                         {
                            expandingOuterFrame.color = "transparent"
                         }
                     }
                     else
                     {
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
        anchors.topMargin: defaultSpacing.topBottomMargin
        anchors.bottomMargin: defaultSpacing.topBottomMargin
    }

    Rectangle {
        id: headerPanel
        anchors {left: parent.left; right: parent.right}

        color: "transparent"

        anchors.top: mainColor.top
        height: defaultSpacing.minimumRowHeight + mainPanel.radius * 2 + (squareModifier / 2)

        Rectangle {
            //mouse over panel and activeFocus border

            id: mouseHighlight
            color: "transparent"
            anchors.fill: parent
            anchors.margins: defaultSpacing.doubleBorderSize
            radius: defaultSpacing.halfRadius

            activeFocusOnTab: collapsible_

            border.width: defaultSpacing.standardBorderSize
            border.color: activeFocus && collapsible_ ? palette.LighterShade : "transparent"

            Keys.onPressed: {
                if (event.key == Qt.Key_Space && collapsible_)
                {
                    if (expanded_ && collapsible_)
                    {
                        expandAnimation.enabled = true
                        content.opacity = 0
                        expanded_ = false;
                    }
                    else if (!expanded_ && collapsible_)
                    {
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
                if (collapsible_)
                {
                    mouseHighlight.color = palette.LighterShade
                }
            }

            onExited: {
                if (collapsible_)
                {
                    mouseHighlight.color = "transparent"
                }
            }

            onClicked: {
                if (expanded_ && collapsible_)
                {
                    expandAnimation.enabled = true
                    content.opacity = 0
                    expanded_ = false;
                }
                else if (!expanded_ && collapsible_)
                {
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
            anchors.leftMargin: collapsible_ ? defaultSpacing.leftMargin : 0
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

                MouseArea {
                    anchors.fill: parent
                    enabled: headerLabel.visible
                    hoverEnabled: headerLabel.visible
                    cursorShape: Qt.PointingHandCursor

                    onClicked:{
                        if (!subPanel_HeaderLaber_WGCopyable.enabled || !globalSettings.wgCopyableEnabled )
                        {
                            return;
                        }

                        if ((mouse.button == Qt.LeftButton) && (mouse.modifiers & Qt.ControlModifier))
                        {
                            if (subPanel_HeaderLaber_WGCopyable.selected)
                            {
                                subPanel_HeaderLaber_WGCopyable.deSelect()
                            }
                            else
                            {
                                subPanel_HeaderLaber_WGCopyable.select()
                            }
                        }
                        else if (mouse.button == Qt.LeftButton)
                        {
                            subPanel_HeaderLaber_WGCopyable.rootCopyable.deSelect();
                            subPanel_HeaderLaber_WGCopyable.select()
                        }
                    }
                }
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
                enabled: visible
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

            anchors.leftMargin: defaultSpacing.leftMargin
            anchors.rightMargin: defaultSpacing.standardMargin
            anchors.verticalCenter: parent.verticalCenter
        }

        //controls that can be placed in the header itself
        Loader {
            id: headerControl

            anchors.left: headerBox.right
            anchors.right: panelMenu.left
            anchors.verticalCenter: parent.verticalCenter

            anchors.leftMargin: defaultSpacing.leftMargin
            anchors.rightMargin: contentRightMargin_
            enabled: headerCheck.checked

            sourceComponent: headerObject_
        }

        WGToolButton {
            id: panelMenu
            anchors.right: parent.right

            anchors.verticalCenter: parent.verticalCenter

            anchors.leftMargin: defaultSpacing.leftMargin
            anchors.rightMargin: defaultSpacing.standardMargin

            iconSource: "qrc:///icons/menu_16x16"

            menu: WGMenu{
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
        anchors.topMargin: defaultSpacing.topBottomMargin
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

        height: parent.expanded_ ? content.height + defaultSpacing.doubleMargin - (squareModifier / 2): 0

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
        anchors.topMargin: defaultSpacing.topBottomMargin
    }
}
