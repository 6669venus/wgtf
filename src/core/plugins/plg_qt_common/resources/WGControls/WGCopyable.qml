import QtQuick 2.3

//An item that acts as a selectable border for copy/paste type functionality

Rectangle {
	id: copyable

	property bool selected: false

	property int childrenCount: 0
    property int selectedChildrenCount: 0

    property QtObject rootCopyable: parentCopyable === null ? copyable : parentCopyable.rootCopyable
	//This is set by a copyable above this object.
	property QtObject parentCopyable: null

    anchors.fill: visible ? parent : undefined
	anchors.leftMargin: -1
	anchors.rightMargin: -1

	//for some reason without this the control is 1 pixel too low.
	anchors.topMargin: -2

	radius: defaultSpacing.halfRadius

	color: "transparent"
	border.width: defaultSpacing.standardBorderSize
    border.color: "transparent"

    enabled: parent.enabled
    visible: parent.visible

	//Recursively finds a copyable child and sets this object as its parent if it doesn't have one.
	//TODO: This seems a little dependent on whether or not the lowest parent copyable runs this first... seems a bit risky.
    function setParentCopyable(parentObject){
        if((parentObject === null))
        {
            return;
        }
        var found = false;
		for(var i=0; i<parentObject.children.length; i++){
            if(typeof parentObject.children[i].parentCopyable !== "undefined"){
                if(parentObject.children[i].enabled)
                {
                    if(parentObject.children[i] !== copyable){
						if(parentObject.children[i].parentCopyable === null)
                        {
							parentObject.children[i].parentCopyable = copyable
							copyable.childrenCount++;
                            found = true;
						}
					}
                }
            }
		}
        if(!found)
        {
            for(var i=0; i<parentObject.children.length; i++){
                if(typeof parentObject.children[i].parentCopyable !== "undefined"){
                    continue;
                } else {
                    setParentCopyable(parentObject.children[i])
                }
            }
        }
    }

	//Recursively finds a copyable child and disable them, as this object
	// will be copied as a whole instead of copying its children separately
    function disableChildrenCopyable(parentObject){
        if((parentObject === null))
        {
            return;
        }
        for(var i=0; i<parentObject.children.length; i++){
            if(typeof parentObject.children[i].parentCopyable !== "undefined"){
                if(parentObject.children[i].enabled)
                {
                    if(parentObject.children[i] !== copyable){
                        parentObject.children[i].enabled = false;
                         parentObject.children[i].visible = false;
                    }
                }
            } else {
                disableChildrenCopyable( parentObject.children[i] );
            }
        }
    }


	//selects this object and toggles all selected
	function select(){
        copyable.selected = true
        selectChildren(copyable)
	}


	function deSelect(){
        //deselects this object
        copyable.selected = false
        //deselects all children
        deSelectChildren(copyable)
	}

    //children copyables listen for this and 'can' deselect themselves if all have been selected.
    signal selectChildren()
    signal deSelectChildren()

    onSelectedChanged: {
        if(parentCopyable === null)
        {
            return;
        }

        if(!selected)
        {
            parentCopyable.selected = false;
            parentCopyable.selectedChildrenCount--;
        }
        else
        {
            parentCopyable.selectedChildrenCount++;
            if(parentCopyable.selectedChildrenCount === parentCopyable.childrenCount)
            {
                parentCopyable.selected = true;
            }
        }
    }

	Connections {
		target: parentCopyable
        onSelectChildren: {
            select();
		}
		//listen for parent being deselected (only if all children are selected) and pass that through to this objects children
		onDeSelectChildren: {
            deSelect()
        }
	}

	states: [
		State {
			//clicked state
			name: "SELECTED"
            when: copyable.selected && copyable.enabled && copyable.visible
			PropertyChanges {
				target: copyable
				border.color: "transparent"
				color: palette.HighlightShade
			}
		},
		State {
			//border when copy key held (Ctrl)
			name: "COPYACTIVE"
            when: !copySelect.containsMouse && !copyable.selected
                  && copyable.enabled && copyable.visible
                  && globalSettings.wgCopyableEnabled
			PropertyChanges {
				target: copyable
				color: "transparent"
				border.color: palette.LighterShade
			}
		},
		State {
			//border when mouseovered
			name: "HOVERED"
            when: copySelect.containsMouse && !copyable.selected
                  && copyable.enabled && copyable.visible
                  && globalSettings.wgCopyableEnabled
			PropertyChanges {
				target: copyable
				color: "transparent"
				border.color: palette.HighlightShade
			}
		}
	]

	//Click area over the control when Ctrl key is held.
	MouseArea {
		id: copySelect
        anchors.fill: copyable.visible ? parent : undefined
        enabled: copyable.enabled && copyable.visible && globalSettings.wgCopyableEnabled

        hoverEnabled: copyable.enabled && copyable.visible
        cursorShape: hoverEnabled ? Qt.PointingHandCursor : Qt.ArrowCursor

		preventStealing: true

		onClicked: {
			if ((mouse.button == Qt.LeftButton) && (mouse.modifiers & Qt.ControlModifier)){
				if(copyable.selected){
						copyable.deSelect()
				} else {
						copyable.select()
				}
			}
			else if (mouse.button == Qt.LeftButton)
            {
                copyable.rootCopyable.deSelect();
				copyable.select();
			}
		}
	}
}

