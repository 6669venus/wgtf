import QtQuick 2.3

//An item that acts as a selectable border for copy/paste type functionality

Rectangle {
	id: copyableRoot

	//A linked copyable is a copyable placed somewhere where it won't be able to find any child copyables.
	//In this case it just finds its parent copyable and acts as a mouseover/selector for it

	property bool linkedCopyable: false
	property bool selected: linkedCopyable ? parentCopyable.selected : false

	//temporary bool for if this data has been copied
	property bool copied: false

	//non-interactive copyables can't be clicked on and won't show any visual changes but they CAN act as parent copyables for others
	property bool interactive: true

	//if a parent copyable is clicked while this is false it will SELECT all children
	//If the parent is clicked while this is true it will UNSELECT all children
	//if a child is clicked it will set this to FALSE (clicking the parent will select all again)
	property bool allSelected: false

	//This is set by a copyable above this object.
	property QtObject parentCopyable: null

	anchors.fill: parent
	anchors.leftMargin: -1
	anchors.rightMargin: -1

	//for some reason without this the control is 1 pixel too low.
	anchors.topMargin: -2

	radius: defaultSpacing.halfRadius

	color: "transparent"
	border.width: defaultSpacing.standardBorderSize
	border.color: "transparent"

	//Recursively finds a copyable child and sets this object as its parent if it doesn't have one.
	//TODO: This seems a little dependent on whether or not the lowest parent copyable runs this first... seems a bit risky.

	function setParentCopyable(parentObject){
		for(var i=0; i<parentObject.children.length; i++){
			if(typeof parentObject.children[i].parentCopyable != "undefined"){
				if(parentObject.children[i].parentCopyable == null){
					if(parentObject.children[i] != copyableRoot){
						parentObject.children[i].parentCopyable = copyableRoot
						//console.log(copyableRoot.toString() + " Child Copyable Found " + parentObject.children[i].toString())
					}
				}
			} else {
				setParentCopyable(parentObject.children[i])
			}
		}
	}

	//selects this object and toggles all selected
	function select(){
		copyableRoot.selected = true
		copyableRoot.allSelected = true
	}


	function deSelect(){
		//deselects this object
		copyableRoot.selected = false

		//if all children have been selected (recently) then deselect all via signal
		if(allSelected){
			deSelectChildren(parent)
		}

		//If this isn't a dummy copyable and it has a parent copyable... deselect it.
		if(!linkedCopyable && parentCopyable != null){
			parentCopyable.allSelected = false
			parentCopyable.deSelect()
		}

		copyableRoot.allSelected = false
	}

	//children copyables listen for this and 'can' deselect themselves if all have been selected.
	signal deSelectChildren()

	//assign child copyables this object as parent copyable
	Component.onCompleted: {
		if (!linkedCopyable){
			setParentCopyable(parent)
		}
	}

	//can't have the parent copyable selected if all children aren't selected
	onAllSelectedChanged: {
		if(!allSelected){
			copyableRoot.selected = false
		}
	}


	Connections {
		target: parentCopyable
		onSelectedChanged: {
			if(!linkedCopyable){
				//if parent is selected, select this
				if(parentCopyable.selected){
					copyableRoot.selected = true
				}
			} else {
				//two way binding for dummy copyables
				copyableRoot.selected = parentCopyable.selected
			}
		}
		//listen for parent being deselected (only if all children are selected) and pass that through to this objects children
		onDeSelectChildren: {
			deSelect()
			deSelectChildren()
		}
	}

	states: [
		State {
			//clicked state
			name: "SELECTED"
			when: copyableRoot.selected && copyableRoot.interactive
			PropertyChanges {
				target: copyableRoot
				border.color: "transparent"
				color: palette.HighlightShade
			}
		},
		State {
			//border when copy key held (Ctrl)
			name: "COPYACTIVE"
			when: !copySelect.containsMouse && !copyableRoot.selected && copyableRoot.interactive
			PropertyChanges {
				target: copyableRoot
				color: "transparent"
				border.color: palette.LighterShade
			}
		},
		State {
			//border when mouseovered
			name: "HOVERED"
			when: copySelect.containsMouse && !copyableRoot.selected && copyableRoot.interactive
			PropertyChanges {
				target: copyableRoot
				color: "transparent"
				border.color: palette.HighlightShade
			}
		}
	]

	//Click area over the control when Ctrl key is held.
	MouseArea {
		id: copySelect
		anchors.fill: copyableRoot.interactive ? parent : undefined
		enabled: copyableRoot.interactive

		hoverEnabled: copyableRoot.interactive
		cursorShape: interactive && enabled ? Qt.PointingHandCursor : Qt.ArrowCursor

		preventStealing: true

		onClicked: {
			if ((mouse.button == Qt.LeftButton)){
				if(copyableRoot.selected){
					if(linkedCopyable){
						//dummy copyable just passes this to parent
						parentCopyable.deSelect()
					} else {
						copyableRoot.deSelect()
					}
				} else {
					if(linkedCopyable){
						//dummy copyable just passes this to parent
						parentCopyable.select()
					} else {
						copyableRoot.select()
					}
				}
			}
		}
	}
}

