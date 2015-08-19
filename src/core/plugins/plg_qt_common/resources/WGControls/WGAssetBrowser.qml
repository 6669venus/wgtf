import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import QtQuick.Window 2.1
import BWControls 1.0
import WGControls 1.0

//----------------------------------------------------------------------------------------------
// Preliminary Layout Designed but not Finalised! Icons and menus currently placeholders only.
//
// It is operating off data models for file folders and the listview. The layout is based upon:
//		https://confluence.wargaming.net/display/NGT/Asset+Browser+Qt+Prototype
//----------------------------------------------------------------------------------------------

Rectangle {
	id: rootFrame
	property var viewModel;

	anchors.fill: parent
	anchors.margins: defaultSpacing.standardMargin

	color: palette.MainWindowColor

	//TODO Should this be stored somewhere else?
	property int iconSize: 64
	property int iconLabelSize: iconSize > 32 ? 9 : 7
	property int iconLabelLineHeight: iconSize > 32 ? 16 : 10

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

	//--------------------------------------
	// Custom Content Filters
	//--------------------------------------
	// Note: This will be replaced with a more robust filtering system in the near future.

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
		rootFrame.viewModel.events.useSelectedAsset = listModelSelection.selectedItem;
	}

	// Tells the page to navigate the history forward or backward
	// depending on what button was clicked
	function onNavigate( isForward ) {
        // Don't track the folder history while we use the navigate buttons the history
        rootFrame.shouldTrackFolderHistory = false;

		if (isForward) {
			rootFrame.viewModel.events.navigateHistoryForward = true;
		}
		else {
			rootFrame.viewModel.events.navigateHistoryBackward = true;
		}
	}


	//--------------------------------------
	// Folder Tree Filter & Model
	//--------------------------------------
	WGTreeFilter {
		id: filter
		source: rootFrame.viewModel.data.folders
		filter: folderSearchBox.text
	}

	WGTreeModel {
		id : folderModel

		source : filter.filteredSource

		ValueExtension {}
		ColumnExtension {}
		ComponentExtension {}
		TreeExtension {}
		ThumbnailExtension {}
        SelectionExtension {
			id: selector
            onSelectionChanged: {
                // Source change
                rootFrame.viewModel.folderTreeItemSelected = selector.selectedItem;

                if (rootFrame.shouldTrackFolderHistory)
                {
                    // Track the folder selection indices history
                    folderHistoryIndices.push(selector.selectedIndex);
                }

                // Reset the flag to track the folder history
                rootFrame.shouldTrackFolderHistory = true;

                // Let the filter know about this source change
                folderContentsFilter.sourceChanged();

                // Update the breadcrumb current index
                breadcrumbFrame.currentIndex = rootFrame.viewModel.breadcrumbItemIndex;
				// TODO: support multi-selection
				rootFrame.viewModel.events.folderSelectionChanged = selector.selectedItem
            }
        }
	}


	//--------------------------------------
    // List Filter for Folder Contents
    //--------------------------------------
    AssetBrowserListFilter {
        id: folderContentsFilter
		source: rootFrame.viewModel.data.folderContents
        filter: folderContentsSearchBox.text
    }


	//--------------------------------------
	// List View Model for Folder Contents
	//--------------------------------------
	WGListModel {
		id : folderContentsModel

		//NOTE: Due to a thread issue between the ListFilter and the ListModel,
		//		the filter is applied to the model directly through the FilterChanged event.
		source : rootFrame.viewModel.data.folderContents

		ValueExtension {}

        ColumnExtension {}
        ComponentExtension {}
        TreeExtension {}
        ThumbnailExtension {}
		SelectionExtension {
			id: listModelSelection
			multiSelect: true
			onSelectionChanged: {	
				// TODO: support multi-selection
				rootFrame.viewModel.events.assetSelectionChanged = listModelSelection.selectedItem;
			}
		}
	}


	//--------------------------------------
	// List Model for Location Breadcrumbs
	//--------------------------------------
	WGListModel {
		id: breadcrumbModel
		source: rootFrame.viewModel.breadcrumbs

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
            // Update the folder TreeModel selectedIndex
            selector.selectedIndex = folderModel.index(rootFrame.viewModel.folderTreeItemIndex, 0, folderModel.parent(folderHistoryIndices[data]));
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
			// Button Bar then Breadcrumbs/Path
			id: assetBrowserInfo
			Layout.fillWidth: true
			Layout.preferredHeight: defaultSpacing.minimumRowHeight + defaultSpacing.doubleBorderSize

			// Tool Buttons:
			WGToolButton {
				id: btnAssetBrowserNewAsset
				iconSource: "qrc:///icons/new_16x16"
				noFrame_: false
				tooltip: "New Asset"

				menu: WGMenu {
					title: "Create New:"
					MenuItem {
						text: "MOCKUP ONLY"
					}

					MenuSeparator{}

					MenuItem {
						text: "Asset Type 1"
					}
					MenuItem {
						text: "Asset Type 2"
					}
					MenuItem {
						text: "Asset Type 3"
					}

					MenuSeparator { }

					MenuItem {
						text: "Other Asset..."
					}
				}
			}

			WGToolButton {
				id: btnAssetBrowserImportAsset
				iconSource: "qrc:///icons/import_object_16x16"
				noFrame_: false
				tooltip: "Import Asset"
			}

			WGSeparator {
				vertical_: true
				Layout.fillHeight: false
				Layout.preferredHeight: defaultSpacing.minimumRowHeight
			}

			WGToolButton {
				id: btnAssetBrowserMoveToCollection
				iconSource: "qrc:///icons/add_to_folder_16x16"
				noFrame_: false
				tooltip: "Add Asset to Collection"

				menu: WGMenu {
					title: "Add Selected To:"

					MenuItem {
						text: "MOCKUP ONLY"
					}

					MenuSeparator{}

					MenuItem {
						text: "Collection 1"
					}
					MenuItem {
						text: "Collection 2"
					}
					MenuItem {
						text: "Collection 3"
					}

					MenuSeparator { }

					MenuItem {
						text: "New Collection..."
					}
				}
			}

			WGToolButton {
				id: btnAssetBrowserSelectAll
				iconSource: "qrc:///icons/select_object_16x16"
				noFrame_: false
				tooltip: "Select All Instances"
			}

			WGToolButton {
				id: btnAssetBrowserReplaceAll
				iconSource: "qrc:///icons/replace_object_16x16"
				noFrame_: false
				tooltip: "Replace All Instances"
			}

			WGSeparator {
				vertical_: true
				Layout.fillHeight: false
				Layout.preferredHeight: defaultSpacing.minimumRowHeight
			}

			//Breadcrumbs and browsing

			WGToolButton {
				id: btnAssetBrowserBack
				iconSource: "qrc:///icons/back_16x16"
				noFrame_: false
				tooltip: "Back"

				onClicked: {
					onNavigate( false );
				}
			}

			WGToolButton {
				id: btnAssetBrowserForward
				iconSource: "qrc:///icons/fwd_16x16"
				noFrame_: false
				tooltip: "Forward"

				onClicked: {
					onNavigate( true );
				}
			}

			// Breadcrumbs/Path

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

				RowLayout {

					id: breadcrumbLayout
					anchors.fill: parent

					Component {
						id: breadcrumbDelegate

						WGLabel {
							id: breadcrumbLabel

							Layout.fillWidth: false
							Layout.preferredHeight: defaultSpacing.minimumRowHeight

							elide: Text.ElideRight

							text: Value

							font.bold: true
							font.pointSize: 11

							color: palette.NeutralTextColor;

							MouseArea {
								id: breadcrumbMouseArea
								anchors.fill: parent
								cursorShape: Qt.PointingHandCursor
								onPressed: {
									// TODO: Will need a proper method call here to
									//       navigate the asset tree location from
									//       the selected breadcrumb.
									console.log("You have clicked " + Value)

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

						// TODO: Didn't put in the ">" since it was tacking on
						//       an extra one at the end. Not sure how we can
						//       handle that in QML (gnelson)
					}

					WGExpandingRowLayout {
						id: breadcrumbRowLayout
						Layout.fillWidth: true
						Layout.preferredHeight: defaultSpacing.minimumRowHeight + defaultSpacing.doubleBorderSize

						Repeater {
							model: breadcrumbModel
							delegate: breadcrumbDelegate
						}
					}
				}
			}

			// Asset Browser View Options
			WGToolButton {
				id: btnAssetBrowserOrientation
				iconSource: checked ? "qrc:///icons/rows_16x16" : "qrc:///icons/columns_16x16"
				noFrame_: false
				checkable: true
				checked: false

				tooltip: "Horizontal/Vertical Toggle"

				onClicked: {
					if(checked){
						assetSplitter.orientation = Qt.Vertical
						leftFrame.Layout.fillHeight = false
						leftFrame.Layout.fillWidth = true
						leftFrame.Layout.minimumHeight = 250
						leftFrame.Layout.minimumWidth = 0
					} else {
						assetSplitter.orientation = Qt.Horizontal
						leftFrame.Layout.fillHeight = true
						leftFrame.Layout.fillWidth = false
						leftFrame.Layout.minimumHeight = 0
						leftFrame.Layout.minimumWidth = 250
					}
				}
			}

			WGToolButton {
				id: btnAssetBrowserHideFolders
				iconSource: checked ? "qrc:///icons/folder_tree_off_16x16" : "qrc:///icons/folder_tree_16x16"
				noFrame_: false
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

			WGToolButton {
				id: btnUseSelectedAsset
				iconSource: "qrc:///icons/list_plus_16x16"
				noFrame_: false

				tooltip: "Apply Asset"

				onClicked: {
					onUseSelectedAsset()
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

				Layout.fillHeight: true
				Layout.minimumWidth: 250

				color: "transparent"

				// Left Column: Search bar and folder tree
				ColumnLayout {


					id: parentColumnLayout
					anchors.fill: parent

					// Filtering
					WGExpandingRowLayout {
						id: folderFilterControls
						Layout.fillWidth: true
						Layout.preferredHeight: defaultSpacing.minimumRowHeight + defaultSpacing.doubleBorderSize

						WGToolButton {
							id: btnOpenAssetLocation
							iconSource: "qrc:///icons/search_folder_16x16"
							noFrame_: false

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
					}

					// TODO Set this up to use tabs for different collections.
					// This will need a proper TabViewStyle made though

					WGTabView{
						anchors.fill: parent
						tabPosition: Qt.BottomEdge

						Layout.fillHeight: true
						Layout.fillWidth: true

						// Folders (TreeView) Tab
						Tab {
							title : "Folders"

							WGTreeView {
								id: folderView
								model : folderModel
								anchors.fill: parent
								columnDelegates : [defaultColumnDelegate]
								selectionExtension: selector
							}// TreeView
						}//Tab

						// Recent File History Tab
						Tab {
							title : "Recent Files"
                            anchors.fill: parent

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
												source: "qrc:///icons/file_16x16"
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
                            }
                        } // End of "History" Tab

						/* TODO: Favourites functionality should be added later when a preferences system exists to
						         persist the data. Remove for now, so as to not confuse end-users.
								 JIRA: http://jira.bigworldtech.com/browse/NGT-906
						Tab{
							title : "Favourites"
						}*/

					}//TabView
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


					WGExpandingRowLayout {
						//Filter Box

						id: assetFilter
						Layout.fillWidth: true
						Layout.preferredHeight: defaultSpacing.minimumRowHeight + defaultSpacing.doubleBorderSize


						WGToolButton {
							//Save filters and load previous filters
							id: btnListviewFilters
							iconSource: "qrc:///icons/filter_16x16"
							noFrame_: false

							tooltip: "Filter Options"


							menu: WGMenu {
								title: "Filters"
								MenuItem {
									text: "MOCKUP ONLY"
								}

								MenuSeparator{}

								MenuItem {
									text: "Save Filter..."
								}

								MenuItem {
									text: "Clear Filters"
								}

								MenuSeparator { }

                                    WGMenu {
                                        title: "Saved Filters:"

                                    MenuItem {
                                        text: "Saved Filter 1"
                                    }
                                    MenuItem {
                                        text: "Saved Filter 2"
                                    }
                                    MenuItem {
                                        text: "Saved Filter 3"
                                    }
                                    MenuItem {
                                        text: "Saved Filter 4"
                                    }
                                }
							}
						}

						WGTextBox {
                            id: folderContentsSearchBox
							Layout.fillWidth: true
							placeholderText: "Filter"
							onTextChanged:{
								rootFrame.viewModel.events.filterChanged = folderContentsSearchBox.text
							}
						}

						WGToolButton {
							id: btnListviewAdd
							iconSource: "qrc:///icons/add_16x16"
							noFrame_: false

							tooltip: "Apply Filter"
						}
					}

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
												if (  Value.isDirectory == true ) 
													return "qrc:///icons/folder_128x128"
												else 
													return "qrc:///icons/file_128x128"
											}
										}
									}

									WGMultiLineText {
										id: iconLabel
										text: Value.filename
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
											selectAsset( index )
											assetGrid.currentIndex = index
										}
									}

									onClicked: {
										if(mouse.button == Qt.RightButton){
											selectAsset( index )
											assetGrid.currentIndex = index
										}
									}

									onDoubleClicked: {
										if(mouse.button == Qt.LeftButton){
											selectAsset( index )
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

							onRowClicked: {
								selectAsset( folderContentsModel.indexRow( modelIndex ) )
							}

							onRowDoubleClicked: {
								if(mouse.button == Qt.LeftButton) {
									// TODO: How do we get the item from the index?
									selectAsset( folderContentsModel.indexRow( modelIndex ) )
									onUseSelectedAsset()
								}
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
										source: "qrc:///icons/file_16x16"
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
										text: itemData.Value.filename
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

					WGExpandingRowLayout{
						Layout.fillWidth: true
						Layout.preferredHeight: defaultSpacing.minimumRowHeight
						//Active Filters, icon options

						WGToolButton {
							id: btnSaveFilters
							iconSource: "qrc:///icons/save_16x16"
							noFrame_: false

							tooltip: "Save Filters"
						}

						WGTextBoxFrame {
							//saved filter 'buttons' go here

							Layout.fillWidth: true
							Layout.preferredHeight: defaultSpacing.minimumRowHeight
						}

						WGToolButton {
							id: btnClearFilters
							iconSource: "qrc:///icons/close_16x16"
							noFrame_: false

							tooltip: "Clear Filters"
						}

						WGSeparator {
							vertical_: true
							Layout.fillHeight: false
							Layout.preferredHeight: defaultSpacing.minimumRowHeight
						}

						WGLabel {
							text: "Icon Size: "
						}

						WGSliderControl {
							//Slider that controls the size of thumbnails
							id: iconSizeSlider
							Layout.preferredWidth: 50
							label_: "Icon Size:"
							minimumValue: 32
							maximumValue: 256
							value: iconSize
							stepSize: 16
							showValue_: false
							decimals_: 0

							b_Target: rootFrame
							b_Property: "iconSize"
							b_Value: value
						}

                        //toggle between icon & list view.
                        WGDropDownBox {
                            id: listviewDisplayTypeMenu
                            Layout.preferredWidth: 100

                            model: contentDisplayType
                            currentIndex: model.currentIndex_

                            onCurrentIndexChanged: {
                                showIcons = (0 == currentIndex);
                            }

                            b_Target: contentDisplayType
                            b_Property: "currentIndex_"
                            b_Value: currentIndex
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
						// End custom content filters elements
                    }
				} //Right Hand Column Layout
			} //RightFrame
		} //SplitView
	}
}