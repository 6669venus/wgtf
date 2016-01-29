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

			for (var i = 0; i < changes.length; i++)
			{
				var stubRow = new Array();
				stubRow[0] = String(changes[i]);
				for (var colIdx in jsonData.columns)
				{
					colIdx = parseInt( colIdx );
					var column = jsonData.columns[colIdx];
					if (type == "fps_regression")
					{
						stubRow[colIdx*2+1] = undefined;
						stubRow[colIdx*2+2] = roundFloat( column.referenceValue );
					}
					else
					{
						stubRow[colIdx+1] = undefined;
					}
				}
				
				dataTable.addRow( stubRow );
			}
			
			// Go through each column again, now populating the data.
			for (var colIdx in jsonData.columns)
			{
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
					var changelist =  row[0];
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
						
					var diff = changes.indexOf(changelist);				
					
					dataTable.setCell( diff, dataIdx, value);

					
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
				interpolateNulls: true,
				//focusTarget: "category",
				vAxis: { 
					title: CHART_TITLES[type],
					viewWindowMode: "explicit",
					viewWindow: {
						max: null,
						min: 0
					}
				},
				hAxis: { 
					title: "Test Changelist",
					textStyle: {
						fontSize: 8
					} 
				},
				chartArea: {
					width: "80%", 
					height: "70%" 
				},
				legend: { 
					position: 'bottom' 
				}
			};
			
			if (type == "fps_regression")
			{
				options.series = [
						{ color: 'blue', visibleInLegend: true, lineWidth:2 },
						{ color: 'blue', visibleInLegend: false, lineWidth:1 },
						{ color: 'green', visibleInLegend: true, lineWidth:2 },
						{ color: 'green', visibleInLegend: false, lineWidth:1 },
						{ color: 'red', visibleInLegend: true, lineWidth:1 },
						{ color: 'red', visibleInLegend: false, lineWidth:1 }
					];
			}
			
			if (type == "fps_regression")
			{
				options.vAxis.viewWindowMode = "explicit";					
			}
			else
			{
				options.vAxis.viewWindowMode = "pretty";
			}
			
			return options;
		}
		
		function loadGraph( opts )
		{
			var element = opts.element;
			var type = opts.type;
			var columns = opts.columns;
			var title = opts.title;
			var performanceTarget = opts.performanceTarget;
			var baseLine = opts.baseLine;
		
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
				if (baseLine != "0")
				{
					options.vAxis.baseline = opts.baseLine;
				}
				
				options.vAxis.baselineColor = 'red';
								
				var subDiv = document.createElement("div");
				subDiv.className = "chart-inner";				
				element.appendChild( subDiv );
				
				var maximizeButton = document.createElement( "div" );
				maximizeButton.className = "chart-maximize-button ui-icon ui-icon-zoomin";
				maximizeButton.onclick = function() {
					var popup = document.getElementById( 'popup-chart' );
					
					
					$( "#popup-chart" ).dialog({
						height: 600,
						width: "80%",
						modal: true,
						draggable:false,
						resizable:false,
						title: "fhqwhgads"
					});
					
					$(".ui-widget-overlay").click(function(){
						$(".ui-dialog-titlebar-close").trigger('click');
					});
					
					clearChildren( popup );
					
					var chart = new google.visualization.LineChart( popup );
					
					chart.draw( dataTable, options );
				};
				
				element.appendChild( maximizeButton );
				
				// All done, spawn the chart
				var chart = new google.visualization.LineChart( subDiv );
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
				var baseLine = $(this).attr( "baseLine" );
				var performanceTarget = $(this).attr( "performanceTarget" );
				if (type == undefined)
				{
					type = "fps_regression";
				}
				if (baseLine == undefined)
				{
					baseLine = "0";
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
								'baseLine':baseLine,
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
		padding-bottom: 20px;
		padding-right: 20px;
		
		width: 29%; 
		height: 200px;
		
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
	<img src="disclaimer.gif" height="60"  align="left" hspace="20"><ul>
		<li>This is our WOT_9.3 and BW_2.8 Merge performance target</li>
		<li>Zero in the graph are days on which the test failed to run.</li>
		<li>//wot/branches/merges/wot_93_bw_28_cs/</li>
		<li>32Bit Hybrid WOT client</li>
		<li>For a summary, please visit: <a href="wot_beta_9_3_summary.php">http://perf/wot_beta_9_3_summary.php</a> </li>
		<li><a href="https://confluence.wargaming.net/display/BWT/Performance+comparision+results">Information on testing methodology</a></li>
	</ul>
	</p>

	<div class="chart-category">
		<h1>Replay name: asia_korea - Frame Time</h1>
		<div class="chart" title="Frame Time Ms 1280 Res-Max baseLine=32.965" type="wot_frame_time" baseLine="32.965">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_asia_korea_max_1280" benchmark="WIN32" name="WOT merge" ></div>
		</div>
		
		<div class="chart" title="Frame Time Ms 1280 Res-Med baseLine=28.339" type="wot_frame_time" baseLine="28.339">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_asia_korea_med_1280" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Frame Time Ms 1280 Res-Low baseLine=18.28" type="wot_frame_time" baseLine="18.28">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_asia_korea_low_1280" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Frame Time Ms 1920 Res-Max baseLine=33.686" type="wot_frame_time" baseLine="33.686">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_asia_korea_max_1920" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Frame Time Ms 1920 Res-Med baseLine=28.503" type="wot_frame_time" baseLine="28.503">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_asia_korea_med_1920" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Frame Time Ms 1920 Res-low baseLine=18.401" type="wot_frame_time" baseLine="18.401">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_asia_korea_low_1920" benchmark="WIN32" name="WOT merge"></div>
		</div>
	</div>	
	<div class="chart-category">
		<h1>Replay name: asia_korea - Loadtime</h1>
		<div class="chart" title="Loadtime Sec 1280 Res-Max baseLine=8.87876" type="wot_loadtime" baseLine="8.87876">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_asia_korea_max_1280" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1280 Res-Med baseLine=8.66668" type="wot_loadtime" baseLine="8.66668">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_asia_korea_med_1280" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1280 Res-Low baseLine=8.26872" type="wot_loadtime" baseLine="8.26872">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_asia_korea_low_1280" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1920 Res-Max baseLine=8.89509" type="wot_loadtime" baseLine="8.89509">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_asia_korea_max_1920" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1920 Res-Med baseLine=8.70783" type="wot_loadtime" baseLine="8.70783">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_asia_korea_med_1920" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1920 Res-Low baseLine=7.94435" type="wot_loadtime" baseLine="7.94435">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_asia_korea_low_1920" benchmark="WIN32" name="WOT merge"></div> 
		</div>
	
	</div>	
	<div class="chart-category">	
		<h1>Replay name: asia_korea - Peak Allocated</h1>
		
		<div class="chart" title="Peak Allocated MB 1280 Res-Max baseLine=1847.45" type="wot_peak_mem" baseLine="1847.45">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_asia_korea_max_1280" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Peak Allocated MB 1280 Res-Med baseLine=1152.74" type="wot_peak_mem" baseLine="1152.74">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_asia_korea_med_1280" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Peak Allocated MB 1280 Res-Low baseLine=970.086" type="wot_peak_mem" baseLine="970.086">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_asia_korea_low_1280" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Peak Allocated MB 1920 Res-Max baseLine=1916.7" type="wot_peak_mem" baseLine="1916.7">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_asia_korea_max_1920" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Peak Allocated MB 1920 Res-Med baseLine=1209.76" type="wot_peak_mem" baseLine="1209.76">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_asia_korea_med_1920" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Peak Allocated MB 1920 Res-Low baseLine=974.643" type="wot_peak_mem" baseLine="974.643">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_asia_korea_low_1920" benchmark="WIN32" name="WOT merge"></div> 
		</div>
		
	</div>
	
	<div class="chart-category">	
		<h1>Replay name: asia_korea - Memory Allocations</h1>
		
		<div class="chart" title="Memory Allocations 1280 Res-Max baseLine=30278082" type="wot_total_mem" baseLine="30278082">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_asia_korea_max_1280" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Memory Allocations 1280 Res-Med baseLine=31182491" type="wot_total_mem" baseLine="31182491">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_asia_korea_med_1280" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Memory Allocations 1280 Res-Low baseLine=34006643" type="wot_total_mem" baseLine="34006643">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_asia_korea_low_1280" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Memory Allocations 1920 Res-Max baseLine=30144953" type="wot_total_mem" baseLine="30144953">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_asia_korea_max_1920" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Memory Allocations 1920 Res-Med baseLine=31288089" type="wot_total_mem" baseLine="31288089">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_asia_korea_med_1920" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Memory Allocations 1920 Res-Low baseLine=33833380" type="wot_total_mem" baseLine="33833380">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_asia_korea_low_1920" benchmark="WIN32" name="WOT merge"></div> 
		</div>
		
	</div>
	
	<div class="chart-category">
	</div>
	<div class="chart-category">
	</div>
	<div class="chart-category">
	</div>
	<div class="chart-category">
	</div>
	<div class="chart-category">
	</div>
	<div class="chart-category">
	</div>
	<div class="chart-category">
	</div>
	<div class="chart-category">
	</div>
	
	<div class="chart-category">
		<h1>Replay name: airfield - Frame Time</h1>
		
		<div class="chart" title="Frame Time Ms 1280 Res-Max baseLine=29.606" type="wot_frame_time" baseLine="29.606">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_airfield_max_1280" benchmark="WIN32" name="WOT merge" ></div>
		</div>
		
		<div class="chart" title="Frame Time Ms 1280 Res-Med baseLine=25.618" type="wot_frame_time" baseLine="25.618">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_airfield_med_1280" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Frame Time Ms 1280 Res-Low baseLine=16.304" type="wot_frame_time" baseLine="16.304">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_airfield_low_1280" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Frame Time Ms 1920 Res-Max baseLine=29.78" type="wot_frame_time" baseLine="29.78">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_airfield_max_1920" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Frame Time Ms 1920 Res-Med baseLine=25.747" type="wot_frame_time" baseLine="25.747">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_airfield_med_1920" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Frame Time Ms 1920 Res-Low baseLine=16.289" type="wot_frame_time" baseLine="16.289">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_airfield_low_1920" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
	</div>
	
	<div class="chart-category">	
		<h1>Replay name: airfield - Loadtime</h1>
				
		<div class="chart" title="Loadtime Sec 1280 Res-Max baseLine=7.23372" type="wot_loadtime" baseLine="7.23372">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_airfield_max_1280" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1280 Res-Med baseLine=6.51668" type="wot_loadtime" baseLine="6.51668">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_airfield_med_1280" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1280 Res-Low baseLine=7.92655" type="wot_loadtime" baseLine="7.92655">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_airfield_low_1280" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1920 Res-Max baseLine=7.19784" type="wot_loadtime" baseLine="7.19784">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_airfield_max_1920" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1920 Res-Med baseLine=6.92188" type="wot_loadtime" baseLine="6.92188">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_airfield_med_1920" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1920 Res-Low baseLine=7.74985" type="wot_loadtime" baseLine="7.74985">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_airfield_low_1920" benchmark="WIN32" name="WOT merge"></div>
		</div>
	</div>
	
	<div class="chart-category">	
		<h1>Replay name: airfield - Peak Allocated</h1>
		
		<div class="chart" title="Loadtime Sec 1280 Res-Max baseLine=1857.01" type="wot_peak_mem" baseLine="1857.01">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_airfield_max_1280" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1280 Res-Med baseLine=1097.56" type="wot_peak_mem" baseLine="1097.56">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_airfield_med_1280" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1280 Res-Low baseLine=932.391" type="wot_peak_mem" baseLine="932.391">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_airfield_low_1280" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1920 Res-Max baseLine=1926.11" type="wot_peak_mem" baseLine="1926.11">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_airfield_max_1920" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1920 Res-Med baseLine=1154.44" type="wot_peak_mem" baseLine="1154.44">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_airfield_med_1920" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1920 Res-Low baseLine=936.967" type="wot_peak_mem" baseLine="936.967">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_airfield_low_1920" benchmark="WIN32" name="WOT merge"></div>
		</div>
	</div>
	
	<div class="chart-category">	
		<h1>Replay name: airfield - Memory Allocations</h1>
				
		<div class="chart" title="Loadtime Sec 1280 Res-Max baseLine=30944427" type="wot_total_mem" baseLine="30944427">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_airfield_max_1280" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1280 Res-Med baseLine=31756267" type="wot_total_mem" baseLine="31756267">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_airfield_med_1280" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1280 Res-Low baseLine=34822031" type="wot_total_mem" baseLine="34822031">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_airfield_low_1280" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1920 Res-Max baseLine=30934160" type="wot_total_mem" baseLine="30934160">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_airfield_max_1920" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1920 Res-Med baseLine=31744323" type="wot_total_mem" baseLine="31744323">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_airfield_med_1920" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1920 Res-Low baseLine=34849226" type="wot_total_mem" baseLine="34849226">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_airfield_low_1920" benchmark="WIN32" name="WOT merge"></div>
		</div>
		

	</div>

	<div class="chart-category">
	</div>
	<div class="chart-category">
	</div>
	<div class="chart-category">
	</div>
	<div class="chart-category">
	</div>
	<div class="chart-category">
	</div>
	<div class="chart-category">
	</div>
	<div class="chart-category">
	</div>
	<div class="chart-category">
	</div>
	
	<div class="chart-category">
		<h1>Replay name: murovanka - Frame Time</h1>
		
		<div class="chart" title="Frame Time Ms 1280 Res-Max baseLine=30.78" type="wot_frame_time" baseLine="30.78">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_murovanka_max_1280" benchmark="WIN32" name="WOT merge" ></div>
		</div>
		
		<div class="chart" title="Frame Time Ms 1280 Res-Med baseLine=25.984" type="wot_frame_time" baseLine="25.984">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_murovanka_med_1280" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Frame Time Ms 1280 Res-Low baseLine=16.583" type="wot_frame_time" baseLine="16.583">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_murovanka_low_1280" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Frame Time Ms 1920 Res-Max baseLine=30.919" type="wot_frame_time" baseLine="30.919">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_murovanka_max_1920" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Frame Time Ms 1920 Res-Med baseLine=26.191" type="wot_frame_time" baseLine="26.191">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_murovanka_med_1920" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Frame Time Ms 1920 Res-Low baseLine=16.587" type="wot_frame_time" baseLine="16.587">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_murovanka_low_1920" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
	</div>
	
	<div class="chart-category">	
		<h1>Replay name: murovanka - Loadtime</h1>
				
		<div class="chart" title="Loadtime Sec 1280 Res-Max baseLine=8.75278" type="wot_loadtime" baseLine="8.75278">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_murovanka_max_1280" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1280 Res-Med baseLine=7.73105" type="wot_loadtime" baseLine="7.73105">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_murovanka_med_1280" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1280 Res-Low baseLine=8.82132" type="wot_loadtime" baseLine="8.82132">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_murovanka_low_1280" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1920 Res-Max baseLine=8.72408" type="wot_loadtime" baseLine="8.72408">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_murovanka_max_1920" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1920 Res-Med baseLine=7.88336" type="wot_loadtime" baseLine="7.88336">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_murovanka_med_1920" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1920 Res-Low baseLine=8.82251" type="wot_loadtime" baseLine="8.82251">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_murovanka_low_1920" benchmark="WIN32" name="WOT merge"></div>
		</div>
	</div>
	
	<div class="chart-category">	
		<h1>Replay name: murovanka - Peak Allocated</h1>
		
		<div class="chart" title="Loadtime Sec 1280 Res-Max baseLine=2094.59" type="wot_peak_mem" baseLine="2094.59">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_murovanka_max_1280" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1280 Res-Med baseLine=1152.89" type="wot_peak_mem" baseLine="1152.89">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_murovanka_med_1280" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1280 Res-Low baseLine=957.32" type="wot_peak_mem" baseLine="957.32">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_murovanka_low_1280" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1920 Res-Max baseLine=2163.77" type="wot_peak_mem" baseLine="2163.77">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_murovanka_max_1920" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1920 Res-Med baseLine=1209.74" type="wot_peak_mem" baseLine="1209.74">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_murovanka_med_1920" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1920 Res-Low baseLine=961.789" type="wot_peak_mem" baseLine="961.789">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_murovanka_low_1920" benchmark="WIN32" name="WOT merge"></div>
		</div>
	</div>
	
	<div class="chart-category">	
		<h1>Replay name: murovanka - Memory Allocations</h1>
				
		<div class="chart" title="Loadtime Sec 1280 Res-Max baseLine=29228798" type="wot_total_mem" baseLine="29228798">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_murovanka_max_1280" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1280 Res-Med baseLine=29785067" type="wot_total_mem" baseLine="29785067">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_murovanka_med_1280" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1280 Res-Low baseLine=31906343" type="wot_total_mem" baseLine="31906343">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_murovanka_low_1280" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1920 Res-Max baseLine=29300242" type="wot_total_mem" baseLine="29300242">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_murovanka_max_1920" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1920 Res-Med baseLine=29785177" type="wot_total_mem" baseLine="29785177">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_murovanka_med_1920" benchmark="WIN32" name="WOT merge"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1920 Res-Low baseLine=31913691" type="wot_total_mem" baseLine="31913691">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="merge_murovanka_low_1920" benchmark="WIN32" name="WOT merge"></div>
		</div>
		

	</div>
						
	<div style="clear:both"></div>
	<p>
		<small>[Page generated by <?php echo $hostname?>:<?php echo __FILE__;?>]</small>
	</p>
</body>
</html>
