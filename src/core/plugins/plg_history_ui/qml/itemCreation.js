
// Create components for a display item in the history list,
// depending on which properties the display item has added to it.
function createObjects( owner ) {

	{
		var component = Qt.createComponent( "WGHistoryImage.qml",
			Qt.Asynchronous,
			owner );
		if (component.status == Component.Loading) {
			component.statusChanged.connect(
				finishImageCreation( component, owner ) );
		}
		else {
			finishImageCreation( component, owner );
		}
	}

	{
		var component = Qt.createComponent( "WGHistoryText.qml",
			Qt.Asynchronous,
			owner );
		if (component.status == Component.Loading) {
			component.statusChanged.connect(
				finishTextCreation( component, owner ) );
		}
		else {
			finishTextCreation( component, owner );
		}
	}
}


// Callback to create object once the component has loaded
function finishImageCreation( component, owner ) {

	if (component.status == Component.Error) {
		console.log( "Error creating component:",
			component.errorString() );
		return;
	}
	if (component.status != Component.Ready) {
		return;
	}

	// Object is attached to owner
	if (owner.columnIndex == 0) {
		var object = component.createObject( owner,
		{
			"type": Qt.binding(
				function() { return owner.displayObject.Type; } ),
			"isApplied": Qt.binding( function() { return owner.isApplied; } )
		} );
	}
}


// Callback to create object once the component has loaded
function finishTextCreation( component, owner ) {

	if (component.status == Component.Error) {
		console.log( "Error creating component:",
			component.errorString() );
		return;
	}
	if (component.status != Component.Ready) {
		return;
	}

	// Create components for properties available on the display object
	// Object is attached to owner
	if (typeof owner.displayObject.Name != 'undefined') {
		var object = component.createObject( owner,
		{
			"text": Qt.binding(
				function() { return owner.displayObject.Name; } ),
			"isCurrentItem": Qt.binding(
				function() { return owner.ListView.isCurrentItem; } ),
			"isApplied": Qt.binding( function() { return owner.isApplied; } )
		} );
	}
	else
	{
		var object = component.createObject( owner,
		{
			"text": "Unknown",
			"isCurrentItem": Qt.binding(
				function() { return owner.ListView.isCurrentItem; } ),
			"isApplied": Qt.binding( function() { return owner.isApplied; } )
		} );
	}

	if (typeof owner.displayObject.PreValue != 'undefined') {
		var object = component.createObject( owner,
		{
			"text": Qt.binding(
				function() { return owner.displayObject.PreValue; } ),
			"isCurrentItem": Qt.binding(
				function() { return owner.ListView.isCurrentItem; } ),
			"isApplied": Qt.binding( function() { return owner.isApplied; } )
		} );
	}

	if (typeof owner.displayObject.PostValue != 'undefined') {
		var object = component.createObject( owner,
		{
			"text": Qt.binding(
				function() { return owner.displayObject.PostValue; } ),
			"isCurrentItem": Qt.binding(
				function() { return owner.ListView.isCurrentItem; } ),
			"isApplied": Qt.binding( function() { return owner.isApplied; } )
		} );
	}
}

