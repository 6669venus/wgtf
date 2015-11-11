import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import QtQuick.Window 2.1
import BWControls 1.0
import WGControls 1.0

/*!
 \brief
----------------------------------------------------------------------------------------------
 Preliminary Layout Designed but not Finalised! Icons and menus currently placeholders only.

 It is operating off data models for file folders and the listview. The layout is based upon:
        https://confluence.wargaming.net/display/NGT/Asset+Browser+Qt+Prototype
----------------------------------------------------------------------------------------------

Example:
\code{.js}
WGAssetBrowser {
    id: testAssetBrowserControl
    viewModel: view
}
\endcode
*/

Rectangle {
    id: rootFrame
    objectName: "WGAssetBrowser"

    property var viewModel;

    anchors.fill: parent
    anchors.margins: defaultSpacing.standardMargin

    color: palette.MainWindowColor

    //TODO Should this be stored somewhere else?
    /*! This property determines the default size of the icons in the listview
        The default value is \c 64
    */
    property int iconSize: 64

    /* This property determines the size of the label of each icon */
    property int iconLabelSize: iconSize > 32 ? 9 : 7

    /* This property determines the line height for each icon label */
    property int iconLabelLineHeight: iconSize > 32 ? 16 : 10

    /*  This property is used to toggle between showing items in a grid (true) or list view (false)
        The default value is \c 64
    */
    property bool showIcons: true

    property QtObject contentDisplayType: ListModel {
        // Start with 'List View'
        property int currentIndex_: 1
        ListElement {
            name_: "displayType1_"
            text: "Icons"
        }
        ListElement {
            name_: "displayType2_"
            text: "List View"
        }
    }

    property bool shouldTrackFolderHistory: true

    // Keep track of folder TreeModel selection indices history
    property var folderHistoryIndices: new Array()
    property int currentFolderHistoryIndex: 0
    property int maxFolderHistoryIndices: 0

    property var activeFilters_: activeFilters

    //--------------------------------------
    // Custom Content Filters
    //--------------------------------------
    // Note: This will be replaced with a more robust filtering system in the near future.


    onHeightChanged: changeAlignment()
    onWidthChanged: changeAlignment(), checkAssetBrowserWidth()

    WGListModel {
        id: customContentFiltersModel
        source: rootFrame.viewModel.data.customContentFilters
        ValueExtension {}
    }

    ListModel {
        id: customContentFiltersList

        Component.onCompleted: {
            var filterItr = iterator( rootFrame.viewModel.data.customContentFilters )
            while (filterItr.moveNext()) {
                customContentFiltersList.append({
                    text: filterItr.current
                });
            }
        }
    }

    BWDataChangeNotifier {
        id: customContentFilterIndexNotifier
        source: rootFrame.viewModel.data.customContentFilterIndexNotifier
        onDataChanged: {
            rootFrame.viewModel.refreshData;
        }
    }

    //--------------------------------------
    // Functions
    //--------------------------------------

    function checkAssetBrowserWidth() {
        // Change breadcrumbs and preferences to double line layout
        if (resizeContainer.singleLineLayout == true)
        {
            var changingLayout = (breadcrumbRepeater.count > 0 &&
                                  breadcrumbRowLayout.width > breadcrumbFrame.width)
            if (changingLayout)
            {
                // reparent breadcrumb group to its own rowlayout
                assetBrowserInfoFirstLine.parent = assetBrowserDoubleLineColumn
                assetBrowserInfoSecondLine.parent = assetBrowserDoubleLineColumn

                //reparent preferences group to its own rowlayout
                assetBrowserPreferencesContainer.parent = assetBrowserInfoSecondLine

                //visibility change
                assetBrowserDoubleLineColumn.visible = true
                assetBrowserInfoSecondLine.visible = true

                resizeContainer.singleLineLayout = false
            }
        }
        else // Change breadcrumbs and preferences to single line layout
        {
            var changingLayoutagain = (breadcrumbRepeater.count > 0 &&
                                       breadcrumbFrame.width - breadcrumbRowLayout.width >
                                       assetBrowserInfoSecondLine.childrenRect.width)
            if (changingLayoutagain)
            {
                // reparent everything to a single row
                assetBrowserInfoFirstLine.parent = resizeContainer
                assetBrowserInfoSecondLine.parent = resizeContainer
                assetBrowserDoubleLineColumn.parent = resizeContainer

                assetBrowserPreferencesContainer.parent = assetBrowserInfoFirstLine

                //visibility swap
                assetBrowserDoubleLineColumn.visible = false
                assetBrowserInfoSecondLine.visible = false

                resizeContainer.singleLineLayout = true
            }
        }
    }

    function changeAlignment() {
        if (assetSplitter.orientation == Qt.Vertical)
        {
            if (height / width < 0.3)
            {
                btnAssetBrowserOrientation.checked = false
                assetSplitter.state = "HORIZONTAL"
            }
        }
        else // Qt.Horizontal
        {
            if (width / height < 0.35)  //The asset browser is less usable in Qt.Horizontal and must switch earlier
            {
                btnAssetBrowserOrientation.checked = true
                assetSplitter.state = "VERTICAL"
            }
        }
    }

    // Selects an asset from the folder contents view
    function selectAsset( index ){
        rootFrame.viewModel.currentSelectedAssetIndex = index;

        // Prepare the menu for display by querying selected asset
        // data and determine what the menu should include or exclude
        // TODO: This functionality should be moved to the WGMenu
        //       after Qt controls v1.4 is added to NGT.
        rootFrame.viewModel.contextMenu.prepareMenu;
    }

    // Tells the page to use the current selected asset
    function onUseSelectedAsset() {
        rootFrame.viewModel.events.useSelectedAsset( listModelSelection.selectedItem );
    }

    // Tells the page to navigate the history forward or backward
    // depending on what button was clicked
    function onNavigate( isForward ) {
        // Don't navigate if we're actively filtering assets
        if (folderContentsModel.isFiltering) {
            return;
        }

        // Don't track the folder history while we use the navigate buttons the history
        rootFrame.shouldTrackFolderHistory = false;

        if (isForward) {
            if (folderHistoryIndices.length > currentFolderHistoryIndex + 1) {
                currentFolderHistoryIndex += 1;
                selector.selectedIndex = folderHistoryIndices[currentFolderHistoryIndex];
            }

        }
        else {
            if (currentFolderHistoryIndex > -1) {
                currentFolderHistoryIndex -= 1;
                selector.selectedIndex = folderHistoryIndices[currentFolderHistoryIndex];
            }
        }
    }


    //--------------------------------------
    // Folder Tree Model
    //--------------------------------------
    WGTreeModel {
        id : folderModel
        objectName: "AssetBrowserTreeModel"
        source : rootFrame.viewModel.data.folders

        ValueExtension {}
        ColumnExtension {}
        ComponentExtension {}
        TreeExtension {
            id: folderTreeExtension
            selectionExtension: selector
        }

        ThumbnailExtension {}
        SelectionExtension {
            id: selector
            onSelectionChanged: {
                if (!folderTreeExtension.blockSelection && !folderContentsModel.isFiltering)
                {
                    // Source change
                    folderModelSelectionHelper.select(getSelection());
                    if (rootFrame.shouldTrackFolderHistory)
                    {
                        // Track the folder selection indices history
                        folderHistoryIndices.push(selector.selectedIndex);
                        currentFolderHistoryIndex = folderHistoryIndices.length - 1;
                        maxFolderHistoryIndices = folderHistoryIndices.length - 1;
                    }

                    // Reset the flag to track the folder history
                    rootFrame.shouldTrackFolderHistory = true;

                    // Update the breadcrumb current index
                    breadcrumbFrame.currentIndex = rootFrame.viewModel.breadcrumbItemIndex;
                }

                folderTreeExtension.blockSelection = false;
            }
        }
    }

    SelectionHelper {
        id: folderModelSelectionHelper
        source: rootFrame.viewModel.folderSelectionHandler
        onSourceChanged: {
            select(selector.getSelection());
        }
    }


    //--------------------------------------
    // List View Model for Folder Contents
    //--------------------------------------
    WGFilteredListModel {
        id : folderContentsModel

        source : rootFrame.viewModel.data.folderContents
        filter: WGTokenizedStringFilter {
            id: folderContentsFilter
            filterText: activeFilters_.stringValue
            itemRole: "Value"
            splitterChar: " "
        }

        onFilteringBegin: {
            folderTreeExtension.blockSelection = true;
        }

        onFilteringEnd: {
            folderTreeExtension.blockSelection = false;
        }

        ValueExtension {}
        AssetItemExtension {}

        ColumnExtension {}
        ComponentExtension {}
        ThumbnailExtension {}
        SelectionExtension {
            id: listModelSelection
            multiSelect: true
            onSelectionChanged: {
                fileModelSelectionHelper.select(getSelection());
            }

            onCurrentIndexChanged: {
                listModelSelection.selectedIndex = currentIndex;
            }
        }
    }

    SelectionHelper {
        id: fileModelSelectionHelper
        source: rootFrame.viewModel.folderContentSelectionHandler
        onSourceChanged: {
            select(listModelSelection.getSelection());
        }
    }

    //--------------------------------------
    // List Model for Location Breadcrumbs
    //--------------------------------------
    WGListModel {
        id: breadcrumbModel
        //gnelsontodo source: rootFrame.viewModel.breadcrumbs
		source: rootFrame.viewModel.breadcrumbsModel.breadcrumbs

        ValueExtension {}
    }

    //--------------------------------------
    // List Model for recent file history
    //--------------------------------------
    WGListModel {
        id: recentFileHistoryModel
        source: rootFrame.viewModel.recentFileHistory

        ValueExtension {}
        ColumnExtension {}
        ComponentExtension {}
        TreeExtension {}
        ThumbnailExtension {}
        SelectionExtension {
            id: recentFileHistorySelection
            multiSelect: false
        }
    }

    BWDataChangeNotifier {
        id: folderSelectionHistory
        source: rootFrame.viewModel.folderSelectionHistoryIndex

        // Update the breadcrumb frame's current item index when we get this data change notify
        onDataChanged: {
            currentFolderHistoryIndex = data;

            // Update the folder TreeModel selectedIndex
            selector.selectedIndex = folderHistoryIndices[data];
        }
    }

    BWDataChangeNotifier {
        id: breadcrumbSelection
        source: rootFrame.viewModel.breadcrumbItemIndexNotifier

        // Update the breadcrumb frame's current item index when we get this data change notify
        onDataChanged: {
            // The breadcrumb index is changed
            breadcrumbFrame.currentIndex = data;

            // Make sure the current index is valid
            if (breadcrumbFrame.currentIndex < breadcrumbFrame.previousIndex)
            {
                // Current parent index
                var newSelectedIndex = selector.selectedIndex;

                var loopCount = breadcrumbFrame.previousIndex - breadcrumbFrame.currentIndex;

                // Update the breadcrumb index
                breadcrumbFrame.currentIndex = data;

                // The parent's index is our new item index
                for (var i = 0; i < loopCount; i++)
                {
                    newSelectedIndex = folderModel.parent( newSelectedIndex );
                }

                // Update the folder TreeModel selectedIndex
                selector.selectedIndex = newSelectedIndex;

                // Reset the previous
                breadcrumbFrame.previousIndex = 0;
            }
        }
    }

    //--------------------------------------
    // Context Menu Enabled Flags Management
    //--------------------------------------

    property bool canAddToSourceControl : true;
    property bool canAssetManageDependencies : true;
    property bool canCheckIn : true;
    property bool canCheckOut : true;
    property bool canCheckOutForDelete : true;
    property bool canCheckOutForMove : true;
    property bool canCheckOutForRename : true;
    property bool canCreatePath : true;
    property bool canExplore : true;
    property bool canFindInDepot : true;
    property bool canGetLatest : true;
    property bool canGetLatestDependencies : true;
    property bool canMakeWritable : true;
    property bool canProperties : true;
    property bool canShowRevisionHistory : true;
    property bool canShowP4FileInfo : true;
    property bool canUndoGet : true;
    property bool canUndoCheckOut : true;

    BWDataChangeNotifier {
        id: canAddToSourceControlNotifier
        source: rootFrame.viewModel.contextMenu.canAddToSourceControlNotifier
        onDataChanged: {
            rootFrame.canAddToSourceControl = data;
        }
    }

    BWDataChangeNotifier {
        id: canAssetManageDependenciesNotifier
        source: rootFrame.viewModel.contextMenu.canAssetManageDependenciesNotifier
        onDataChanged: {
            rootFrame.canAssetManageDependencies = data;
        }
    }

    BWDataChangeNotifier {
        id: canCheckInNotifier
        source: rootFrame.viewModel.contextMenu.canCheckInNotifier
        onDataChanged: {
            rootFrame.canCheckIn = data;
        }
    }

    BWDataChangeNotifier {
        id: canCheckOutNotifier
        source: rootFrame.viewModel.contextMenu.canCheckOutNotifier
        onDataChanged: {
            rootFrame.canCheckOut = data;
        }
    }

    BWDataChangeNotifier {
        id: canCheckOutForDeleteNotifier
        source: rootFrame.viewModel.contextMenu.canCheckOutForDeleteNotifier
        onDataChanged: {
            rootFrame.canCheckOutForDelete = data;
        }
    }

    BWDataChangeNotifier {
        id: canCheckOutForMoveNotifier
        source: rootFrame.viewModel.contextMenu.canCheckOutForMoveNotifier
        onDataChanged: {
            rootFrame.canCheckOutForMove = data;
        }
    }

    BWDataChangeNotifier {
        id: canCheckOutForRenameNotifier
        source: rootFrame.viewModel.contextMenu.canCheckOutForRenameNotifier
        onDataChanged: {
            rootFrame.canCheckOutForRename = data;
        }
    }

    BWDataChangeNotifier {
        id: canCreatePathNotifier
        source: rootFrame.viewModel.contextMenu.canCreatePathNotifier
        onDataChanged: {
            rootFrame.canCreatePath = data;
        }
    }

    BWDataChangeNotifier {
        id: canExploreNotifier
        source: rootFrame.viewModel.contextMenu.canExploreNotifier
        onDataChanged: {
            rootFrame.canExplore = data;
        }
    }

    BWDataChangeNotifier {
        id: canFindInDepotNotifier
        source: rootFrame.viewModel.contextMenu.canFindInDepotNotifier
        onDataChanged: {
            rootFrame.canFindInDepot = data;
        }
    }

    BWDataChangeNotifier {
        id: canGetLatestNotifier
        source: rootFrame.viewModel.contextMenu.canGetLatestNotifier
        onDataChanged: {
            rootFrame.canGetLatest = data;
        }
    }

    BWDataChangeNotifier {
        id: canGetLatestDependenciesNotifier
        source: rootFrame.viewModel.contextMenu.canGetLatestDependenciesNotifier
        onDataChanged: {
            rootFrame.canGetLatestDependencies = data;
        }
    }

    BWDataChangeNotifier {
        id: canMakeWritableNotifier
        source: rootFrame.viewModel.contextMenu.canMakeWritableNotifier
        onDataChanged: {
            rootFrame.canMakeWritable = data;
        }
    }

    BWDataChangeNotifier {
        id: canPropertiesNotifier
        source: rootFrame.viewModel.contextMenu.canPropertiesNotifier
        onDataChanged: {
            rootFrame.canProperties = data;
        }
    }

    BWDataChangeNotifier {
        id: canShowRevisionHistoryNotifier
        source: rootFrame.viewModel.contextMenu.canShowRevisionHistoryNotifier
        onDataChanged: {
            rootFrame.canShowRevisionHistory = data;
        }
    }

    BWDataChangeNotifier {
        id: canShowP4FileInfoNotifier
        source: rootFrame.viewModel.contextMenu.canShowP4FileInfoNotifier
        onDataChanged: {
            rootFrame.canShowP4FileInfo = data;
        }
    }

    BWDataChangeNotifier {
        id: canUndoGetNotifier
        source: rootFrame.viewModel.contextMenu.canUndoGetNotifier
        onDataChanged: {
            rootFrame.canUndoGet = data;
        }
    }

    BWDataChangeNotifier {
        id: canUndoCheckOutNotifier
        source: rootFrame.viewModel.contextMenu.canUndoCheckOutNotifier
        onDataChanged: {
            rootFrame.canUndoCheckOut = data;
        }
    }


    //--------------------------------------
    // View Parent Layout
    //--------------------------------------

    ColumnLayout {
        // Initial column layout with button/path bar at the top and then
        // the split two column panel underneath it.

        id: mainColumn

        anchors.fill: parent
        anchors.margins: defaultSpacing.standardMargin

        WGExpandingRowLayout {
            // Contains both Row and column layout that internal controls are reparented to within checkAssetBrowserWidth
            id: resizeContainer
            Layout.fillWidth: true

            z: 1

            property bool singleLineLayout: true

            WGExpandingRowLayout {
                //contains all assetBrowserInfo in a single line
                id: assetBrowserInfoFirstLine
                Layout.fillWidth: true
                Layout.preferredHeight: defaultSpacing.minimumRowHeight + defaultSpacing.doubleBorderSize

                WGButtonBar {
                    evenBoxes: false

                    buttonList: [
                        // Breadcrumbs and back/forward
                        WGToolButton {
                            id: btnAssetBrowserBack
                            iconSource: "icons/back_16x16.png"
                            tooltip: "Back"
                            enabled: (currentFolderHistoryIndex != 0)

                            onClicked: {
                                onNavigate( false );
                            }
                        },
                        WGToolButton {
                            id: btnAssetBrowserForward
                            iconSource: "icons/fwd_16x16.png"
                            tooltip: "Forward"
                            enabled: (currentFolderHistoryIndex < folderHistoryIndices.length - 1)

                            onClicked: {
                                onNavigate( true );
                            }
                        },
                        WGToolButton {
                            id: btnAssetBrowserHistory
                            iconSource: "icons/arrow_down_small_16x16.png"
                            tooltip: "History"
                            enabled: (currentFolderHistoryIndex != 0)
                            width: 16

                            showMenuIndicator: false

                            menu: WGMenu {
                                id: historyMenu

                                /*
                                    TODO: Make this show the last 10 or so history items. (chronological not depth order)

                                    Instantiator {
                                        model: recentFolderHistoryModel
                                        MenuItem {
                                            text: Value
                                        }
                                        onObjectAdded: historyMenu.insertItem(index, object)
                                        onObjectRemoved: historyMenu.removeItem(object)
                                    }
                                */

                                MenuItem {
                                    text: "History Folder 1"
                                }
                                MenuItem {
                                    text: "History Folder 2"
                                }
                                MenuItem {
                                    text: "History Folder 3"
                                }
                            }
                        }
                    ]
                }

                // TODO: Folder names etc. need to be links
                Rectangle {
                    id: breadcrumbFrame
                    Layout.fillHeight: false
                    Layout.preferredHeight: defaultSpacing.minimumRowHeight
                    Layout.fillWidth: true
                    color: "transparent"

                    // The current breadcrumb item index.
                    property int currentIndex : 0
                    property int previousIndex : 0

                    property bool __showBreadcrumbs: true


                    MouseArea {
                        anchors.fill: parent
                        enabled: breadcrumbFrame.__showBreadcrumbs
                        hoverEnabled: true

                        cursorShape: Qt.IBeamCursor

                        onClicked: {
                            breadcrumbFrame.__showBreadcrumbs = false
                            pathTextBox.forceActiveFocus()
                        }
                    }

                    WGTextBox {
                        id: pathTextBox
                        anchors.fill: parent
                        visible: !breadcrumbFrame.__showBreadcrumbs

                        //TODO: Make this the actual path, and parse a new entered path properly
                        //TODO MUCH LATER: Auto complete.

                        text: "res\\sample\\path\\here"

                        onEditingFinished: {
                            breadcrumbFrame.__showBreadcrumbs = true
                        }
                    }

                    RowLayout {
                        id: breadcrumbLayout
                        anchors.fill: parent
                        spacing: 0

                        visible: breadcrumbFrame.__showBreadcrumbs

                        Component {
                            id: breadcrumbDelegate

                            RowLayout {
                                Layout.fillWidth: false
                                spacing: 1
                                WGLabel {
                                    id: breadcrumbLabel

                                    Layout.fillWidth: true
                                    Layout.preferredHeight: defaultSpacing.minimumRowHeight

                                    elide: Text.ElideRight

                                    //gnelsontodo text: Value.toString()
									text: Value.displayValue

                                    font.bold: true
                                    font.pointSize: 11

                                    color: breadcrumbMouseArea.containsMouse ? palette.TextColor : palette.NeutralTextColor;

									Component.onCompleted: {	
										// @JAMES - this is the section -- I'm expecting to see subItems spit out that it's a QVariant, which is correct
										// but then Value.subItems isn't producing iterator results at all.
										console.log("gnelsontodo - printing out items for " + Value.displayValue + " = " + Value.subItems);									
										var testIter = iterator( Value.subItems );
										while (testIter.moveNext()) {
											console.log("gnelsontodo - " + testIter.current.value);
										}
									}

									MouseArea {
										id: breadcrumbMouseArea
										anchors.fill: parent
										cursorShape: Qt.PointingHandCursor
										hoverEnabled: true
										onPressed: {
												// Do not navigate if we are filtering assets
												if (folderContentsModel.isFiltering) {
													return;
												}

												// Don't track the folder history while we navigate the history
												rootFrame.shouldTrackFolderHistory = false;

												// Update the frame's current index for label color.
												breadcrumbFrame.currentIndex = index;
												breadcrumbFrame.previousIndex = rootFrame.viewModel.breadcrumbItemIndex;

												// Tell the code about this index change by this mouse onPressed event.
												rootFrame.viewModel.breadcrumbItemIndex = index;
												rootFrame.viewModel.events.breadcrumbSelected = Value;
											}
									}
                                }

                                WGToolButton {
                                    visible: index < breadcrumbRepeater.count - 1

                                    Layout.preferredWidth: 16
                                    Layout.preferredHeight: defaultSpacing.minimumRowHeight
                                    showMenuIndicator: false

                                    iconSource: "icons/arrow_right_small_16x16.png"

                                    menu: WGMenu {
                                        id: siblingFolderMenu
										
                                        Instantiator {
                                            model: Value.subItems

											MenuItem {
												text: Value.displayValue
												onTriggered: {
													console.log("Signal that subitem # of item # was clicked.");
												}
											}

                                            onObjectAdded: siblingFolderMenu.insertItem(index, object)
                                            onObjectRemoved: siblingFolderMenu.removeItem(object)
                                        }

                                        MenuItem {
                                            text: "Current Folder"
                                        }
                                        MenuItem {
                                            text: "Sibling Folder 2"
                                        }
                                        MenuItem {
                                            text: "Sibling Folder 3"
                                        }
                                        MenuItem {
                                            text: "Sibling Folder 4"
                                        }
                                        MenuItem {
                                            text: "Sibling Folder 5"
                                        }
                                    }
                                }
                            }
                        }

                        WGExpandingRowLayout {
                            id: breadcrumbRowLayout
                            Layout.fillWidth: true
                            Layout.preferredHeight: defaultSpacing.minimumRowHeight + defaultSpacing.doubleBorderSize

                            onWidthChanged: checkAssetBrowserWidth()

                            spacing: 1

                            Repeater {
                                id: breadcrumbRepeater
                                model: breadcrumbModel
                                delegate: breadcrumbDelegate
                            }
                        }
                    }
                }

                WGExpandingRowLayout {
                    id: assetBrowserPreferencesContainer
                    Layout.fillWidth: false

                    WGPushButton {
                        id: displayButton
                        Layout.preferredWidth: 100
                        checkable: true

                        text: showIcons ? (iconSize + "px Icons") : "List View"

                        Timer {
                            id: fadeTimer
                            running: false
                            interval: 1000

                            onTriggered: {
                                displayButton.checked = false
                            }
                        }

                        onActiveFocusChanged: {
                            if(!activeFocus)
                            {
                                displayButton.checked = false
                            }
                        }

                        Rectangle {
                            id: sizeMenu
                            anchors.left: displayButton.left
                            anchors.top: displayButton.bottom
                            visible: displayButton.checked
                            height: 120
                            width: 100

                            color: palette.MainWindowColor
                            border.width: defaultSpacing.standardBorderSize
                            border.color: palette.DarkColor


                            WGExpandingRowLayout {
                                anchors.fill: parent
                                anchors.margins:{left: 2; right: 2; top: 5; bottom: 5}

                                WGSlider {
                                    id: slider
                                    stepSize: 32
                                    minimumValue: 0
                                    maximumValue: 256
                                    Layout.preferredWidth: 16
                                    Layout.fillHeight: true
                                    orientation: Qt.Vertical

                                    rotation: 180

                                    WGSliderHandle {
                                        id: sliderHandle
                                        minimumValue: slider.minimumValue
                                        maximumValue: slider.maximumValue
                                        showBar: true

                                        value: iconSize

                                        onValueChanged: {
                                            rootFrame.iconSize = value
                                            if(value < 32)
                                            {
                                                showIcons = false
                                            }
                                            else
                                            {
                                                showIcons = true
                                            }
                                        }

                                        Binding {
                                            target: sliderHandle
                                            property: "value"
                                            value: rootFrame.iconSize
                                        }
                                    }
                                }

                                ColumnLayout {
                                    id: menuItems
                                    Layout.fillWidth: true
                                    Layout.fillHeight: true

                                    WGLabel {
                                        text: "List View"
                                    }
                                    WGLabel {
                                        text: "Small Icons"
                                    }
                                    Rectangle {
                                        color: "transparent"
                                        Layout.fillWidth: true
                                        Layout.fillHeight: true
                                    }

                                    WGLabel {
                                        text: "Large Icons"
                                    }
                                }
                            }
                        }


                        MouseArea {
                            id: mainMouseArea
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.top: displayButton.top
                            anchors.bottom: sizeMenu.bottom
                            propagateComposedEvents: true

                            hoverEnabled: displayButton.checked

                            acceptedButtons: Qt.NoButton

                            onEntered: {
                                fadeTimer.stop()
                            }
                            onExited: {
                                fadeTimer.restart()
                            }

                            onWheel: {
                                if (wheel.angleDelta.y > 0)
                                {
                                    sliderHandle.range.decreaseSingleStep()
                                }
                                else
                                {
                                    sliderHandle.range.increaseSingleStep()
                                }
                            }
                        }
                    }

                    // Asset Browser View Options
                    WGPushButton {
                        id: btnAssetBrowserOrientation
                        iconSource: checked ? "icons/rows_16x16.png" : "icons/columns_16x16.png"
                        checkable: true
                        checked: false

                        tooltip: "Horizontal/Vertical Toggle"

                        onClicked: {
                            if (checked) { //note: The click event changes the checked state before (checked) is tested
                                assetSplitter.state = "VERTICAL"
                            }
                            else
                            {
                                assetSplitter.state = "HORIZONTAL"
                            }
                        }
                    }

                    WGPushButton {
                        id: btnAssetBrowserHideFolders
                        iconSource: checked ? "icons/folder_tree_off_16x16.png" : "icons/folder_tree_16x16.png"
                        checkable: true
                        checked: false

                        tooltip: "Hide Folder List"

                        onClicked: {
                            if(checked){
                                leftFrame.visible = false
                            } else {
                                leftFrame.visible = true
                            }
                        }
                    }
                    /*
                    WGToolButton {
                        id: btnUseSelectedAsset
                        iconSource: "icons/list_plus_16x16.png"

                        tooltip: "Apply Asset"

                        onClicked: {
                            onUseSelectedAsset()
                        }
                    }*/
                } //end preferences container
            }

            ColumnLayout { // assetBrowser info is reparented within this in checkAssetBrowserWidth()
                id: assetBrowserDoubleLineColumn
                visible:false
                Layout.preferredWidth: 0
                Layout.fillHeight: false
            }

            WGExpandingRowLayout {
                id: assetBrowserInfoSecondLine
                Layout.fillWidth: false
                visible: false
            }
        }


        WGExpandingRowLayout {
            //Filter Box

            id: assetFilter
            Layout.fillWidth: true
            Layout.preferredHeight: defaultSpacing.minimumRowHeight + defaultSpacing.doubleBorderSize

            Rectangle {
                id: activeFiltersRect
                Layout.fillWidth: true
                Layout.minimumHeight: defaultSpacing.minimumRowHeight + defaultSpacing.doubleBorderSize
                Layout.preferredHeight: childrenRect.height
                color: "transparent"

                WGActiveFilters {
                    id: activeFilters
                    anchors {left: parent.left; top: parent.top; right: parent.right}
                    height: childrenRect.height
                    inlineTags: true
                    dataModel: rootFrame.viewModel.data.activeFilters
                }
            }

            // Apply custom filters to data that do not get overridden by
            // the text-based filters. Temporary solution until a more
            // robust filtering system can be added. This will not show
            // if no custom filters are attached, so as to not leave stray
            // and unusable UI elements in the control.
            WGLabel {
                text: "File Type:"
                visible: customContentFiltersList.count > 0
            }

            WGDropDownBox {
                id: customContentFiltersMenu
                Layout.preferredWidth: 150
                visible: customContentFiltersList.count > 0

                model: customContentFiltersList
                currentIndex: 0

                onCurrentIndexChanged: {
                    rootFrame.viewModel.data.currentCustomContentFilter = currentIndex;
                }
            }
        }

        // SplitView that breaks the panel up into two columns with draggable
        // handle. Haven't used this before but seems to work fine.

        SplitView {
            id: assetSplitter
            Layout.fillHeight: true
            Layout.fillWidth: true
            orientation: Qt.Horizontal

            states: [
                State {
                    name: "VERTICAL"
                    PropertyChanges { target: assetSplitter; orientation: Qt.Vertical }
                    PropertyChanges { target: leftFrame; height: Math.min(200, Math.round(assetSplitter.height / 3)) }
                },
                State {
                    name: "HORIZONTAL"
                    PropertyChanges { target: assetSplitter; orientation: Qt.Horizontal }
                    PropertyChanges { target: leftFrame; width: Math.min(300, Math.round(assetSplitter.width / 3)) }
                }
            ]

            // TODO Maybe should be a separate WG Component
            handleDelegate: Rectangle {
                color: "transparent"
                width: defaultSpacing.doubleMargin

                WGSeparator {
                    vertical_: true
                    width: 2
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                }
            }

            Rectangle {
                // This rectangle is basically an invisible layer ... but for
                // some reason if the first level in a SplitView is a layout,
                // it behaves weirdly with minimumWidths
                id: leftFrame

                Layout.minimumHeight: 0;
                Layout.minimumWidth: 0;
                height: Math.min(200, Math.round(assetSplitter.height / 3));
                width: Math.min(250, Math.round(assetSplitter.width/3));

                color: "transparent"

                // Left Column: Search bar and folder tree
                ColumnLayout {
                    id: parentColumnLayout
                    anchors.fill: parent
                    /*
                    // Filtering
                    WGExpandingRowLayout {
                        id: folderFilterControls
                        Layout.fillWidth: true
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight + defaultSpacing.doubleBorderSize

                        WGPushButton {
                            id: btnOpenAssetLocation
                            iconSource: "icons/search_folder_16x16.png"

                            tooltip: "Collection Options"

                            menu: WGMenu {
                                title: "Collections"
                                MenuItem {
                                    text: "MOCKUP ONLY"
                                }

                                MenuSeparator{}

                                MenuItem {
                                    text: "Collection 1"
                                    checkable: true
                                    checked: true
                                }
                                MenuItem {
                                    text: "Collection 2"
                                    checkable: true
                                    checked: true
                                }
                                MenuItem {
                                    text: "Collection 3"
                                    checkable: true
                                    checked: false
                                }

                                MenuSeparator { }

                                MenuItem {
                                    text: "Edit Current Collection..."
                                }
                                MenuItem {
                                    text: "Delete Current Collection..."
                                }

                                MenuSeparator { }

                                MenuItem {
                                    text: "Create New Collection..."
                                }
                            }
                        }

                        // TODO Make search box search for things
                        WGTextBox {
                            id: folderSearchBox
                            Layout.fillWidth: true
                            Layout.preferredHeight: defaultSpacing.minimumRowHeight
                            placeholderText: "Search"
                        }
                    }*/

                    // TODO Set this up to use tabs for different collections.
                    // This will need a proper TabViewStyle made though

                    WGTreeView {
                        id: folderView
                        model : folderModel
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        columnDelegates : [foldersColumnDelegate]
                        selectionExtension: selector
                        treeExtension: folderTreeExtension
                        flatColourisation: true
                        depthColourisation: 0
                        lineSeparator: true

                        property Component foldersColumnDelegate:
                            Rectangle {
                                id: folderIconHeaderContainer
                                color: "transparent"
                                Image{
                                    id: folderFileIcon
                                    anchors.verticalCenter: folderIconHeaderContainer.verticalCenter
                                    visible: true
                                    anchors.left: folderIconHeaderContainer.left //itemData.expandIconArea.right
                                    width: sourceSize.width
                                    height: sourceSize.heigth
                                    //TODO: Awaiting type support for icon customisation
                                    source: itemData.HasChildren ? (itemData.Expanded ? "icons/folder_open_16x16.png" : "icons/folder_16x16.png") : "icons/file_16x16.png"
                                }
                                Text {
                                    anchors.left: folderFileIcon.right
                                    color: palette.TextColor
                                    clip: itemData != null && itemData.Component != null
                                    text: itemData != null ? itemData.Value : ""
                                    anchors.leftMargin: folderView.expandIconMargin // TODO no defined error
                                    font.bold: itemData != null && itemData.HasChildren
                                    verticalAlignment: Text.AlignVCenter
                                    anchors.verticalCenter: folderIconHeaderContainer.verticalCenter
                                    elide: Text.ElideRight
                                }
                            }
                    }// TreeView
                        /*
                        Rectangle {
                            id: recentFileHistoryRect
                            color: "transparent"
                            anchors.fill: parent

                            Layout.fillHeight: true
                            Layout.preferredHeight: defaultSpacing.minimumRowHeight
                            Layout.fillWidth: true

                            WGListView {
                                id: recentFileHistoryList

                                anchors.fill: parent

                                model: recentFileHistoryModel
                                enableVerticalScrollBar: true
                                selectionExtension: recentFileHistorySelection
                                columnDelegates: [historyColumnDelegate]
                            }

                            Component {
                                id: historyColumnDelegate

                                Item {
                                    Layout.fillWidth: true
                                    Layout.preferredHeight: defaultSpacing.minimumRowHeight
                                    Rectangle {
                                        id: historyIcon

                                        color: "transparent"
                                        width: defaultSpacing.minimumRowHeight

                                        anchors.left: parent.left
                                        anchors.top: parent.top
                                        anchors.bottom: parent.bottom

                                        Image {
                                            source: "icons/file_16x16.png"
                                            anchors.centerIn: parent
                                        }
                                    }

                                    Rectangle {
                                        anchors.left: historyIcon.right
                                        anchors.right: parent.right
                                        anchors.top: parent.top
                                        anchors.bottom: parent.bottom
                                        anchors.margins: 1

                                        color: "transparent"

                                        WGLabel {
                                            text: itemData.Value
                                            anchors.fill: parent
                                        }
                                    }
                                }
                            }
                        }*/

                        /* TODO: Favourites functionality should be added later when a preferences system exists to
                                 persist the data. Remove for now, so as to not confuse end-users.
                                 JIRA: http://jira.bigworldtech.com/browse/NGT-906
                        Tab{
                            title : "Favourites"
                        }*/
                } // End of Column
            } //End LeftFrame

            Rectangle {
                // This rectangle is basically invisible... but for some reason
                // if the first level in a SplitView is a layout, it behaves
                // weirdly with minimumWidths

                id: rightFrame
                Layout.fillHeight: true
                Layout.fillWidth: true
                color: "transparent"

                ColumnLayout {
                    // Right Column: Filters, Files + Assets, Saved Filters & View Options

                    id: fileColumn
                    anchors.fill: parent

                    Rectangle {
                        //Assets/Files Frame
                        id: folderContentsRect

                        color: palette.LightPanelColor

                        Layout.fillHeight: true
                        Layout.fillWidth: true

                        clip: true
                        /*
                        Loader {
                            anchors.fill: parent
                            source: showIcons ? assetList : assetGrid
                        }*/

                        // TODO: This should probably be a WGGridView at some point.

                        GridView {
                            id: assetGrid
                            visible: showIcons

                            height: folderContentsRect.height
                            width: folderContentsRect.width

                            cellWidth: folderContentsRect.width / Math.floor(folderContentsRect.width / iconSize)
                            cellHeight: iconSize + 36

                            model: folderContentsModel
                            delegate: folderContentsDelegate

                            snapMode: GridView.SnapToRow

                            highlight: WGHighlightFrame {

                            }

                            highlightMoveDuration: 0

                            WGScrollBar {
                                 id: verticalScrollBar
                                 width: defaultSpacing.rightMargin
                                 anchors.top: assetGrid.top
                                 anchors.right: assetGrid.right
                                 anchors.bottom: assetGrid.bottom
                                 orientation: Qt.Vertical
                                 position: assetGrid.visibleArea.yPosition
                                 pageSize: assetGrid.visibleArea.heightRatio
                                 scrollFlickable: assetGrid
                                 visible: assetGrid.contentHeight > assetGrid.height
                             }
                        }

                        Component {
                            id: folderContentsDelegate
                            //Individual grid file/Asset. Height/Width determined by iconSize from iconSizeSlider

                            Rectangle {
                                id: assetEntryRect
                                visible: showIcons
                                width: assetGrid.cellWidth
                                height: assetGrid.cellHeight

                                color: "transparent"

                                ColumnLayout {
                                    spacing: 0
                                    anchors.fill: parent

                                    //TODO Replace this with proper thumbnail
                                    Rectangle {
                                        Layout.preferredHeight: iconSize
                                        Layout.preferredWidth: iconSize
                                        Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                                        color: "transparent"

                                        Image {
                                            id: icon_file
                                            anchors.fill: parent
                                            source: {
                                                if (  IsDirectory == true )
                                                    return "icons/folder_128x128.png"
                                                else if ( Thumbnail != undefined )
                                                    return Thumbnail
                                                else
                                                    return "icons/file_128x128.png"
                                            }
                                            Image {
                                                source: StatusIcon != undefined ? StatusIcon : ""
                                                anchors.left: icon_file.left
                                                anchors.bottom: icon_file.bottom
                                                anchors.leftMargin: iconSize > 32 ? Math.round(iconSize / 12) : 0
                                                anchors.bottomMargin: iconSize > 32 ? Math.round(iconSize / 24) : 0
                                            }
                                        }
                                    }

                                    WGMultiLineText {
                                        id: iconLabel
                                        text: Value
                                        horizontalAlignment: Text.AlignHCenter

                                        lineHeightMode: Text.FixedHeight
                                        lineHeight: iconLabelSize + 2

                                        y: -2

                                        Layout.preferredWidth: parent.width - defaultSpacing.rowSpacing * 2
                                        Layout.preferredHeight: defaultSpacing.minimumRowHeight * 2
                                        Layout.alignment: Qt.AlignTop | Qt.AlignHCenter

                                        //height: defaultSpacing.minimumRowHeight * 2

                                        maximumLineCount: {
                                            var lines = 2
                                            if (index == rootFrame.viewModel.currentSelectedAssetIndex){
                                                lines = 3
                                            }
                                            if (iconSize <= 32) {
                                                lines += 1
                                            }
                                            return lines
                                        }
                                        wrapMode: Text.WrapAnywhere

                                        font.pointSize: iconLabelSize

                                        elide: {
                                            if (index == rootFrame.viewModel.currentSelectedAssetIndex){
                                                return Text.ElideNone
                                            } else {
                                                return Text.ElideRight
                                            }
                                        }
                                    }
                                }

                                MouseArea {
                                    id: assetMouseArea
                                    anchors.fill: parent
                                    cursorShape: Qt.PointingHandCursor

                                    acceptedButtons: Qt.RightButton | Qt.LeftButton

                                    onPressed: {
                                        if(mouse.button == Qt.LeftButton){
                                            assetGrid.currentIndex = index
                                        }
                                    }

                                    onClicked: {
                                        if(mouse.button == Qt.RightButton){
                                            assetGrid.currentIndex = index
                                        }
                                    }

                                    onDoubleClicked: {
                                        if(mouse.button == Qt.LeftButton){
                                            assetGrid.currentIndex = index
                                            onUseSelectedAsset()
                                        }
                                    }
                                }
                                Loader {
                                    anchors.fill: parent
                                    sourceComponent: fileContextMenu
                                }
                            }
                        }

                        WGListView {
                            id: assetList
                            visible: !showIcons

                            anchors.fill: parent

                            model: folderContentsModel
                            selectionExtension: listModelSelection
                            columnDelegates: [columnDelegate]


                            onRowDoubleClicked: {
                                if(mouse.button == Qt.LeftButton) {
                                    onUseSelectedAsset()
                                }
                            }

                            onReturnPressed: {
                                // Select the current asset when the enter key is pressed
                                onUseSelectedAsset();
                            }
                        }

                        Component {
                            id: columnDelegate

                            Item {
                                visible: !showIcons
                                Layout.fillWidth: true
                                Layout.preferredHeight: defaultSpacing.minimumRowHeight
                                Rectangle {
                                    id: fileIcon

                                    color: "transparent"
                                    width: defaultSpacing.minimumRowHeight

                                    anchors.left: parent.left
                                    anchors.top: parent.top
                                    anchors.bottom: parent.bottom

                                    Image {
                                        source: itemData.TypeIcon != "" ? itemData.TypeIcon : "icons/file_16x16.png"
                                        anchors.centerIn: parent
                                    }

                                    Image {
                                        source: itemData.StatusIcon != undefined ? itemData.StatusIcon : ""
                                        anchors.centerIn: parent
                                    }
                                }

                                Rectangle {
                                    anchors.left: fileIcon.right
                                    anchors.right: parent.right
                                    anchors.top: parent.top
                                    anchors.bottom: parent.bottom
                                    anchors.margins: 1

                                    color: "transparent"

                                    WGLabel {
                                        text: itemData.Value
                                        anchors.fill: parent
                                    }
                                }

                                Loader {
                                    anchors.fill: parent
                                    sourceComponent: fileContextMenu
                                }
                            }
                        }

                        Component {
                            id: fileContextMenu
                            Item {
                                WGContextArea {
                                    // TODO: Allow the menu component to be loaded via the view model to allow customization
                                    // Use the selection as context for determining if menu items are enabled
                                    contextMenu: WGMenu
                                    {
                                        WGMenu {
                                            id: expolorerMenu
                                            title: "Explorer"
                                            MenuItem {
                                                text: "Create Path"
                                                onTriggered: rootFrame.viewModel.contextMenu.createPath
                                                enabled: rootFrame.canCreatePath
                                            }

                                            MenuItem {
                                                text: "Explore"
                                                onTriggered: rootFrame.viewModel.contextMenu.explore
                                                enabled: rootFrame.canExplore
                                            }

                                            MenuItem {
                                                text: "Make Writable"
                                                onTriggered: rootFrame.viewModel.contextMenu.makeWritable
                                                enabled: rootFrame.canMakeWritable
                                            }

                                            MenuItem {
                                                text: "Properties"
                                                onTriggered: rootFrame.viewModel.contextMenu.properties
                                                enabled: rootFrame.canProperties
                                            }
                                        }

                                        WGMenu {
                                            id: p4Menu
                                            title: "Perforce"
                                            MenuItem {
                                                text: "Get Latest Version"
                                                onTriggered: rootFrame.viewModel.contextMenu.getLatest
                                                enabled: rootFrame.canGetLatest
                                            }

                                            MenuItem {
                                                text: "Get Latest with Dependencies"
                                                onTriggered: rootFrame.viewModel.contextMenu.getLatestDependencies
                                                enabled: rootFrame.canGetLatestDependencies
                                            }

                                            MenuItem {
                                                text: "Asset Manage with Dependencies"
                                                onTriggered: rootFrame.viewModel.contextMenu.assetManageDependencies
                                                enabled: rootFrame.canAssetManageDependencies
                                            }

                                            MenuItem {
                                                text: "Undo Get"
                                                onTriggered: rootFrame.viewModel.contextMenu.undoGet
                                                enabled: rootFrame.canUndoGet
                                            }

                                            MenuSeparator { }

                                            MenuItem {
                                                text: "Add to Source Control"
                                                onTriggered: rootFrame.viewModel.contextMenu.addToSourceControl
                                                enabled: rootFrame.canAddToSourceControl
                                            }

                                            MenuSeparator { }

                                            MenuItem {
                                                text: "Check In..."
                                                onTriggered: rootFrame.viewModel.contextMenu.checkIn
                                                enabled: rootFrame.canCheckIn
                                            }

                                            MenuItem {
                                                text: "Check Out"
                                                onTriggered: rootFrame.viewModel.contextMenu.checkOut
                                                enabled: rootFrame.canCheckOut
                                            }

                                            MenuItem {
                                                text: "Undo Check Out..."
                                                onTriggered: rootFrame.viewModel.contextMenu.undoCheckOut
                                                enabled: rootFrame.canUndoCheckOut
                                            }

                                            MenuItem {
                                                text: "Check Out for Delete..."
                                                onTriggered: rootFrame.viewModel.contextMenu.checkOutForDelete
                                                enabled: rootFrame.canCheckOutForDelete
                                            }

                                            MenuItem {
                                                text: "Check Out for Move..."
                                                onTriggered: rootFrame.viewModel.contextMenu.checkOutForMove
                                                enabled: rootFrame.canCheckOutForMove
                                            }

                                            MenuItem {
                                                text: "Check Out for Rename..."
                                                onTriggered: rootFrame.viewModel.contextMenu.checkOutForRename
                                                enabled: rootFrame.canCheckOutForRename
                                            }

                                            MenuSeparator { }

                                            MenuItem {
                                                text: "Revision History..."
                                                onTriggered: rootFrame.viewModel.contextMenu.showRevisionHistory
                                                enabled: rootFrame.canShowRevisionHistory
                                            }

                                            MenuItem {
                                                text: "Perforce File Info..."
                                                onTriggered: rootFrame.viewModel.contextMenu.showP4FileInfo
                                                enabled: rootFrame.canShowP4FileInfo
                                            }

                                            MenuItem {
                                                text: "Find in Depot..."
                                                onTriggered: rootFrame.viewModel.contextMenu.findInDepot
                                                enabled: rootFrame.canFindInDepot
                                            }
                                        }

                                        //TODO: We need access to Qt Quick controls version 1.4 before this
                                        //      will work.
                                        /*onAboutToShow: {
                                            //TODO: Prepare menu code should go here.
                                        }*/
                                    }
                                }
                            }
                        }

                    } //Asset Icon Frame
                } //Right Hand Column Layout
            } //RightFrame
        } //SplitView
    }
}
