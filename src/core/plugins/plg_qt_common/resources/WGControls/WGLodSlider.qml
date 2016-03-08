import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Private 1.0
import QtQuick.Dialogs 1.2

/*!
 \A Level of Detail slider control designed to replace ModelEditors LOD control

Example:
\code{.js}
WGLodSlider {
    Layout.fillWidth: true
}
\endcode
*/

//TODO: An example of this hooked up to real world data is required above

WGColumnLayout {
    id: mainBody
    objectName: "WGLodSlider"

    /*! This property holds a Sample LOD data set if used if one isnt explicitly set */
    //TODO: This should be renamed, it does not require "_"
    property QtObject lodList_: ListModel{
        ListElement {
            objectName: "testLod1_"
            model_: "testLod1_.model"
            text_: "testLOD1"
            minDist_: 0
            maxDist_: 25
            selected_: false
            hidden_: false
            locked_:false
        }
        ListElement {
            objectName: "testLod2_"
            model_: "testLod2_.model"
            text_: "testLOD2"
            minDist_: 25
            maxDist_: 75
            selected_: false
            hidden_: false
            locked_:false
        }
        ListElement {
            objectName: "testLod3_"
            model_: "testLod3_.model"
            text_: "testLOD3"
            minDist_: 75
            maxDist_: 150
            selected_: false
            hidden_: false
            locked_:false
        }
        ListElement {
            objectName: "testLod4_"
            model_: "testLod4_.model"
            text_: "testLOD4"
            minDist_: 150
            maxDist_: 250
            selected_: false
            hidden_: false
            locked_:false
        }
        ListElement {
            objectName: "testLod5_"
            model_: "testLod5_.model"
            text_: "testLOD5"
            minDist_: 250
            maxDist_: 350
            selected_: false
            hidden_: false
            locked_:false
        }
    }

    /* TODO: Make this property do something.
    It is based on the ModelEditor feature that has the optional ability to prevent the currently selected object from modifying parent LOD model settings. */

    /*!
        This property is used to display the locked status of a LoD model. It does not currently work.
        The default value is \c false
    */
    property bool lockLod: false

    /*!
        This property defines the absolute maximum distance a LoD model can be set to
        The default value is \c 999
        */
    /* TODO: This value should probably be a fallback, the value should read from a game configuration file.
    Should this be marked as internal? */
    property int maxDistanceAbs_: 999

    /*!
        This property is determines the width taken up by the WGNumberBox's either side of the LOD control
        The default value is \c 60
    */
    //TODO: This should be renamed, it does not require "_"
    property int valueBoxWidth_: 60

    /*!
        This property is determines the width of the gap to the right of the LoD sliders which is used to indicate a
        LoD value goes off to infinity
        The default value is \c 10
    */
    //TODO: This should be renamed, it does not require "_"
    property int rightGapWidth_: 10

    /*!
        This property holds the current camera distance
        The default value is \c 0
    */
    //TODO: Should this be an internal property?
    property int currentDistance_: 0

    /*! \internal */
    property real upperBound_: {
        if(infinite_ >= 0)
        {
            Math.max(lodList_.get(lodList_.count - 1).maxDist_, rightGapWidth_) //if infinite upper value = last max value
        }
        else
        {
            lodList_.get(lodList_.count - 1).maxDist_ + rightGapWidth_ //if not infinite upper value = last max value plus an extra gap
        }
    }

    //disable bindings when deleting a LOD to prevent errors
    //TODO: This should be an internal control and should be marked as private by "__" prefix
    /*! \internal */
    property bool deleting_: false

    //TODO: Document this
    property bool addLOD_: false

    //TODO: Document this
    property int selectedLOD_: -1

    /*! \internal */
    //TODO: This should be an internal control and should be marked as private by "__" prefix
    property int infinite_: -1 //does the last LOD stretch to infinity

    /*! \internal */
    //TODO: This should be an internal control and should be marked as private by "__" prefix
    property bool virtual_: false

    property real unitWidth_: (lodFrame.width - (defaultSpacing.standardMargin * 2)) / upperBound_

    /*! \internal */
    //TODO: This should be an internal control and should be marked as private by "__" prefix
    property bool changingMin_: false

    /*! \internal */
    //TODO: This should be an internal control and should be marked as private by "__" prefix
    property bool changingMax_: false

    //stop the distance slider overrunning when upperBound_ is changed
    onUpperBound_Changed: {
        if (currentDistance_ > upperBound_){
            currentDistance_ = upperBound_
        }
    }

    onSelectedLOD_Changed: {
        if (selectedLOD_ < 0)
        { // if no LOD is selected disable buttons and set hide to false
            var i
            for (i = 0; i < lodOptions.buttonList.count; i++)
            {
                lodOptions.buttonList[index].enabled = false
            }
            button_hide.checked = false
            button_lock_selected_lod.checked = false

        }
        else
        { // A LOD is selected, enable relevant buttons
            button_open.enabled = true
            button_hide.enabled = true
            button_use_camera.enabled = true
            button_lock_selected_lod.enabled = true

            button_lock_selected_lod.checked = lodList_.get(selectedLOD_).locked_
            button_hide.checked = lodList_.get(selectedLOD_).hidden_

            if (selectedLOD_ == 0)
            { //first lod is selected
                lodOptions.buttonList[1].enabled = false
            }
            else
            {
                lodOptions.buttonList[1].enabled = true
            }

            if (selectedLOD_ == lodList_.count - 1)
            { //last LOD is selected
                lodOptions.buttonList[2].enabled = false
            } else
            {
                lodOptions.buttonList[2].enabled = true
            }
        }
    }

    //Checks if any required changes while a lod is being dragged
    function checkValues (index, newVal)
    {
        //checks max and min of current LOD are ok
        if (lodList_.get(index).maxDist_ < newVal)
        {
            lodList_.set(index,{"maxDist_": newVal})
        }
        else if (lodList_.get(index).minDist_ > newVal)
        {
            lodList_.set(index,{"minDist_": newVal})
        }

        //set hidden on current LOD
        if (lodList_.get(index).maxDist_ == lodList_.get(index).minDist_)
        {
            lodList_.set(index,{"hidden_": true})
            if(index == selectedLOD_)
            {
                button_hide.checked = true
            }
        }
        else
        {
            lodList_.set(index,{"hidden_": false})
            if(index == selectedLOD_)
            {
                button_hide.checked = false
            }
        }

        //Checks all mins and maxes of LODS closer than current LOD are not higher than the new value
        if (index > 0)
        {
            for(var i = 0; i < index; i++)
            {
                var checkMax = lodList_.get(i).maxDist_
                if (checkMax > newVal)
                {
                    lodList_.set(i,{"maxDist_": newVal})
                }
                var checkMin = lodList_.get(i).minDist_
                if (checkMin > newVal)
                {
                    lodList_.set(i,{"minDist_": newVal})
                }
            }
        }

        //Checks all mins and maxes of LODS further than current LOD are not lower than the new value
        if (index < (lodList_.count - 1))
        {
            for(var j = index + 1; j < lodList_.count; j++)
            {
                var checkMax = lodList_.get(j).maxDist_
                if (checkMax < newVal)
                {
                    lodList_.set(j,{"maxDist_": newVal})
                }
                var checkMin = lodList_.get(j).minDist_
                if (checkMin < newVal)
                {
                    lodList_.set(j,{"minDist_": newVal})
                }
            }
        }
    }

    //checks that all the lods are continuous and that the upperBound is correct
    function checkLods()
    {
        for(var i = 0; i < lodList_.count; i++)
        {
            if(lodList_.get(i).minDist_ > lodList_.get(i).maxDist_)
            {
                lodList_.set(i,{"maxDist_": lodList_.get(i).minDist_})
            }

            if (i != lodList_.count - 1)
            {
                if(lodList_.get(i).maxDist_ != lodList_.get(i + 1).minDist_)
                {
                    lodList_.set(i + 1,{"minDist_": lodList_.get(i).maxDist_})
                }
            }

            if(lodList_.get(i).maxDist_ == lodList_.get(i).minDist_)
            {
                lodList_.set(i,{"hidden_": true})
                if(i == selectedLOD_)
                {
                    button_hide.checked = true
                }
            }
            else
            {
                lodList_.set(i,{"hidden_": false})
                if(i == selectedLOD_)
                {
                    button_hide.checked = false
                }
            }
        }

        if(infinite_ >= 0)
        {
            setInfinite()
        }

        if(infinite_ >= 0 && upperBound_ <= maxDistanceAbs_)
        {
            upperBound_ = Math.max(lodList_.get(lodList_.count - 1).maxDist_, rightGapWidth_) //if infinite upper value = last max value
        }
        else  if (infinite_ == -1 && upperBound_ <= maxDistanceAbs_)
        {
            upperBound_ = lodList_.get(lodList_.count - 1).maxDist_ + rightGapWidth_ //if not infinite upper value = last max value plus an extra gap
        }

        if (upperBound_ > maxDistanceAbs_)
        {
            upperBound_ = maxDistanceAbs_
        }
        else if (upperBound_ < rightGapWidth_)
        {
            upperBound_ = rightGapWidth_
        }
    }

    //Moves a LOD in fromIndex up or down the list (direction = +1/-1), count number of times)
    function moveLOD(fromIndex, direction, count)
    {
        if(Math.abs(direction) != 1)
        {
            console.log("Move LOD failed. Direction must be 1 or -1")
            return
        }

        for (var i=0; i < count; i++)
        {
            if (fromIndex == 0 && direction == -1)
            {
                console.log("Reached start of LOD list")
                i = count
            }
            else if (fromIndex == lodList_.count -1 && direction == 1)
            {
                console.log("Reached end of LOD list")
                i = count
            }

            var toIndex = fromIndex + direction
            var toModel = lodList_.get(toIndex).model_
            var toName = lodList_.get(toIndex).text_

            lodList_.set(toIndex, {"model_": lodList_.get(fromIndex).model_, "text_": lodList_.get(fromIndex).text_,})
            lodList_.set(fromIndex, {"model_": toModel, "text_": toName,})

            var fromWidth = lodList_.get(fromIndex).maxDist_ - lodList_.get(fromIndex).minDist_
            var toWidth = lodList_.get(toIndex).maxDist_ - lodList_.get(toIndex).minDist_

            if (direction == 1)
            {
                lodList_.set(fromIndex, {"maxDist_": lodList_.get(fromIndex).minDist_ + toWidth})
            }
            else if (direction == -1)
            {
                lodList_.set(toIndex, {"maxDist_": lodList_.get(toIndex).minDist_ + fromWidth})
            }

            checkLods()
            fromIndex += direction
        }
    }

    //Change a LOD's model and name
    function changeLOD(filename)
    {
        //get the trimmed modelname from the URL
        var lastSlash = filename.toString().lastIndexOf("/")
        var lastDot = filename.toString().lastIndexOf(".")
        var trimmedName = filename.toString().substring(lastSlash + 1, lastDot)
    }

    //add a new LOD
    function addLOD(filename)
    {
        //get the trimmed modelname from the URL
        var lastSlash = filename.toString().lastIndexOf("/")
        var lastDot = filename.toString().lastIndexOf(".")
        var trimmedName = filename.toString().substring(lastSlash + 1, lastDot)

        var oldMaxDist = lodList_.get(lodList_.count - 1).maxDist_

        lodList_.append({
                    "model_": filename.toString(),
                    "text_": trimmedName,
                    "minDist_": oldMaxDist,
                    "maxDist_": oldMaxDist + rightGapWidth_
                        })
        addLOD_ = false
        checkLods()
        selectedLOD_ = lodList_.count - 1
    }

    //Set hidden_ and change LOD width
    function hideLOD(index)
    {
        if(lodList_.get(index).hidden_)
        {
            lodList_.set(index,{"hidden_": false, "maxDist_": lodList_.get(index).maxDist_ + 10})
        } else
        {
            lodList_.set(index,{"hidden_": true, "maxDist_": lodList_.get(index).minDist_})
        }
        checkValues(index, lodList_.get(index).maxDist_)
        checkLods()
    }

    //Delete a LOD and shuffle everything up.
    function deleteLOD(index)
    {
        deleting_ = true

        var deletedWidth = lodList_.get(index).maxDist_ - lodList_.get(index).minDist_
        for (var i = index + 1; i < lodList_.count - 1; i++)
        {
            lodList_.set(i,{"minDist_": lodList_.get(i).minDist_ - deletedWidth, "maxDist_": lodList_.get(i).maxDist_ - deletedWidth})
        }

        lodStack.children[index].Layout.row += 1

        lodList_.remove(index)

        checkLods()

        selectedLOD_ = -1
        deleting_ = false
    }

    function setInfinite()
    {
        infinite_ = -1
        for (var i = lodList_.count - 1; i >= 0; i--)
        {
            if (lodList_.get(i).minDist_ != lodList_.get(i).maxDist_)
            {
                infinite_ = i
                i = -1
            }
        }
    }

    //Labels for value boxes and bar list.
    WGExpandingRowLayout {
        Layout.fillWidth: true
        spacing: defaultSpacing.rowSpacing

        WGLabel {
            text: "Min Dist (m)"
            width: valueBoxWidth_
        }

        WGLabel {
            text: "LOD List"
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            font.bold: true
        }

        WGLabel {
            text: "Max Dist (m)"
            width: valueBoxWidth_
        }

    }

    WGExpandingRowLayout {

        Layout.fillWidth: true
        spacing: defaultSpacing.rowSpacing

        WGColumnLayout {
            id: minColumn

            Repeater {
                // Stack of minimum distance number boxes
                model: lodList_
                delegate: WGNumberBox {
                    value: model.minDist_
                    Layout.preferredWidth: valueBoxWidth_
                    minimumValue: 0
                    maximumValue: index > 0 ? maxDistanceAbs_ : 0 //min distance on LOD 0 should always be 0

                    textColor: {
                        if(enabled && index == selectedLOD_ && !activeFocus)
                        {
                            palette.HighlightColor
                        }
                        else if(enabled && !readOnly)
                        {
                            palette.TextColor
                        }
                        else if(enabled && readOnly)
                        {
                            palette.NeutralTextColor
                        }
                        else
                        {
                            palette.DisabledTextColor
                        }
                    }


                    onValueChanged: {
                        if (!changingMin_ && !changingMax_ && index > 0)
                        {
                            //change this LOD's min distance and the LOD above's max distance
                            if (dragging_)
                            {
                                changingMin_ = true
                                lodList_.set(index,{"minDist_": value})
                                lodList_.set(index-1,{"maxDist_": value})
                                checkValues(index, value)
                                checkLods()
                                changingMin_ = false
                            }
                        }
                    }

                    onEditingFinished: {
                        changingMin_ = true
                        lodList_.set(index,{"minDist_": value})
                        lodList_.set(index-1,{"maxDist_": value})
                        checkValues(index, value)
                        checkLods()
                        changingMin_ = false
                    }

                    readOnly: index == 0 ? true : false

                    Connections {
                        target: deleting_ ? null : lodList_.get(index)
                        onMinDist_Changed: {
                            value = minDist_
                        }
                    }
                }
            }
        }

        WGColumnLayout { // Area containing LOD slider bars

            WGTextBoxFrame {
                id: lodFrame
                Layout.fillWidth: true
                //(Number of Lods * rowHeight) + spacing between rows + top & bottom margins
                Layout.preferredHeight: (lodList_.count * defaultSpacing.minimumRowHeight) + ((lodList_.count - 1) * defaultSpacing.rowSpacing) + (defaultSpacing.standardMargin * 2)

                clip: true

                Rectangle {
                    id: cameraDistanceBar
                    color: palette.TextColor
                    opacity: 0.3
                    width: defaultSpacing.standardBorderSize
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    z: 2
                    x: (currentDistance_ * unitWidth_) + defaultSpacing.standardMargin
                }

                GridLayout {
                    id: lodStack
                    anchors.fill: parent
                    anchors.margins: defaultSpacing.standardMargin
                    rowSpacing: defaultSpacing.rowSpacing
                    columnSpacing: 0

                    Repeater {
                        model: lodList_
                        delegate: Rectangle {
                            id: colorBar

                            property bool highlighted_: {
                                if (!deleting_)
                                {
                                    lodList_.get(index).selected_
                                } else
                                {
                                    false
                                }
                            }

                            Connections {
                                target: mainBody
                                onSelectedLOD_Changed: {
                                    if (target.selectedLOD_ == index)
                                    {
                                        highlighted_ = true
                                    } else
                                    {
                                        highlighted_ = false
                                    }
                                }
                            }

                            border.width: highlighted_ ? defaultSpacing.standardBorderSize : 0
                            border.color: highlighted_ ? palette.TextColor : "transparent"

                            //3 repeating colours
                            color: {
                                var barColor = "transparent"
                                if (!deleting_)
                                {
                                    var colorCount = index % 3
                                    if (colorCount == 0)
                                    {
                                        barColor = "#e23627"
                                    }
                                    else if (colorCount == 1)
                                    {
                                        barColor = "#7ac943"
                                    }
                                    else if (colorCount == 2)
                                    {
                                        barColor = "#3fa9f5"
                                    }
                                }
                                return barColor
                            }

                            opacity: model.maxDist_ === model.minDist_ ? 0.5 : 1
                            radius: defaultSpacing.halfRadius

                            Layout.preferredHeight: defaultSpacing.minimumRowHeight
                            Layout.preferredWidth: {
                                if(!deleting_)
                                {
                                    if (lodList_.get(index).maxDist_ == lodList_.get(index).minDist_)
                                    {
                                        0
                                    }
                                    else
                                    {
                                        (lodList_.get(index).maxDist_ - lodList_.get(index).minDist_) * unitWidth_
                                    }
                                }
                                else
                                {
                                    0
                                }
                            }

                            Layout.fillWidth: true
                            Layout.row: index
                            Layout.column: index

                            //click to select bar
                            MouseArea {
                                anchors.centerIn: parent
                                width: parent.width - defaultSpacing.doubleMargin
                                height: parent.height
                                hoverEnabled: true
                                cursorShape: Qt.PointingHandCursor

                                onClicked: {
                                    selectedLOD_ = index
                                    lodList_.set(index, {"selected_": true})
                                    parent.focus = true
                                }
                            }

                            //model name next to the bar. Click to change the model
                            Text {
                                objectName: "barName"
                                id: barName
                                color: state == "" || parent.highlighted_ ? palette.TextColor : parent.color
                                text: deleting_ ? "" : lodList_.get(index).text_
                                opacity: state == "" || parent.highlighted_  ? 1 : 0.5
                                y: defaultSpacing.standardBorderSize
                                horizontalAlignment: Text.AlignLeft
                                x: defaultSpacing.standardMargin

                                font.bold: parent.highlighted_ ? true : false

                                states: [
                                    State {
                                        name: "RIGHT"
                                        when: ((upperBound_ * unitWidth_) - (lodList_.get(index).maxDist_ * unitWidth_) > barName.paintedWidth)

                                        PropertyChanges {target: barName; x: parent.width + defaultSpacing.standardMargin}
                                        PropertyChanges {target: barName; horizontalAlignment: Text.AlignLeft}
                                        PropertyChanges {target: barName; visible: true}
                                    },
                                    State {
                                        name: "LEFT"
                                        when: ((lodList_.get(index).minDist_ * unitWidth_ > barName.paintedWidth))

                                        PropertyChanges {target: barName; x: -barName.paintedWidth - defaultSpacing.standardMargin}
                                        PropertyChanges {target: barName; horizontalAlignment: Text.AlignRight}
                                        PropertyChanges {target: barName; visible: true}
                                    }
                                ]

                                MouseArea {
                                    anchors.centerIn: parent
                                    height: parent.height
                                    width: parent.width - defaultSpacing.doubleBorderSize
                                    hoverEnabled: true

                                    cursorShape: Qt.PointingHandCursor

                                    onClicked: {
                                        selectedLOD_ = index
                                        lodList_.set(index, {"selected_": true})
                                    }
                                }

                            }

                            //LOD number inside LOD bar, greys out and italics when LOD is zero width (hidden)
                            Text {
                                id: lodNumber
                                text: (infinite_ == index) ? "∞" : index
                                color: model.maxDist_ === model.minDist_ ? palette.DisabledTextColor : palette.TextColor
                                anchors.centerIn: parent
                                horizontalAlignment: Text.AlignHCenter
                                font.pixelSize: (infinite_ == index) ? 22 : 12
                                font.bold: true
                                font.italic: model.maxDist_ === model.minDist_ && infinite_ != index ? true : false
                            }

                            //draggable handle on left side of LOD bar
                            MouseArea {
                                id: minDrag
                                height: parent.height
                                width: defaultSpacing.doubleMargin
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.horizontalCenter: parent.left
                                hoverEnabled: true
                                cursorShape: Qt.SizeHorCursor

                                //no minimum handle on first LOD
                                visible: index > 0 ? true : false

                                z: 1

                                onMouseXChanged: {
                                    if (pressed)
                                    {
                                        if(index > 0)
                                        {
                                            //map mouse to inside LOD Frame Box
                                            var pos = mapToItem(lodFrame,mouse.x, mouse.y)
                                            var newVal = 0
                                            if (infinite_ >= 0)
                                            {
                                                if (pos.x > 0 && pos.x < lodFrame.width)
                                                { //move value to position of mouse inside Frame Box
                                                    newVal = Math.round((pos.x / lodFrame.width) * upperBound_)
                                                } else if (pos.x <= 0)
                                                { //set value to 0 if mouse to left of Frame Box
                                                    newVal = 0
                                                } else if (pos.x >= lodFrame.width)
                                                { //set value to max if mouse to right of Frame Box
                                                    newVal = upperBound_
                                                }
                                            }
                                            else
                                            {
                                                if (pos.x > 0 && pos.x < (lodFrame.width - rightGapWidth_))
                                                { //move value to position of mouse inside Frame Box and extra gap at max
                                                    newVal = Math.round((pos.x / lodFrame.width) * upperBound_)
                                                }
                                                else if (pos.x <= 0)
                                                { //set value to 0 if mouse to left of Frame Box
                                                    newVal = 0
                                                }
                                                else if (pos.x >= (lodFrame.width - rightGapWidth_))
                                                { //increase the upper bound and set value to upper bound

                                                    //TODO:: This should probably not be per tick but on some nice real time value
                                                    if (upperBound_ < maxDistanceAbs_)
                                                    {
                                                        upperBound_ += 1
                                                    }
                                                    newVal = upperBound_
                                                }
                                            }

                                            changingMin_ = true

                                            //update this LODS min value and the previous LODS max value

                                            lodList_.set((index - 1),{"maxDist_": newVal})
                                            lodList_.set((index),{"minDist_": newVal})

                                            checkValues(index, newVal)
                                        }
                                    }
                                }

                                onPressed: {
                                    preventStealing = true
                                }

                                onReleased: {
                                    preventStealing = false

                                    checkLods()

                                    changingMin_ = false
                                    changingMax_ = false
                                }


                            }

                            //draggable handle on right side of LOD bar
                            MouseArea {
                                id: maxDrag
                                height: parent.height
                                width: defaultSpacing.doubleMargin
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.horizontalCenter: parent.right

                                cursorShape: Qt.SizeHorCursor

                                //no minimum handle on last LOD if infinite = true
                                visible: {
                                    if (infinite_ == -1)
                                    {
                                        true
                                    }
                                    else if (infinite_ >= 0 && index < lodList_.count - 1)
                                    {
                                        true
                                    }
                                    else
                                    {
                                        false
                                    }
                                }

                                z: 1

                                onMouseXChanged: {
                                    if (pressed)
                                    {
                                        //map mouse to inside LOD Frame Box
                                        var pos = mapToItem(lodFrame,mouse.x, mouse.y)
                                        var newVal = 0
                                        if (infinite_ >= 0)
                                        {
                                            if (pos.x > 0 && pos.x < lodFrame.width)
                                            { //move value to position of mouse inside Frame Box
                                                newVal = Math.round((pos.x / lodFrame.width) * upperBound_)
                                            }
                                            else if (pos.x <= 0)
                                            { //set value to 0 if mouse to left of Frame Box
                                                newVal = 0
                                            }
                                            else if (pos.x >= lodFrame.width)
                                            { //set value to max if mouse to right of Frame Box
                                                newVal = upperBound_
                                            }
                                        }
                                        else
                                        {
                                            if (pos.x > 0 && pos.x < (lodFrame.width - rightGapWidth_))
                                            { //move value to position of mouse inside Frame Box and extra gap at max
                                                newVal = Math.round((pos.x / lodFrame.width) * upperBound_)
                                            }
                                            else if (pos.x <= 0)
                                            { //set value to 0 if mouse to left of Frame Box
                                                newVal = 0
                                            }
                                            else if (pos.x >= (lodFrame.width - rightGapWidth_))
                                            { //increase the upper bound and set value to upper bound

                                                //TODO:: This should probably not be per tick but on some nice real time value
                                                if (upperBound_ < maxDistanceAbs_)
                                                {
                                                    upperBound_ += 1
                                                }
                                                newVal = upperBound_
                                            }
                                        }

                                        changingMax_ = true

                                        //update this LODS max value and the next LODS min value
                                        if (index < lodList_.count - 1)
                                        {
                                            lodList_.set((index + 1),{"minDist_": newVal})
                                        }

                                        lodList_.set((index),{"maxDist_": newVal})

                                        checkValues(index, newVal)
                                    }
                                }

                                onPressed: {
                                    preventStealing = true
                                }

                                onReleased: {
                                    preventStealing = false

                                    checkLods()

                                    changingMin_ = false
                                    changingMax_ = false
                                }
                            }
                        }
                    }

                    //invisible gap if slider != infinite
                    Rectangle {
                        id: finiteSpacer
                        color: "transparent"
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight
                        Layout.preferredWidth: {
                            if (infinite_ >= 0 && lodList_.get(lodList_.count - 1).maxDist_ >= rightGapWidth_)
                            {
                                0
                            }
                            else if (infinite_ >= 0 && lodList_.get(lodList_.count - 1).maxDist_ <= rightGapWidth_)
                            {
                                (rightGapWidth_ - lodList_.get(lodList_.count - 1).maxDist_) * unitWidth_
                            }
                            else
                            {
                                rightGapWidth_ * unitWidth_
                            }
                        }
                        Layout.column: lodList_.count
                        Layout.row: lodList_.count - 1
                    }
                }
            }
        }

        WGColumnLayout {
            id: maxColumn

            Repeater {
                // Stack of maximum distance number boxes
                model: lodList_
                delegate: WGNumberBox {
                    value: model.maxDist_
                    Layout.preferredWidth: valueBoxWidth_
                    minimumValue: 0
                    maximumValue: infinite_ >= 0 ? maxDistanceAbs_ : maxDistanceAbs_ - rightGapWidth_

                    textColor: {
                        if (enabled && index == selectedLOD_ && !activeFocus)
                        {
                            palette.HighlightColor
                        }
                        else if (enabled && !readOnly)
                        {
                            palette.TextColor
                        }
                        else if (enabled && readOnly)
                        {
                            palette.NeutralTextColor
                        }
                        else
                        {
                            palette.DisabledTextColor
                        }
                    }

                    onValueChanged: {
                        if (!changingMin_ && !changingMax_)
                        {
                            //change this LOD's max distance and the LOD before's max distance
                            if (dragging_)
                            {
                                changingMax_ = true
                                lodList_.set(index,{"maxDist_": value})

                                if (index < lodList_.count - 1)
                                {
                                    lodList_.set(index + 1,{"minDist_": value})
                                }

                                checkValues(index, value)
                                checkLods()
                                changingMax_ = false
                            }
                        }
                    }

                    onEditingFinished: {
                        changingMax_ = true
                        lodList_.set(index,{"maxDist_": value})

                        if (index < lodList_.count - 1)
                        {
                            lodList_.set(index + 1,{"minDist_": value})
                        }

                        checkValues(index, value)
                        checkLods()
                        changingMax_ = false
                    }

                    Connections {
                        target: deleting_ ? null : lodList_.get(index)
                        onMaxDist_Changed: {
                            value = maxDist_
                        }
                    }
                }
            }
        }


        //Delete LOD buttons. Disabled if only one LOD left.
        WGColumnLayout {
            Repeater {
                model: lodList_
                delegate: WGToolButton {
                    enabled: lodList_.count > 1 && index == selectedLOD_
                    iconSource: "icons/delete_16x16.png"
                    tooltip: "Delete Selected LOD"
                    onClicked: {
                        deleteLOD(index)
                    }
                }
            }
        }
    }

    WGSeparator{}

    //Labels for Distance slider & toggle options
    WGExpandingRowLayout {
        Layout.fillWidth: true
        spacing: defaultSpacing.rowSpacing

        WGLabel {
            text: "Dist (m)"
            Layout.preferredWidth: minColumn.width
        }

        WGExpandingRowLayout {
            Layout.maximumWidth: lodFrame.width
            Layout.minimumWidth: lodFrame.width


            WGButtonBar {
                objectName: "WGButtonBar"
                id: lodOptions
                Layout.fillWidth: true
                buttonList: [
                    WGPushButton {
                        objectName: "buttonOpen"
                        id: button_open
                        iconSource: "icons/open_16x16.png"
                        tooltip: "Open Model"
                        enabled: false
                        onClicked: chooseLODModelDialog.open()
                    },
                    WGPushButton {
                        objectName: "buttonUp"
                        id: button_up
                        iconSource: "icons/arrow2_up_16x16.png"
                        tooltip: "Move LOD Up"
                        enabled: false

                        onClicked: {
                            moveLOD(selectedLOD_,-1,1)
                            selectedLOD_ -= 1
                        }
                    },
                    WGPushButton {
                        objectName: "buttonDown"
                        id: button_down
                        iconSource: "icons/arrow2_down_16x16.png"
                        tooltip: "Move LOD Down"
                        enabled: false
                        onClicked: {
                            moveLOD(selectedLOD_,1,1)
                            selectedLOD_ += 1
                        }
                    },
                    WGPushButton {
                        objectName: "buttonHide"
                        id: button_hide
                        iconSource: checked ? "icons/show_16x16.png" : "icons/hide_16x16.png"
                        tooltip: checked ? "Unhide LOD" : "Hide LOD"
                        enabled: false
                        checkable: true
                        checked: false
                        onClicked: {
                            hideLOD(selectedLOD_)
                        }
                    },
                    WGPushButton {
                        objectName: "buttonUseCamera"
                        id: button_use_camera
                        iconSource: "icons/camera_dist_16x16.png"
                        tooltip: "Use Current Camera Distance"
                        enabled: false
                        onClicked: { //ToDo Test if the lod is locked
                            lodList_.set(selectedLOD_, {"maxDist_": currentDistance_})
                            checkValues(selectedLOD_, lodList_.get(selectedLOD_).maxDist_)
                            checkLods()
                        }
                    },
                    WGPushButton {
                        objectName: "buttonLockSelectedLod"
                        id: button_lock_selected_lod
                        iconSource: "icons/lock_16x16.png"
                        tooltip: "Lock the selected LOD"
                        enabled: false
                        checkable: true
                        checked: false
                        onClicked: {
                            if (lodList_.get(selectedLOD_).locked_ == true)
                            {
                                lodList_.set(selectedLOD_, {"locked_": false})
                            }
                            else
                            {
                                lodList_.set(selectedLOD_, {"locked_": true})
                            }
                        }
                    }
                ]
            }


            WGPushButton {
                objectName: "buttonExtendLOD"
                iconSource: "icons/infinite_16x16.png"
                Layout.preferredWidth: implicitWidth
                tooltip: "Extend LOD to Infinity"
                checkable: true
                checked: infinite_ >= 0
                onClicked: {
                    if(checked)
                    {
                        setInfinite()
                        checkLods()
                    }
                    else
                    {
                        infinite_ = -1
                        checkLods()
                    }
                }
            }

            WGPushButton {
                objectName: "buttonAddNewLOD"
                iconSource: "icons/add_16x16.png"
                Layout.preferredWidth: implicitWidth
                tooltip: "Add New LOD"
                onClicked: {
                    addLOD_ = true
                    chooseLODModelDialog.open()
                }
            }
        }

        WGLabel {
            text: "Max Dist (m)"
            Layout.preferredWidth: maxColumn.width
        }
    }

    //Distance slider and values
    WGExpandingRowLayout {
        Layout.fillWidth: true
        spacing: defaultSpacing.rowSpacing

        WGNumberBox {
            objectName: "cameraDistance"
            //Camera distance
            id: currentDistanceNum
            Layout.preferredWidth: valueBoxWidth_
            value: currentDistance_
            minimumValue: 0
            maximumValue: upperBound_

            onValueChanged: {
                currentDistance_ = value
            }

            Connections {
                target: mainBody
                onCurrentDistance_Changed: {
                    currentDistanceNum.value = target.currentDistance_
                }
            }
        }

        //camera distance control
        WGSliderControl {
            objectName: "cameraDistanceSlider"
            id: distanceSlider
            Layout.fillWidth: true
            minimumValue: 0
            maximumValue: upperBound_
            value: currentDistance_
            decimals: 0

            //no value box because we want it on the left
            showValue: false

            onValueChanged: {
                currentDistance_ = value
            }

            Connections {
                target: mainBody
                onCurrentDistance_Changed: {
                    distanceSlider.value = target.currentDistance_
                }
            }
        }

        //uneditable textbox that shows the upperBound
        WGNumberBox {
            id: upperBoundNum
            Layout.preferredWidth: valueBoxWidth_
            value: upperBound_
            minimumValue: upperBound_
            maximumValue: upperBound_
            readOnly: true

            Connections {
                target: mainBody
                onUpperBound_Changed: {
                    upperBoundNum.value = target.upperBound_
                }
            }
        }

        //add LOD button
        WGToolButton {
            objectName: "buttonVirtualLODDistance"
            iconSource: checked ? "icons/camera_lock_16x16.png" : "icons/camera_16x16.png"
            checkable: true
            checked: false
            tooltip: "Virtual LOD Distance"
            onClicked: {
                virtual_ = checked
            }
        }
    }

    FileDialog {
        objectName: "chooseLODModelDialog"
        id: chooseLODModelDialog
        title: "Choose a model"
        visible: false
        nameFilters: [ "Model files (*.model)" ]
        onAccepted: {
            if (addLOD_)
            {
                addLOD(chooseLODModelDialog.fileUrl)
            } else
            {
                changeLOD(chooseLODModelDialog.fileUrl)
            }
        }
        onRejected:
        {
            console.log("Open Model Cancelled")
        }
    }
}
