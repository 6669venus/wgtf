import QtQuick 2.4
import QtQuick.Window 2.2
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import BWControls 1.0
import WGControls 1.0


Rectangle {
	id: curveEditor
	property string title: "Curve Editor"
	property var layoutHints: { 'curveeditor': 1.0, 'bottom': 0.5 }

	property var selection: [];

	Layout.fillHeight: true
	color: palette.MainWindowColor

	QtObject
	{
		id: internal
		property var colors: []
		property var positions: []
		property bool gradientEnabled: curveRepeater.count == 3 || curveRepeater.count == 4
	}

	function addPointsToCurves(mouse, updateGradient)
	{
		var pos = timeline.viewTransform.inverseTransform(Qt.point(mouse.x, mouse.y))
		pos.x = Math.max(pos.x, 0)
		pos.x = Math.min(pos.x, 1)
		
		beginUndoFrame();
		var curveIt = iterator(curves)
		while(curveIt.moveNext()){
			curveIt.current.addAt( pos.x, true )
		}
		if(updateGradient)
		{
			updateColorGradient();
		}
		endUndoFrame();
	}

	function clearSelection()
	{
		for(var index = 0; index < curveRepeater.count; ++index)
		{
			var currentCurve = curveRepeater.itemAt(index);
			for(var i = 0; i < currentCurve.pointRepeater.count; ++i)
			{
				currentCurve.pointRepeater.itemAt(i).selected = false;
			}
		}
	}

	function curveRemoved(item)
	{
		var newSelection = []
		for(var i = 0; i < selection.length; ++i)
		{
			var point = selection[i]
			if(point.parentCurve != item && point.selected)
			{
				newSelection.push(point)
			}
		}
		selection = newSelection
	}

	function deletePointsAt(valuesToDelete, updateGradient)
	{
		if(valuesToDelete.length > 0)
		{
			beginUndoFrame();
			for(var i = 0; i < valuesToDelete.length; ++i)
			{
				var curveIt = iterator(curves)
				while(curveIt.moveNext()){
					curveIt.current.removeAt( valuesToDelete[i], true );
				}
			}
			if(updateGradient)
			{
				updateColorGradient();
			}
			endUndoFrame();
		}
	}

	function deleteSelected()
	{
		var curveIt = iterator(curves)
		var index = 0;
		var valuesToDelete = []
		while(curveIt.moveNext()){
			var currentCurve = curveRepeater.itemAt(index++);
			for(var i = 0; i < currentCurve.pointRepeater.count; ++i){
				var point = currentCurve.pointRepeater.itemAt(i);
				if(point.selected){
					point.selected = false;
					valuesToDelete.push(point.point.pos.x);
				}
			}
		}
		deletePointsAt(valuesToDelete, true);
	}

	function pointSelectionChanged(point)
	{
		var newSelection = []
		for(var i = 0; i < selection.length; ++i)
		{
			if(selection[i].selected)
				newSelection.push(selection[i])
		}
		// Point is being selected, add it to our collection
		if(point.selected)
		{
			newSelection.push(point)
		}
		selection = newSelection;
	}

	function binarySearch(ar, el) {
		var m = 0;
		var n = ar.length - 1;
		while (m <= n) {
			var k = (n + m) >> 1;
			var cmp = el - ar[k];
			if (cmp > 0) {
				m = k + 1;
			} else if(cmp < 0) {
				n = k - 1;
			} else {
				return k;
			}
		}
		return -m - 1;
	}

	function pointPositionChanged(point, xDelta, yDelta)
	{
		var movedX = [point.point.pos.x, point.point.pos.x - xDelta]
		movedX.sort();
		for(var i = 0; i < selection.length; ++i)
		{
			var selectedPoint = selection[i]
			// Shift the other selected points
			if(selectedPoint !== point)
			{
				var newX = selectedPoint.point.pos.x + xDelta;
				var newY = selectedPoint.point.pos.y + yDelta;
				// For now points with the same x's are moved in C++ Particle Editor code
				// TODO: Make this code generic for the curve editor
				var index = binarySearch(movedX, selectedPoint.point.pos.x)
				if(index >= 0)
				{
					// Only move y value, x will already get updated
					newX = selectedPoint.point.pos.x
				}
				else
				{
					// Add the old x value 
					movedX.splice(-index - 1, 0, selectedPoint.point.pos.x)
				}

				selectedPoint.setPosition(newX, newY);

				// Add the new x value after it has been constrained
				index = binarySearch(movedX, selectedPoint.point.pos.x)
				if(index < 0)
				{
					movedX.splice(-index - 1, 0, selectedPoint.point.pos.x)
				}
			}
		}
		updateColorGradient();
	}

	function repaintCurves() {
		for(var i = 0; i < curveRepeater.count; ++i) {
			curveRepeater.itemAt(i).requestPaint()
		}
	}

	function toggleCurve(index) {
		var curve = curveRepeater.itemAt(index)
		if(curve) {
			curve.enabled = !curve.enabled;
			curve.requestPaint()
		}
	}

	function curveEnabled(index) {
		var curve = curveRepeater.itemAt(index)
		return curveRepeater.count > index && curve && curve.enabled;
	}

	function updateColorGradient()
	{
		if(!internal.gradientEnabled)
			return;

		var oldCount = internal.colors.length

		var newColors = [];
		var newPositions = [];
		var red = curveRepeater.itemAt(0);
		var green = curveRepeater.itemAt(1);
		var blue = curveRepeater.itemAt(2);
		var alpha = curveRepeater.count == 4 ? curveRepeater.itemAt(3)
			: {
				pointRepeater:{
					count: 1,
					itemAt: function(index){ return {point:{pos:{y:1.0}}}; }
				}};
		for(var i = 0; i < red.pointRepeater.count; ++i)
		{
			newColors.push(Qt.rgba(
							  red.pointRepeater.itemAt(i).point.pos.y,
							  green.pointRepeater.itemAt(i).point.pos.y,
							  blue.pointRepeater.itemAt(i).point.pos.y,
							  alpha.pointRepeater.itemAt(i%alpha.pointRepeater.count).point.pos.y))
			newPositions.push(red.pointRepeater.itemAt(i).point.pos.x)
		}
		internal.colors = newColors;
		internal.positions = newPositions;

		colorGradient.updateData();
	}

	ColumnLayout{
		id: contents
		spacing: 2
		anchors.fill: parent

		CurveEditorToolbar {
			id: toolbar

			title: subTitle
			time:  selection.length > 0 ? selection[selection.length-1].point.pos.x : 0
			value:  selection.length > 0 ? selection[selection.length-1].point.pos.y : 0
			timeScale: xScale
			valueScale: yScale
			editEnabled: selection.length > 0

			onToggleX: toggleCurve(0)
			onToggleY: toggleCurve(1)
			onToggleZ: toggleCurve(2)
			onToggleW: toggleCurve(3)
			onTimeScaleChanged: xScale = timeScale;
			onValueScaleChanged: yScale = valueScale;

			onUnscaledTimeChanged:
			{
				if(selection.length > 0)
				{
					var point = selection[selection.length-1];
					point.setPosition(time, point.point.pos.y)
				}
			}

			onUnscaledValueChanged:
			{
				if(selection.length > 0)
				{
					var point = selection[selection.length-1];
					point.setPosition(point.point.pos.x, value)
				}
			}
			// HACK: Due to not currently getting change notifications when points are modified in C++
			// We use this timer to update the text boxes periodically with the selected point's values
			Timer
			{
				interval: 100
				running: selection.length > 0
				repeat: true
				onTriggered: {
					if(selection.length > 0)
					{
						toolbar.time = selection[selection.length-1].point.pos.x;
						toolbar.value = selection[selection.length-1].point.pos.y;
					}
					else
					{
						toolbar.time = 0;
						toolbar.value = 0;
					}
				}
			}
		}

		Timeline {
			id:timeline
			focus: true
			timeScale: xScale
			valueScale: yScale

			// Zoom to the extents of the curve, always zooms the full X axis and zooms to the available y extremes
			function zoomExtents()
			{
				// Calculate the y extremes
				var yMin = undefined;
				var yMax = undefined;
				for(var index = 0; index < curveRepeater.count; ++index){
					var currentCurve = curveRepeater.itemAt(index);
					for(var i = 0; i < currentCurve.pointRepeater.count; ++i){
						var point = currentCurve.pointRepeater.itemAt(i);
						if(yMin === undefined || point.point.pos.y < yMin){
							yMin = point.point.pos.y;
						}
						if(yMax === undefined || point.point.pos.y > yMax){
							yMax = point.point.pos.y;
						}
					}
				}

				// If there were no points on any curves zoom to the full y extents
				if(yMin === undefined)
				{
					yMin = 0;
					yMax = 1;
				}

				// If the y extremes are the same make sure we have something to zoom to centering the points
				if(yMin === yMax)
				{
					yMax += 0.5;
					yMin -= 0.5;
				}

				var topMargin = .1
				var botMargin = .9
				var leftMargin = .05
				var rightMargin = .9

				// Here we solve for the yScale and origin.y necessary to zoom to our extents.
				// We want to have our yMax/yMin values a comfortable distance from the top/bottom of the timeline
				// This gives us two equations and two unknowns (yScale, origin.y)
				// yMax * yScale + origin.y = timeline.height * topMargin
				// yMin * yScale + origin.y = timeline.height * botMargin
				// Solve for origin.y in terms of yScale
				//  origin.y = (timeline.height * botMargin) - (yMin * yScale)
				// Now solve for yScale
				//  yMax * yScale = (timeline.height * topMargin) - origin.y
				//  yScale = ((timeline.height * topMargin) - origin.y) / yMax
				// Substitute origin.y
				//  yScale = ((timeline.height * topMargin) - ((timeline.height * botMargin) - (yMin * yScale))) / yMax
				//  yScale = ((timeline.height * topMargin) - (timeline.height * botMargin) + (yMin * yScale)) / yMax
				//  yScale = (timeline.height * topMargin)/yMax - (timeline.height * botMargin)/yMax + (yMin * yScale)/yMax
				//  yScale - (yMin * yScale)/yMax = (timeline.height/yMax) * (topMargin - botMargin)
				//  yScale * (1 - yMin/yMax) = (timeline.height/yMax) * (topMargin - botMargin)
				//  yScale = ((timeline.height/yMax) * (topMargin - botMargin)) / (1 - yMin/yMax)

				// Because we divide by yMax it must not be zero
				if(yMax === 0)
					yMax = 0.001
				var yViewScale = ((topMargin - botMargin)*timeline.height/yMax) / (1 - yMin/yMax)
				// Use a margin so xMin/xMax are a comfortable distance from the left/right of the timeline
				timeline.viewTransform.origin.x = timeline.width * leftMargin;
				timeline.viewTransform.origin.y = (timeline.height * botMargin) - (yMin * yViewScale)
				timeline.viewTransform.xScale = timeline.width * rightMargin;
				timeline.viewTransform.yScale = yViewScale;
				timeline.requestPaint();
			}

			Keys.onPressed:{
				if(event.key === Qt.Key_E && event.modifiers & Qt.ControlModifier){
					timeline.zoomExtents();
				}
				else if(event.key === Qt.Key_Delete)
				{
					curveEditor.deleteSelected();
				}
				else if(event.key === Qt.Key_X)
				{
					toggleCurve(0)
				}
				else if(event.key === Qt.Key_Y)
				{
					toggleCurve(1)
				}
				else if(event.key === Qt.Key_Z)
				{
					toggleCurve(2)
				}
				else if(event.key === Qt.Key_W)
				{
					toggleCurve(3)
				}
			}

			// Data model coming from C++
			WGListModel
			{
				id: curvesModel
				source: curves
				ValueExtension {}
			}

			SelectionArea
			{
				onSelectArea:{
					min = timeline.viewTransform.inverseTransform(min)
					max = timeline.viewTransform.inverseTransform(max)
					var addToSelection = (mouse.modifiers & Qt.ControlModifier)
					for(var index = 0; index < curveRepeater.count; ++index)
					{
						var currentCurve = curveRepeater.itemAt(index);
						if(!currentCurve.enabled)
							continue;
						for(var i = 0; i < currentCurve.pointRepeater.count; ++i)
						{
							var point = currentCurve.pointRepeater.itemAt(i)
							var pos = point.point.pos
							// Invert y to account for window coordinates
							var contained = pos.x >= min.x && pos.x <= max.x && pos.y <= min.y && pos.y >= max.y;
							if(contained)
								point.selected = true;
							else if(!addToSelection)
								point.selected = false;
						}
					}
				}
			}

			MouseArea{
				anchors.fill: parent
				acceptedButtons: Qt.AllButtons
				onPressed:{
					// Focus the timeline for keyboard shortcuts
					timeline.forceActiveFocus(Qt.MouseFocusReason);
					mouse.accepted = false;

					if(mouse.button == Qt.LeftButton)
					{
						if(mouse.modifiers & Qt.AltModifier)
						{
							addPointsToCurves(mouse, true)
							mouse.accepted = true;
						}
						else if(!(mouse.modifiers & Qt.ControlModifier))
						{
							clearSelection()
						}
					}
				}
			}
			
			Repeater
			{
				id: curveRepeater
				model: curvesModel
				delegate: Curve{
					points: Value.points
					curveModel: Value
					viewTransform: timeline.viewTransform;
					Component.onCompleted:{
						// Assign and don't bind the color, otherwise we get qml errors when the curvesModel changes
						// Should we update colors when the curvesModel Changes?
						color = ["#b45b4e", "#99dc74", "#92cfdd", "#808080"][index%4]
					}
					onPointSelectionChanged:{
						curveEditor.pointSelectionChanged(point);
					}
					onPointPositionChanged:{
						curveEditor.pointPositionChanged(point, xDelta, yDelta)
					}
					onPointPressed:{
						if(point.selected === false && mouse.modifiers !== Qt.ControlModifier)
						{
							clearSelection();
						}
					}
					onPointClicked:{
						if(mouse.modifiers !== Qt.ControlModifier)
						{
							clearSelection();
						}
					}
				}

				onItemRemoved:
				{
					curveEditor.curveRemoved(item);
				}
			}

			CurveEditorContextMenu {}

			// Commenting this out until multiple gradient stops work correctly
			// Also need signals to enable handling changes to update the curve data
			WGColorSlider {
				id: colorGradient
				visible: internal.gradientEnabled
				anchors.bottom: timeline.bottom
				x: timeline.viewTransform.transformX(0)
				width: timeline.viewTransform.transformX(1) - timeline.viewTransform.transformX(0)
				height: 10
				minimumValue: 0
				maximumValue: 1.0
				stepSize: .001
				colorData: internal.colors
				positionData: internal.positions
				linkColorsToHandles: true

				onVisibleChanged: {
					// When initially made visible update the gradient
					if(visible)
						curveEditor.updateColorGradient();
				}

				//onWidthChanged: {
				//	// Until the color slider supports resizing re-create the handles
				//	curveEditor.updateColorGradient();
				//}

				onChangeValue: {
					if(!internal.gradientEnabled)
						return;

					// The handle index is the point index
					var index = handleIndex
					var red = curveRepeater.itemAt(0).getPoint(index);
					var green = curveRepeater.itemAt(1).getPoint(index);
					var blue = curveRepeater.itemAt(2).getPoint(index);
					var alpha = curveRepeater.count == 4 ?
								curveRepeater.itemAt(3).getPoint(index) : null
					beginUndoFrame()
					red.pos.x = val
					green.pos.x = val
					blue.pos.x = val
					if(alpha){
						alpha.pos.x = val
					}
					endUndoFrame()
					repaintCurves()
				}

				onColorModified: {
					var color = colorGradient.colorData[index]
					beginUndoFrame()
					var red = curveRepeater.itemAt(0).getPoint(index);
					var green = curveRepeater.itemAt(1).getPoint(index);
					var blue = curveRepeater.itemAt(2).getPoint(index);
					var alpha = curveRepeater.count == 4 ?
								curveRepeater.itemAt(3).getPoint(index) : null

					red.pos.y = color.r
					green.pos.y = color.g
					blue.pos.y = color.b
					if(alpha){
						alpha.pos.y = color.a
					}
					endUndoFrame()
					repaintCurves()
				}

				onPointAdded: {
					var color = colorGradient.colorData[index]
					var relPos = colorGradient.positionData[index]
					var mousePos = timeline.viewTransform.transformX(relPos)
					beginUndoFrame()
					curveEditor.addPointsToCurves(Qt.point(mousePos,0), false)
					var red = curveRepeater.itemAt(0).getPoint(index);
					var green = curveRepeater.itemAt(1).getPoint(index);
					var blue = curveRepeater.itemAt(2).getPoint(index);
					var alpha = curveRepeater.count == 4 ?
								curveRepeater.itemAt(3).getPoint(index) : null

					red.pos.y = color.r
					green.pos.y = color.g
					blue.pos.y = color.b
					if(alpha){
						alpha.pos.y = color.a
					}
					endUndoFrame()
				}

				onPointRemoved: {
					var red = curveRepeater.itemAt(0).getPoint(index);
					curveEditor.deletePointsAt([red.pos.x], false);
				}
			}
		}
	}
}
