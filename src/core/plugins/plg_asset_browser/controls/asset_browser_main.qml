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
	property var title: "Asset Browser"

	id: rootFrame

	color: palette.MainWindowColor
    property QtObject panelProps: WGPanelProperties{}

	//TODO Should this be stored somewhere else?
	property int iconSize: 64

	anchors.fill:parent

	
	//--------------------------------------
	// Functions
	//--------------------------------------

	// Selects an asset from the folder contents view
    function selectAsset( index ){
        CurrentSelectedAssetIndex = index;
    }

	// Tells the page to use the current selected asset
	function onApplyAsset() {
		applyAsset;
	}

	// Tells the page to navigate the history forward or backward
	// depending on what button was clicked
	function onNavigate( isForward ) {
		if (isForward) {
			navigateHistoryForward;
		}
		else {
			navigateHistoryBackward;
		}
	}

	
	//--------------------------------------
	// Folder Tree Filter & Model
	//--------------------------------------
	BWTreeFilter {
		id: filter
		source: folders
		filter: folderSearchBox.text
	}

	BWTreeModel {
		id : folderModel

		source : filter.filteredSource

		ValueExtension {}
		ColumnExtension {}
		ComponentExtension {}
		TreeExtension {}
		ThumbnailExtension {}
		SelectionExtension {}
	}


	//--------------------------------------
	// List View Model for Folder Contents
	//--------------------------------------
	BWListModel {
		id : folderContentsModel
		source : folderContents

		ValueExtension {}
	}

	
	//--------------------------------------
	// List Model for Location Breadcrumbs
	//--------------------------------------
	BWListModel {
		id: breadcrumbModel
		source: breadcrumbs

		ValueExtension {}
	}

	
	//--------------------------------------
	// View Parent Layout
	//--------------------------------------

	ColumnLayout {
		// Initial column layout with button/path bar at the top and then 
		// the split two column panel underneath it.

		id: mainColumn
		anchors.fill: parent
		anchors.margins: panelProps.standardMargin_

		WGExpandingRowLayout {
			// Button Bar then Breadcrumbs/Path
			id: assetBrowserInfo
			Layout.fillWidth: true
			Layout.preferredHeight: panelProps.rowHeight_ + panelProps.doubleBorder_

			// Tool Buttons:
			WGToolButton {
				id: btnAssetBrowserNewAsset
				iconSource: "qrc:///icons/add_16x16"
				noFrame_: false
				tooltip: "New Asset"

				menu: Menu {
					title: "Create New:"

					MenuSeparator { }

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
				iconSource: "qrc:///icons/open_16x16"
				noFrame_: false
				tooltip: "Import Asset"
			}

			WGSeparator {
				vertical_: true
				Layout.fillHeight: false
				Layout.preferredHeight: panelProps.rowHeight_
			}

			WGToolButton {
				id: btnAssetBrowserMoveToCollection
				iconSource: "qrc:///icons/play_16x16"
				noFrame_: false
				tooltip: "Add Asset to Collection"

				menu: Menu {
					title: "Add Selected To:"

					MenuSeparator { }

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
				iconSource: "qrc:///icons/copy_small_16x16"
				noFrame_: false
				tooltip: "Select All Instances"
			}

			WGToolButton {
				id: btnAssetBrowserReplaceAll
				iconSource: "qrc:///icons/paste_16x16"
				noFrame_: false
				tooltip: "Replace All Instances"
			}

			WGSeparator {
				vertical_: true
				Layout.fillHeight: false
				Layout.preferredHeight: panelProps.rowHeight_
			}

			//Breadcrumbs and browsing

			WGToolButton {
				id: btnAssetBrowserBack
				iconSource: "qrc:///icons/arrow2_down_16x16"
				noFrame_: false
				tooltip: "Back"

				onClicked: {
					onNavigate( false );
				}
			}

			WGToolButton {
				id: btnAssetBrowserForward
				iconSource: "qrc:///icons/arrow2_up_16x16"
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
				Layout.preferredHeight: panelProps.rowHeight_
				Layout.fillWidth: true
				color: "transparent"

				RowLayout {

					id: breadcrumbLayout
					anchors.fill: parent

					Component {
						id: breadcrumbDelegate
						
						WGLabel {
							id: breadcrumbLabel

							Layout.fillWidth: false
							Layout.preferredHeight: panelProps.rowHeight_

							elide: Text.ElideRight

							text: Value

							font.bold: true
							font.pointSize: 12

							MouseArea {
								id: breadcrumbMouseArea
								anchors.fill: parent
								cursorShape: Qt.PointingHandCursor
								onPressed: {
									// TODO: Will need a proper method call here to
									//       navigate the asset tree location from
									//       the selected breadcrumb.
									console.log("You have clicked " + Value)
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
						Layout.preferredHeight: panelProps.rowHeight_ + panelProps.doubleBorder_

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
				iconSource: "qrc:///icons/loop_16x16"
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
				iconSource: "qrc:///icons/hide_16x16"
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
				id: btnApplyAsset
				iconSource: "qrc:///icons/list_plus_16x16"
				noFrame_: false

				tooltip: "Apply Asset"

				onClicked: {
					onApplyAsset()
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
				width: panelProps.doubleMargin_
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
						Layout.preferredHeight: panelProps.rowHeight_ + panelProps.doubleBorder_

						WGToolButton {
							id: btnOpenAssetLocation
							iconSource: "qrc:///icons/arrow_down_small_16x16"
							noFrame_: false

							tooltip: "Collection Options"

							menu: Menu {
								title: "Collections"
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
							Layout.preferredHeight: panelProps.rowHeight_
							placeholderText: "Search"
						}
					}

					// TODO Set this up to use tabs for different collections.
					// This will need a proper TabViewStyle made though

					Rectangle {
						//Tree View Frame

						color: palette.DarkShade

						Layout.fillHeight: true
						Layout.fillWidth: true

						// Folder Structure TreeView

						TreeView {
							model_ : folderModel
							anchors.fill: parent
							anchors.margins: panelProps.standardMargin_
							columnCount_ : 1
							property Component propertyDelegate : Loader {
								clip : true
								sourceComponent : itemData_ != null ? itemData_.Component : null
							}
							columnDelegates_ : [ columnDelegate_, propertyDelegate ]
							clampWidth_ : true
						}
					}
				}
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
						Layout.preferredHeight: panelProps.rowHeight_ + panelProps.doubleBorder_


						WGToolButton {
							//Save filters and load previous filters
							id: btnListviewFilters
							iconSource: "qrc:///icons/arrow_down_small_16x16"
							noFrame_: false

							tooltip: "Filter Options"


							menu: Menu {
								title: "Filters"
								MenuItem {
									text: "Save Filter..."
								}

								MenuItem {
									text: "Clear Filters"
								}

								MenuSeparator { }

								MenuItem {
									text: "Saved Filters:"
								}

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

						//TODO Make filters filter things
						WGTextBox {
							id: listviewSearchBox
							Layout.fillWidth: true
							placeholderText: "Filter"
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

						WGSubScrollPanel {
							anchors.fill: parent
							clip: true

							childObject_: GridLayout {
								//Grid of assets.

								//Check if it even handles lots of items?
								id: assetGrid
								columnSpacing: 5
								rowSpacing: 5

								width: folderContentsRect.width

								columns: width / (iconSize + 5)
								rows: 10

								Repeater {
									model: folderContentsModel
									delegate: folderContentsDelegate
								}
							}
						}

						Component {
							id: folderContentsDelegate
							//Individual grid file/Asset. Height/Width determined by iconSize from iconSizeSlider

							Rectangle {
								id: assetEntryRect
								Layout.preferredWidth: iconSize + panelProps.standardMargin_ * 2

								//TODO: This extra height seems a bit arbitrary. Might not handle long labels.

								//Need to get the required height for the longest label in each row somehow?
								//Clip if the label is over a certain length?

								Layout.preferredHeight: iconSize + panelProps.rowHeight_ * 2

								color: {
									if (index == CurrentSelectedAssetIndex) {
										return Qt.rgba(33, 33, 33, 0.2);
									}				
									else {
										return "transparent";
									}
								}

								ColumnLayout {
									spacing: 2
									anchors.fill: parent
									anchors.margins: panelProps.standardMargin_

									//TODO Replace this with proper thumbnail
									Rectangle {
										Layout.preferredHeight: iconSize
										Layout.preferredWidth: iconSize
										Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
										color: "lightblue"
									}

									WGMultiLineText {
										id: iconLabel
										text: Value.filename
										horizontalAlignment: Text.AlignHCenter
										Layout.preferredHeight: iconSize + panelProps.rowHeight_
										Layout.preferredWidth: iconSize
										Layout.fillHeight: true

										wrapMode: Text.WrapAnywhere
									}

									MouseArea {
										id: assetMouseArea
										anchors.fill: parent
										cursorShape: Qt.PointingHandCursor
										onPressed: {
											selectAsset( index )
										}
									}
								}
							}
						}

					} //Asset Icon Frame

					WGExpandingRowLayout{
						Layout.fillWidth: true
						Layout.preferredHeight: panelProps.rowHeight_
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
							Layout.preferredHeight: panelProps.rowHeight_
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
							Layout.preferredHeight: panelProps.rowHeight_
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

						//toggle between icon & list view. Low Priority
						WGDropDownBox {
							id: listviewDisplayTypeMenu
							Layout.preferredWidth: 100
							model: ["Icons","List View"]
						}
					}
				} //Right Hand Column Layout
			} //RightFrame
		} //SplitView
	}
}
