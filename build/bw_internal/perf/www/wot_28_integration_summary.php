<?php
if (function_exists( "gethostname" ))
{
	$hostname = gethostname();
}
else
{
	$hostname = php_uname( 'n' );
}
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
   "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
<head>
	<link rel="stylesheet" href="themes/ui-lightness/jquery.ui.all.css">
	<script type="text/javascript" src="https://www.google.com/jsapi"></script>
	<script type="text/javascript" src="jquery-1.7.2.min.js"></script>
	<script type="text/javascript" src="jquery-ui-1.8.21.custom.min.js"></script>
	<script type="text/javascript">
		"use strict";
		google.load( "visualization", "1", {packages:["corechart"]} );				
		google.setOnLoadCallback( loadGraphs );
		
		var CHART_TITLES = {
			"fps_regression": "Frame Time (ms)",
			"fps_absolute": "Frame Time (ms)",
			"wot_frame_time": "Frame Time (ms)",
			"wot_total_mem": "Memory Allocations",
			"loadtime": "Load Time (sec)",
			"peak_mem_usage": "Peak Memory Usage (MB)",
			"total_mem_allocs": "Memory Allocations"
		}
		
		function roundFloat( v )
		{
			return parseFloat( v.toFixed(2) );
		}
								
		function perfDataURL( type, columns )
		{
			var url = 'perf_results_json.php?type='+type+'&';
			var count = 0;
			for (var i = 0, column; column = columns[i]; i++)
			{
				if (count > 0)
				{
					url += '&';
				}
				
				var joinedColumns = column.hostname + ',' + column.configuration + ',' + column.executable + ',' + 
									column.branch + ',' + column.benchmark + ',' + column.name;
				
				url += "column[]=" + encodeURIComponent( joinedColumns );
				
				count += 1;
			}			
		
			return url;
		}
		
		function validateColumns( columns )
		{
			if (columns.length == 0)
			{
				return false;
			}
			else
			{
				return true;
			}
		}
		
		function clearChildren( element )
		{
			if ( element.hasChildNodes() )
			{
				while (element.childNodes.length > 0)
				{
					element.removeChild( element.firstChild );
				} 
			}
		}
		
		function makeDataTable( opts )
		{
			var type = opts.type;
			var jsonData = opts.jsonData;
			var performanceTarget = opts.performanceTarget;
			var dataTable = new google.visualization.DataTable();
			// Add changelist column at index 0
			dataTable.addColumn( {type: 'string', label: 'changelist', id: 'changelist', role: 'domain'} );
			
			// Insert the columns (no data yet)
			// Calculate min/max vertical bounds at this point
			for (var colIdx in jsonData.columns)
			{
				colIdx = parseInt( colIdx );
				var column = jsonData.columns[colIdx];		
				dataTable.addColumn( {type:"number", label:column.name, id:column.name} );
			}
			
			// Insert changes, and stub out values to undefined.
			var changes = jsonData.changelist;

			var stubRow = new Array();
			stubRow[0] = undefined;//String(changes[changes.length-1]);
			for (var colIdx in jsonData.columns)
			{
				colIdx = parseInt( colIdx );
				stubRow[colIdx+1] = undefined;
			}
			
			dataTable.addRow( stubRow );
				
			// Go through each column again, now populating the data.
			for (var colIdx in jsonData.columns)
			{
				console.log(colIdx);
				colIdx = parseInt( colIdx );
				var column = jsonData.columns[colIdx];
				
				var dataIdx = colIdx+1;
				if (type == "fps_regression")
				{
					dataIdx = (colIdx*2)+1;
				}
				
				for (var rowIdx in column.data)
				{
					var row = column.data[rowIdx];
					var value = 0;
					
					if (type == "fps_regression")
					{
						value = roundFloat( row[1] - column.referenceValue );
					}
					else if (type == "loadtime" || type == "fps_absolute")
					{
						value = roundFloat( row[1] );
					}
					else if (type == "peak_mem_usage")
					{
						value = roundFloat( row[1] / (1024*1024) );
					}
					else if (type == "total_mem_allocs")
					{
						value = roundFloat( row[1] );
					}
					else
					{
						value = roundFloat( row[1] );
					}	
					
					dataTable.setCell( 0, dataIdx, value);
					
				}
			}
			console.log(type);
			return dataTable;
		}
		
		function makeOptions( type, title )
		{
			var options = {
				curveType: "linear",
				title: title,
				pointSize : 7,
				fontSize: 12,
				orientation: 'horizontal',
				interpolateNulls: true,
				//focusTarget: "category",
				bar: {groupWidth: "80%"},
				vAxis: { 
					title: CHART_TITLES[type],
					viewWindowMode: "explicit",
					viewWindow: {
						max: null,
						min: 0
					}
				},
				hAxis: { 
					// title: "Test Changelist",
					textStyle: {
						fontSize: 7
					},
					textPosition: 'none',
					direction: -1
				},
				chartArea: {
					width: "90%", 
					height: "80%" 
				},
				legend: { 
					position: 'bottom' 
				}
			};
			
			
			options.series = [
					{ color: '#202020', visibleInLegend: true, lineWidth:2 },
					{ color: '#0B614B', visibleInLegend: true, lineWidth:2 },
					{ color: '#202020', visibleInLegend: true, lineWidth:2 },
					{ color: '#0B614B', visibleInLegend: true, lineWidth:2 },
					{ color: '#202020', visibleInLegend: true, lineWidth:2 },
					{ color: '#0B614B', visibleInLegend: true, lineWidth:2 },
					{ color: '#202020', visibleInLegend: true, lineWidth:2 },
					{ color: '#0B614B', visibleInLegend: true, lineWidth:2 },
					{ color: '#202020', visibleInLegend: true, lineWidth:2 },
					{ color: '#0B614B', visibleInLegend: true, lineWidth:2 },
					{ color: '#202020', visibleInLegend: true, lineWidth:2 },
					{ color: '#0B614B', visibleInLegend: true, lineWidth:2 },
					{ color: '#202020', visibleInLegend: true, lineWidth:2 },
					{ color: '#0B614B', visibleInLegend: true, lineWidth:2 },
					{ color: '#202020', visibleInLegend: true, lineWidth:2 },
					{ color: '#0B614B', visibleInLegend: true, lineWidth:2 }
				];			
			
			options.vAxis.viewWindowMode = "pretty";
			
			return options;
		}
		
		function loadGraph( opts )
		{
			var element = opts.element;
			var type = opts.type;
			var columns = opts.columns;
			var title = opts.title;
			var performanceTarget = opts.performanceTarget;
		
			var url = perfDataURL( type, columns );
			// console.log( "loadGraph", columns, url );
			
			$.getJSON( url, function(jsonData) 
			{
				// console.log("URL:" + url)
				// console.log("JSON Data: " + JSON.stringify(jsonData))
				var dataTable = makeDataTable( {'type': type, 
												'jsonData':jsonData, 
												'performanceTarget':performanceTarget} );

				var options = makeOptions( type, title + "\nLast updated " +  jsonData.dateMax[0] +  "/" + jsonData.dateMax[1] +  "/" + jsonData.dateMax[2] );
								
				var subDiv = document.createElement("div");
				subDiv.className = "chart-inner";				
				element.appendChild( subDiv );
				
				var maximizeButton = document.createElement( "div" );
				maximizeButton.className = "chart-maximize-button ui-icon ui-icon-zoomin";
				maximizeButton.onclick = function() {
					var popup = document.getElementById( 'popup-chart' );
					
					
					$( "#popup-chart" ).dialog({
						height: 600,
						width: "90%",
						modal: true,
						draggable:false,
						resizable:false,
						title: "fhqwhgads"
					});
					
					$(".ui-widget-overlay").click(function(){
						$(".ui-dialog-titlebar-close").trigger('click');
					});
					
					clearChildren( popup );
					
					var chart = new google.visualization.ColumnChart( popup );
					
					chart.draw( dataTable, options );
				};
				
				element.appendChild( maximizeButton );
				
				// All done, spawn the chart
				var chart = new google.visualization.ColumnChart( subDiv );
				chart.draw( dataTable, options );
				
			} );
		}
		
		function loadGraphs()
		{
			$(".chart").each( function()
			{
				var columns = [];
				
				var title = $(this).attr( "title" );
				var type = $(this).attr( "type" );
				var performanceTarget = $(this).attr( "performanceTarget" );
				if (type == undefined)
				{
					type = "fps_regression";
				}
				
				
				$(this).find(".column").each( function() 
				{
					columns.push( {
						hostname: $(this).attr( "hostname" ),
						configuration: $(this).attr( "configuration" ),
						executable: $(this).attr( "executable" ),
						branch: $(this).attr( "branch" ),
						benchmark: $(this).attr( "benchmark" ),
						name: $(this).attr( "name" )
					} );
				} );
				
				if (validateColumns( columns ))
				{
					loadGraph( {'element':this, 
								'type':type,
								'columns':columns, 
								'title':title, 
								'performanceTarget':performanceTarget} );
				}
				else
				{
					$(this).innerHTML = "<div class='chart-error'>Invalid columns: " + columns + "</div>";
				}
			} );
		}
	</script>
	
	<style>
	<!--
	body
	{
		font-family: Arial;
	}
	
	div.chart
	{
		position:relative;
		background-image:url('ajax-loader.gif');
		background-repeat: no-repeat;
		background-position: center center;
		border: solid silver 1px;
		margin: 1em;
		// padding-bottom: 20px;
		// padding-right: 20px;
		
		width: 48%; 
		height: 400px;
		
		float: left;
	}
	
	div.chart-inner
	{
		height: 100%;
	}
	
	div.chart-maximize-button
	{
		position:absolute;
		top:0;
		right:0;		
		cursor:pointer;
	}
	
	div.chart-category
	{
		clear: both;
		border-bottom: solid black 2px;
	}
	
	.ui-resizable-helper
	{
		border: 2px dotted #00F;
	}
	
	#blanket 
	{
		background-color:#111;
		opacity: 0.65;
		position:absolute;
		z-index: 9001; /*ooveeerrrr nine thoussaaaannnd*/
		top:0px;
		left:0px;
		width:100%;
	}
	
	.ui-dialog .ui-dialog-content
	{
		padding:0;
	}
	
	.ui-dialog-titlebar 
	{ 
		display: none; 
	}
	-->
	</style>
</head>

<body>
	<div id="blanket" style="display:none;"></div>
	<div id="popup-chart" style="display:none;">		
	</div>	
	
	<h1>Current Target</h1>
	<p>
	<ul>
		<li>This is our WOT trunk and BW 2.8 integration performance target</li>
		<li>//wot/remote/branches/bigworldtech/wot_trunk_bwclient_2.8_bwserver_2.9_integration/</li>
		<li>32Bit Hybrid WOT client</li>
		<li>it's showing only last results</li>
		<li>For full data, please visit: <a href="wot_28_integration.php">https://perf/wot_28_integration.php</a> </li>
		<li>//wot/remote/trunk/ @188962 is used as a baseline</li>
	</ul>
	</p>
	
	<div class="chart-category">	
		<h1><a href="http://ci.bigworldtech.com/job/test_wot_28_integration_memory_testing05/">Cat tests Testing05-i7</a></h1>	
				
		<div class="chart" title="Available Virtual Memory (KB), 17_munchen " type="wot_total_mem">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_max_1280_17_munchen" benchmark="WIN32" name="baseline_max_1280" ></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1280_17_munchen" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_med_1280_17_munchen" benchmark="WIN32" name="baseline_med_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1280_17_munchen" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1280_17_munchen" benchmark="WIN32" name="baseline_low_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_17_munchen" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_max_1920_17_munchen" benchmark="WIN32" name="baseline_max_1920" ></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1920_17_munchen" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_med_1920_17_munchen" benchmark="WIN32" name="baseline_med_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1920_17_munchen" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1920_17_munchen" benchmark="WIN32" name="baseline_low_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_17_munchen" benchmark="WIN32" name="low_1920"></div>
		</div>
		
		<div class="chart" title="Frame Time - cat test, 17_munchen" type="wot_frame_time">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_max_1280_17_munchen" benchmark="WIN32" name="baseline_max_1280" ></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1280_17_munchen" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_med_1280_17_munchen" benchmark="WIN32" name="baseline_med_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1280_17_munchen" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1280_17_munchen" benchmark="WIN32" name="baseline_low_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_17_munchen" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_max_1920_17_munchen" benchmark="WIN32" name="baseline_max_1920" ></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1920_17_munchen" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_med_1920_17_munchen" benchmark="WIN32" name="baseline_med_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1920_17_munchen" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1920_17_munchen" benchmark="WIN32" name="baseline_low_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_17_munchen" benchmark="WIN32" name="low_1920"></div>
		</div>
		
		<div class="chart" title="Available Virtual Memory (KB), 23_westfeld " type="wot_total_mem">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_max_1280_23_westfeld" benchmark="WIN32" name="baseline_max_1280" ></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1280_23_westfeld" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_med_1280_23_westfeld" benchmark="WIN32" name="baseline_med_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1280_23_westfeld" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1280_23_westfeld" benchmark="WIN32" name="baseline_low_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_23_westfeld" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_max_1920_23_westfeld" benchmark="WIN32" name="baseline_max_1920" ></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1920_23_westfeld" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_med_1920_23_westfeld" benchmark="WIN32" name="baseline_med_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1920_23_westfeld" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1920_23_westfeld" benchmark="WIN32" name="baseline_low_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_23_westfeld" benchmark="WIN32" name="low_1920"></div>
		</div>
		
		<div class="chart" title="Frame Time - cat test, 23_westfeld" type="wot_frame_time">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_max_1280_23_westfeld" benchmark="WIN32" name="baseline_max_1280" ></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1280_23_westfeld" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_med_1280_23_westfeld" benchmark="WIN32" name="baseline_med_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1280_23_westfeld" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1280_23_westfeld" benchmark="WIN32" name="baseline_low_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_23_westfeld" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_max_1920_23_westfeld" benchmark="WIN32" name="baseline_max_1920" ></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1920_23_westfeld" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_med_1920_23_westfeld" benchmark="WIN32" name="baseline_med_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1920_23_westfeld" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1920_23_westfeld" benchmark="WIN32" name="baseline_low_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_23_westfeld" benchmark="WIN32" name="low_1920"></div>
		</div>
		
		<div class="chart" title="Available Virtual Memory (KB), 28_desert " type="wot_total_mem">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_max_1280_28_desert" benchmark="WIN32" name="baseline_max_1280" ></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1280_28_desert" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_med_1280_28_desert" benchmark="WIN32" name="baseline_med_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1280_28_desert" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1280_28_desert" benchmark="WIN32" name="baseline_low_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_28_desert" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_max_1920_28_desert" benchmark="WIN32" name="baseline_max_1920" ></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1920_28_desert" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_med_1920_28_desert" benchmark="WIN32" name="baseline_med_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1920_28_desert" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1920_28_desert" benchmark="WIN32" name="baseline_low_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_28_desert" benchmark="WIN32" name="low_1920"></div>
		</div>
		
		<div class="chart" title="Frame Time - cat test, 28_desert" type="wot_frame_time">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_max_1280_28_desert" benchmark="WIN32" name="baseline_max_1280" ></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1280_28_desert" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_med_1280_28_desert" benchmark="WIN32" name="baseline_med_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1280_28_desert" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1280_28_desert" benchmark="WIN32" name="baseline_low_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_28_desert" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_max_1920_28_desert" benchmark="WIN32" name="baseline_max_1920" ></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1920_28_desert" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_med_1920_28_desert" benchmark="WIN32" name="baseline_med_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1920_28_desert" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1920_28_desert" benchmark="WIN32" name="baseline_low_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_28_desert" benchmark="WIN32" name="low_1920"></div>
		</div>
		
		
		<div class="chart" title="Available Virtual Memory (KB), 87_ruinberg_on_fire " type="wot_total_mem">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_max_1280_87_ruinberg_on_fire" benchmark="WIN32" name="baseline_max_1280" ></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1280_87_ruinberg_on_fire" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_med_1280_87_ruinberg_on_fire" benchmark="WIN32" name="baseline_med_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1280_87_ruinberg_on_fire" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1280_87_ruinberg_on_fire" benchmark="WIN32" name="baseline_low_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_87_ruinberg_on_fire" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_max_1920_87_ruinberg_on_fire" benchmark="WIN32" name="baseline_max_1920" ></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1920_87_ruinberg_on_fire" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_med_1920_87_ruinberg_on_fire" benchmark="WIN32" name="baseline_med_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1920_87_ruinberg_on_fire" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1920_87_ruinberg_on_fire" benchmark="WIN32" name="baseline_low_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_87_ruinberg_on_fire" benchmark="WIN32" name="low_1920"></div>
		</div>
		
		<div class="chart" title="Frame Time - cat test, 87_ruinberg_on_fire" type="wot_frame_time">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_max_1280_87_ruinberg_on_fire" benchmark="WIN32" name="baseline_max_1280" ></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1280_87_ruinberg_on_fire" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_med_1280_87_ruinberg_on_fire" benchmark="WIN32" name="baseline_med_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1280_87_ruinberg_on_fire" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1280_87_ruinberg_on_fire" benchmark="WIN32" name="baseline_low_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_87_ruinberg_on_fire" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_max_1920_87_ruinberg_on_fire" benchmark="WIN32" name="baseline_max_1920" ></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1920_87_ruinberg_on_fire" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_med_1920_87_ruinberg_on_fire" benchmark="WIN32" name="baseline_med_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1920_87_ruinberg_on_fire" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1920_87_ruinberg_on_fire" benchmark="WIN32" name="baseline_low_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_87_ruinberg_on_fire" benchmark="WIN32" name="low_1920"></div>
		</div>
	</div>	
	
	<div class="chart-category">	
		<h1><a href="http://ci.bigworldtech.com/job/test_wot_28_integration_memory_testing06/">Cat tests Testing06-i7 AMD</a></h1>
			<div class="chart" title="Available Virtual Memory (KB), 17_munchen " type="wot_total_mem">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_max_1280_17_munchen" benchmark="WIN32" name="baseline_max_1280" ></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1280_17_munchen" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_med_1280_17_munchen" benchmark="WIN32" name="baseline_med_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1280_17_munchen" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1280_17_munchen" benchmark="WIN32" name="baseline_low_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_17_munchen" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_max_1920_17_munchen" benchmark="WIN32" name="baseline_max_1920" ></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1920_17_munchen" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_med_1920_17_munchen" benchmark="WIN32" name="baseline_med_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1920_17_munchen" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1920_17_munchen" benchmark="WIN32" name="baseline_low_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_17_munchen" benchmark="WIN32" name="low_1920"></div>
		</div>
		
		<div class="chart" title="Frame Time - cat test, 17_munchen" type="wot_frame_time">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_max_1280_17_munchen" benchmark="WIN32" name="baseline_max_1280" ></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1280_17_munchen" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_med_1280_17_munchen" benchmark="WIN32" name="baseline_med_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1280_17_munchen" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1280_17_munchen" benchmark="WIN32" name="baseline_low_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_17_munchen" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_max_1920_17_munchen" benchmark="WIN32" name="baseline_max_1920" ></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1920_17_munchen" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_med_1920_17_munchen" benchmark="WIN32" name="baseline_med_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1920_17_munchen" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1920_17_munchen" benchmark="WIN32" name="baseline_low_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_17_munchen" benchmark="WIN32" name="low_1920"></div>
		</div>
		
		<div class="chart" title="Available Virtual Memory (KB), 23_westfeld " type="wot_total_mem">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_max_1280_23_westfeld" benchmark="WIN32" name="baseline_max_1280" ></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1280_23_westfeld" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_med_1280_23_westfeld" benchmark="WIN32" name="baseline_med_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1280_23_westfeld" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1280_23_westfeld" benchmark="WIN32" name="baseline_low_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_23_westfeld" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_max_1920_23_westfeld" benchmark="WIN32" name="baseline_max_1920" ></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1920_23_westfeld" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_med_1920_23_westfeld" benchmark="WIN32" name="baseline_med_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1920_23_westfeld" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1920_23_westfeld" benchmark="WIN32" name="baseline_low_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_23_westfeld" benchmark="WIN32" name="low_1920"></div>
		</div>
		
		<div class="chart" title="Frame Time - cat test, 23_westfeld" type="wot_frame_time">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_max_1280_23_westfeld" benchmark="WIN32" name="baseline_max_1280" ></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1280_23_westfeld" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_med_1280_23_westfeld" benchmark="WIN32" name="baseline_med_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1280_23_westfeld" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1280_23_westfeld" benchmark="WIN32" name="baseline_low_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_23_westfeld" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_max_1920_23_westfeld" benchmark="WIN32" name="baseline_max_1920" ></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1920_23_westfeld" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_med_1920_23_westfeld" benchmark="WIN32" name="baseline_med_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1920_23_westfeld" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1920_23_westfeld" benchmark="WIN32" name="baseline_low_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_23_westfeld" benchmark="WIN32" name="low_1920"></div>
		</div>
		
		<div class="chart" title="Available Virtual Memory (KB), 28_desert " type="wot_total_mem">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_max_1280_28_desert" benchmark="WIN32" name="baseline_max_1280" ></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1280_28_desert" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_med_1280_28_desert" benchmark="WIN32" name="baseline_med_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1280_28_desert" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1280_28_desert" benchmark="WIN32" name="baseline_low_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_28_desert" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_max_1920_28_desert" benchmark="WIN32" name="baseline_max_1920" ></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1920_28_desert" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_med_1920_28_desert" benchmark="WIN32" name="baseline_med_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1920_28_desert" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1920_28_desert" benchmark="WIN32" name="baseline_low_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_28_desert" benchmark="WIN32" name="low_1920"></div>
		</div>
		
		<div class="chart" title="Frame Time - cat test, 28_desert" type="wot_frame_time">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_max_1280_28_desert" benchmark="WIN32" name="baseline_max_1280" ></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1280_28_desert" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_med_1280_28_desert" benchmark="WIN32" name="baseline_med_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1280_28_desert" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1280_28_desert" benchmark="WIN32" name="baseline_low_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_28_desert" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_max_1920_28_desert" benchmark="WIN32" name="baseline_max_1920" ></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1920_28_desert" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_med_1920_28_desert" benchmark="WIN32" name="baseline_med_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1920_28_desert" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1920_28_desert" benchmark="WIN32" name="baseline_low_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_28_desert" benchmark="WIN32" name="low_1920"></div>
		</div>
		
		
		<div class="chart" title="Available Virtual Memory (KB), 87_ruinberg_on_fire " type="wot_total_mem">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_max_1280_87_ruinberg_on_fire" benchmark="WIN32" name="baseline_max_1280" ></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1280_87_ruinberg_on_fire" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_med_1280_87_ruinberg_on_fire" benchmark="WIN32" name="baseline_med_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1280_87_ruinberg_on_fire" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1280_87_ruinberg_on_fire" benchmark="WIN32" name="baseline_low_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_87_ruinberg_on_fire" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_max_1920_87_ruinberg_on_fire" benchmark="WIN32" name="baseline_max_1920" ></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1920_87_ruinberg_on_fire" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_med_1920_87_ruinberg_on_fire" benchmark="WIN32" name="baseline_med_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1920_87_ruinberg_on_fire" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1920_87_ruinberg_on_fire" benchmark="WIN32" name="baseline_low_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_87_ruinberg_on_fire" benchmark="WIN32" name="low_1920"></div>
		</div>
		
		<div class="chart" title="Frame Time - cat test, 87_ruinberg_on_fire" type="wot_frame_time">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_max_1280_87_ruinberg_on_fire" benchmark="WIN32" name="baseline_max_1280" ></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1280_87_ruinberg_on_fire" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_med_1280_87_ruinberg_on_fire" benchmark="WIN32" name="baseline_med_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1280_87_ruinberg_on_fire" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1280_87_ruinberg_on_fire" benchmark="WIN32" name="baseline_low_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_87_ruinberg_on_fire" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_max_1920_87_ruinberg_on_fire" benchmark="WIN32" name="baseline_max_1920" ></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1920_87_ruinberg_on_fire" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_med_1920_87_ruinberg_on_fire" benchmark="WIN32" name="baseline_med_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1920_87_ruinberg_on_fire" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1920_87_ruinberg_on_fire" benchmark="WIN32" name="baseline_low_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_87_ruinberg_on_fire" benchmark="WIN32" name="low_1920"></div>
		</div>
	</div>	
	
	
	<div class="chart-category">	
		<h1><a href="http://ci.bigworldtech.com/job/test_wot_28_integration_memory_testing07/">Cat tests Testing07-Core 2 Duo</a></h1>
		<div class="chart" title="Available Virtual Memory (KB), 17_munchen " type="wot_total_mem">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_max_1280_17_munchen" benchmark="WIN32" name="baseline_max_1280" ></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1280_17_munchen" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_med_1280_17_munchen" benchmark="WIN32" name="baseline_med_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1280_17_munchen" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1280_17_munchen" benchmark="WIN32" name="baseline_low_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_17_munchen" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_max_1920_17_munchen" benchmark="WIN32" name="baseline_max_1920" ></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1920_17_munchen" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_med_1920_17_munchen" benchmark="WIN32" name="baseline_med_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1920_17_munchen" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1920_17_munchen" benchmark="WIN32" name="baseline_low_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_17_munchen" benchmark="WIN32" name="low_1920"></div>
		</div>
		
		<div class="chart" title="Frame Time - cat test, 17_munchen" type="wot_frame_time">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_max_1280_17_munchen" benchmark="WIN32" name="baseline_max_1280" ></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1280_17_munchen" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_med_1280_17_munchen" benchmark="WIN32" name="baseline_med_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1280_17_munchen" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1280_17_munchen" benchmark="WIN32" name="baseline_low_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_17_munchen" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_max_1920_17_munchen" benchmark="WIN32" name="baseline_max_1920" ></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1920_17_munchen" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_med_1920_17_munchen" benchmark="WIN32" name="baseline_med_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1920_17_munchen" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1920_17_munchen" benchmark="WIN32" name="baseline_low_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_17_munchen" benchmark="WIN32" name="low_1920"></div>
		</div>
		
		<div class="chart" title="Available Virtual Memory (KB), 23_westfeld " type="wot_total_mem">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_max_1280_23_westfeld" benchmark="WIN32" name="baseline_max_1280" ></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1280_23_westfeld" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_med_1280_23_westfeld" benchmark="WIN32" name="baseline_med_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1280_23_westfeld" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1280_23_westfeld" benchmark="WIN32" name="baseline_low_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_23_westfeld" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_max_1920_23_westfeld" benchmark="WIN32" name="baseline_max_1920" ></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1920_23_westfeld" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_med_1920_23_westfeld" benchmark="WIN32" name="baseline_med_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1920_23_westfeld" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1920_23_westfeld" benchmark="WIN32" name="baseline_low_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_23_westfeld" benchmark="WIN32" name="low_1920"></div>
		</div>
		
		<div class="chart" title="Frame Time - cat test, 23_westfeld" type="wot_frame_time">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_max_1280_23_westfeld" benchmark="WIN32" name="baseline_max_1280" ></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1280_23_westfeld" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_med_1280_23_westfeld" benchmark="WIN32" name="baseline_med_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1280_23_westfeld" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1280_23_westfeld" benchmark="WIN32" name="baseline_low_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_23_westfeld" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_max_1920_23_westfeld" benchmark="WIN32" name="baseline_max_1920" ></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1920_23_westfeld" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_med_1920_23_westfeld" benchmark="WIN32" name="baseline_med_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1920_23_westfeld" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1920_23_westfeld" benchmark="WIN32" name="baseline_low_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_23_westfeld" benchmark="WIN32" name="low_1920"></div>
		</div>
		
		<div class="chart" title="Available Virtual Memory (KB), 28_desert " type="wot_total_mem">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_max_1280_28_desert" benchmark="WIN32" name="baseline_max_1280" ></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1280_28_desert" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_med_1280_28_desert" benchmark="WIN32" name="baseline_med_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1280_28_desert" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1280_28_desert" benchmark="WIN32" name="baseline_low_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_28_desert" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_max_1920_28_desert" benchmark="WIN32" name="baseline_max_1920" ></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1920_28_desert" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_med_1920_28_desert" benchmark="WIN32" name="baseline_med_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1920_28_desert" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1920_28_desert" benchmark="WIN32" name="baseline_low_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_28_desert" benchmark="WIN32" name="low_1920"></div>
		</div>
		
		<div class="chart" title="Frame Time - cat test, 28_desert" type="wot_frame_time">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_max_1280_28_desert" benchmark="WIN32" name="baseline_max_1280" ></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1280_28_desert" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_med_1280_28_desert" benchmark="WIN32" name="baseline_med_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1280_28_desert" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1280_28_desert" benchmark="WIN32" name="baseline_low_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_28_desert" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_max_1920_28_desert" benchmark="WIN32" name="baseline_max_1920" ></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1920_28_desert" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_med_1920_28_desert" benchmark="WIN32" name="baseline_med_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1920_28_desert" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1920_28_desert" benchmark="WIN32" name="baseline_low_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_28_desert" benchmark="WIN32" name="low_1920"></div>
		</div>
		
		
		<div class="chart" title="Available Virtual Memory (KB), 87_ruinberg_on_fire " type="wot_total_mem">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_max_1280_87_ruinberg_on_fire" benchmark="WIN32" name="baseline_max_1280" ></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1280_87_ruinberg_on_fire" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_med_1280_87_ruinberg_on_fire" benchmark="WIN32" name="baseline_med_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1280_87_ruinberg_on_fire" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1280_87_ruinberg_on_fire" benchmark="WIN32" name="baseline_low_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_87_ruinberg_on_fire" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_max_1920_87_ruinberg_on_fire" benchmark="WIN32" name="baseline_max_1920" ></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1920_87_ruinberg_on_fire" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_med_1920_87_ruinberg_on_fire" benchmark="WIN32" name="baseline_med_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1920_87_ruinberg_on_fire" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1920_87_ruinberg_on_fire" benchmark="WIN32" name="baseline_low_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_87_ruinberg_on_fire" benchmark="WIN32" name="low_1920"></div>
		</div>
		
		<div class="chart" title="Frame Time - cat test, 87_ruinberg_on_fire" type="wot_frame_time">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_max_1280_87_ruinberg_on_fire" benchmark="WIN32" name="baseline_max_1280" ></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1280_87_ruinberg_on_fire" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_med_1280_87_ruinberg_on_fire" benchmark="WIN32" name="baseline_med_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1280_87_ruinberg_on_fire" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1280_87_ruinberg_on_fire" benchmark="WIN32" name="baseline_low_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_87_ruinberg_on_fire" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_max_1920_87_ruinberg_on_fire" benchmark="WIN32" name="baseline_max_1920" ></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1920_87_ruinberg_on_fire" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_med_1920_87_ruinberg_on_fire" benchmark="WIN32" name="baseline_med_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1920_87_ruinberg_on_fire" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1920_87_ruinberg_on_fire" benchmark="WIN32" name="baseline_low_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_87_ruinberg_on_fire" benchmark="WIN32" name="low_1920"></div>
		</div>	
	</div>	
	
	<div class="chart-category">	
		<h1><a href="http://ci.bigworldtech.com/job/test_wot_28_integration_memory_testing08/">Cat tests Testing08-i5</a></h1>
		<div class="chart" title="Available Virtual Memory (KB), 17_munchen " type="wot_total_mem">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1280_17_munchen" benchmark="WIN32" name="baseline_low_1280"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_17_munchen" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1920_17_munchen" benchmark="WIN32" name="baseline_low_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_17_munchen" benchmark="WIN32" name="low_1920"></div>
		</div>
		
		<div class="chart" title="Frame Time - cat test, 17_munchen" type="wot_frame_time">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1280_17_munchen" benchmark="WIN32" name="baseline_low_1280"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_17_munchen" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1920_17_munchen" benchmark="WIN32" name="baseline_low_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_17_munchen" benchmark="WIN32" name="low_1920"></div>
		</div>
		
		<div class="chart" title="Available Virtual Memory (KB), 23_westfeld " type="wot_total_mem">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1280_23_westfeld" benchmark="WIN32" name="baseline_low_1280"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_23_westfeld" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1920_23_westfeld" benchmark="WIN32" name="baseline_low_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_23_westfeld" benchmark="WIN32" name="low_1920"></div>
		</div>
		
		<div class="chart" title="Frame Time - cat test, 23_westfeld" type="wot_frame_time">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1280_23_westfeld" benchmark="WIN32" name="baseline_low_1280"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_23_westfeld" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1920_23_westfeld" benchmark="WIN32" name="baseline_low_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_23_westfeld" benchmark="WIN32" name="low_1920"></div>
		</div>
		
		<div class="chart" title="Available Virtual Memory (KB), 28_desert " type="wot_total_mem">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1280_28_desert" benchmark="WIN32" name="baseline_low_1280"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_28_desert" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1920_28_desert" benchmark="WIN32" name="baseline_low_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_28_desert" benchmark="WIN32" name="low_1920"></div>
		</div>
		
		<div class="chart" title="Frame Time - cat test, 28_desert" type="wot_frame_time">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1280_28_desert" benchmark="WIN32" name="baseline_low_1280"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_28_desert" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1920_28_desert" benchmark="WIN32" name="baseline_low_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_28_desert" benchmark="WIN32" name="low_1920"></div>
		</div>
		
		
		<div class="chart" title="Available Virtual Memory (KB), 87_ruinberg_on_fire " type="wot_total_mem">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1280_87_ruinberg_on_fire" benchmark="WIN32" name="baseline_low_1280"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_87_ruinberg_on_fire" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1920_87_ruinberg_on_fire" benchmark="WIN32" name="baseline_low_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_87_ruinberg_on_fire" benchmark="WIN32" name="low_1920"></div>
		</div>
		
		<div class="chart" title="Frame Time - cat test, 87_ruinberg_on_fire" type="wot_frame_time">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1280_87_ruinberg_on_fire" benchmark="WIN32" name="baseline_low_1280"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_87_ruinberg_on_fire" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="trunk_FREE_MEM_low_1920_87_ruinberg_on_fire" benchmark="WIN32" name="baseline_low_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_87_ruinberg_on_fire" benchmark="WIN32" name="low_1920"></div>
		</div>
	</div>	
		
		
	<div style="clear:both"></div>
	
</body>
</html>
