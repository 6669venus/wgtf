import QtQuick 2.1
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import WGControls 1.0

Rectangle
{
    id: nodeContainer
    objectName: "Node"
    property var nodeObj
    property var nodeID
    property var nodeTitle
    property var inputSlotsModel
    property var outputSlotsModel
    property var localPosition
    property var globalPosition: mapToItem(graphView, x, y)

    property bool nodeIsExpanded: true

    // TODO link to data
    property string nodeSubTitle: ""

    // TODO link to Data
    property color nodeColor: "blue"

    // TODO link to data
    property string nodeIcon: "images/model_16x16.png"

    color: palette.darkerShade

    radius: defaultSpacing.standardRadius

    width : mainLayout.width + defaultSpacing.doubleBorderSize + defaultSpacing.doubleMargin
    height : mainLayout.height + defaultSpacing.doubleBorderSize + defaultSpacing.doubleBorderSize

    border.width: defaultSpacing.standardBorderSize
    border.color: palette.darkestShade

    x: localPosition.x
    y: localPosition.y

    onGlobalPositionChanged: { nodeObj.setPos(globalPosition.x, globalPosition.y); }

    function getSlotViewBySlotObj(slotObj)
    {
        var isInput = slotObj.isInput;
        var listModel = (isInput) ? inputSlotsRepeater : outputSlotsRepeater;
        var slotView = null;

        var size = listModel.count;
        for (var i = 0; i < size; ++i)
        {
            slotView = listModel.itemAt(i);
            if (slotView.slotObj.id == slotObj.id)
            {
                return slotView;
            }
        }

        return null;
    }

    function getSlotByPos(Pos)
    {
        var slotPos = mapToItem(inputSlotsLayout, Pos.x, Pos.y);
        var slot = inputSlotsLayout.childAt(slotPos.x, slotPos.y);

        if (!slot)
        {
            slotPos = mapToItem(outputSlotsLayout, Pos.x, Pos.y);
            slot = outputSlotsLayout.childAt(slotPos.x, slotPos.y);
        }

        if (slot && slot.objectName == "Slot")
        {
            return slot;
        }

        return null;
    }

    Rectangle {
        color: nodeColor

        anchors.top: parent.top
        anchors.margins: defaultSpacing.standardBorderSize
        anchors.left: parent.left
        anchors.right: parent.right
        height: defaultSpacing.minimumRowHeight

        LinearGradient {
            anchors.fill: parent
            start: Qt.point(0, 0)
            end: Qt.point(parent.width * 0.8, 0)
            opacity: 0.8
            source: parent
            gradient: Gradient {
                GradientStop { position: 0.0; color: nodeColor }
                GradientStop { position: 1.0; color: "black" }
            }
        }

        radius: defaultSpacing.halfRadius
        z: -2

        MouseArea
        {
            anchors.fill: parent
            drag.target: nodeContainer
            drag.axis: Drag.XAndYAxis
            acceptedButtons: Qt.LeftButton
            preventStealing: true

            onDoubleClicked: {
                nodeIsExpanded = !nodeIsExpanded
            }
        }
    }

    ColumnLayout
    {
        id : mainLayout
        anchors.centerIn: parent

        WGExpandingRowLayout {
            id: headerBox
            Layout.fillWidth: true

            Item {
                Layout.preferredHeight: 20
                Layout.preferredWidth: 20

                Image
                {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.horizontalCenterOffset: -defaultSpacing.rowSpacing
                    source: nodeIcon
                    visible: nodeIcon != ""
                }
            }

            WGLabel
            {
                id : header
                Layout.fillHeight : true
                Layout.alignment : Qt.AlignHCenter | Qt.AlignVCenter
                text : nodeTitle
                font.bold: true

                // Text.QtRendering looks a bit fuzzier sometimes but is much better for arbitrary scaling
                renderType: Text.QtRendering
            }

            Item {
                Layout.preferredHeight: 20
                Layout.preferredWidth: 20

                // This could be replaced by a popup menu button?
                WGToolButton
                {
                    id: nodeExpandedBox
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.horizontalCenterOffset: defaultSpacing.rowSpacing
                    width: 20
                    height: 20
                    iconSource: nodeIsExpanded ? "images/arrow_down_small_16x16.png" : "images/arrow_right_small_16x16.png"
                    onClicked: {
                        nodeIsExpanded = !nodeIsExpanded
                    }
                }

            }

        }
        // Placeholder subtitle
        WGMultiLineText
        {
            id : subHeader
            Layout.fillHeight : true
            Layout.preferredWidth: header.width
            Layout.alignment : Qt.AlignLeft | Qt.AlignVCenter
            text : nodeSubTitle
            color: palette.DisabledTextColor

            visible: nodeIsExpanded && nodeSubTitle != ""

            // Text.QtRendering looks a bit fuzzier but is much better for arbitrary scaling
            renderType: Text.QtRendering
        }

        // Feels like a slot should be one object with an input and/or output and potentially an edit control in the middle rather than two separate slots.
        // This would make it easier to line everything up into three columns as well.

        WGExpandingRowLayout {
            id: nodes
            visible: nodeIsExpanded

            ColumnLayout
            {
                id: inputSlotsLayout
                Layout.alignment : Qt.AlignTop | Qt.AlignLeft

                WGListModel
                {
                    id : inputSlotsListModel
                    source : inputSlotsModel

                    ValueExtension {}
                }

                Repeater
                {
                    id : inputSlotsRepeater
                    model : inputSlotsListModel
                    delegate : Slot
                    {
                        z : nodeContainer.z + 10
                        slotObj: Value
                        isInput: Value.isInput
                    }
                }
            }
            // TODO Loader for an edit control?
            Item
            {
                Layout.fillWidth: true
            }

            ColumnLayout
            {
                id: outputSlotsLayout
                Layout.alignment : Qt.AlignTop | Qt.AlignRight

                WGListModel
                {
                    id: outputSlotsListModel
                    source: outputSlotsModel

                    ValueExtension {}
                }


                Repeater
                {
                    id : outputSlotsRepeater
                    model : outputSlotsListModel
                    Layout.alignment : Qt.AlignTop | Qt.AlignRight
                    delegate : Slot
                    {
                        z : nodeContainer.z + 10
                        slotObj: Value
                        isInput: Value.isInput
                        Layout.alignment : Qt.AlignTop | Qt.AlignRight
                    }
                }
            }
        }
    }

    NodeContextMenu
    {
    }
}
