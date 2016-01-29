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
	
	<p>
	<img src="disclaimer.gif" height="60"  align="left" hspace="20"><ul>
		<li>This is our WOT_9.3 and BW_2.8 performance target</li>
		<li>32Bit Hybrid WOT client</li>
		<li>Zero in the graph are days on which the test failed to run.</li>
		<li>For a summary, please visit: <a href="wot_decoupled_summary.php">https://perf/wot_decoupled_summary.php</a> </li>
		<li><a href="https://confluence.wargaming.net/display/BWT/Performance+comparision+results">Information on testing methodology</a></li>
		<li>//wot/branches/merges/wot_9.3_decoupled_bwclient_2.8_bwserver_2.9/</li>
		<li>//wot/branches/features/performance_markup/beta_9.3 #179754 is used as a baseline (red line)</li>
	</ul>
	</p>
	
	<div class="chart-category">
		<h1><a href="http://ci.bigworldtech.com/job/test_wot_93_decoupled_performance_test/">Replay name: asia_korea - Frame Time Testing05-i7</a></h1>
		<h2><a href="http://ci.bigworldtech.com/job/test_wot_93_decoupled_performance_chunk_testing05/">Chunk job</a></h2>
		<div class="chart" title="Frame Time Ms 1280 Res-Max baseLine=32.84" type="wot_frame_time" baseLine="32.84">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_asia_korea_max_1280" benchmark="WIN32" name="WOT decoupled" ></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_asia_korea_max_1280" benchmark="WIN32" name="Chunk" ></div>
		</div>
		
		<div class="chart" title="Frame Time Ms 1280 Res-Med baseLine=28.13" type="wot_frame_time" baseLine="28.13">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_asia_korea_med_1280" benchmark="WIN32" name="WOT decoupled"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_asia_korea_med_1280" benchmark="WIN32" name="Chunk"></div>
		</div>
		
		<div class="chart" title="Frame Time Ms 1280 Res-Low baseLine=18.29" type="wot_frame_time" baseLine="18.29">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_asia_korea_low_1280" benchmark="WIN32" name="WOT decoupled"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_asia_korea_low_1280" benchmark="WIN32" name="Chunk"></div>
		</div>
		
		<div class="chart" title="Frame Time Ms 1920 Res-Max baseLine=32.52" type="wot_frame_time" baseLine="32.52">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_asia_korea_max_1920" benchmark="WIN32" name="WOT decoupled"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_asia_korea_max_1920" benchmark="WIN32" name="Chunk"></div>
		</div>
		
		<div class="chart" title="Frame Time Ms 1920 Res-Med baseLine=27.97" type="wot_frame_time" baseLine="27.97">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_asia_korea_med_1920" benchmark="WIN32" name="WOT decoupled"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_asia_korea_med_1920" benchmark="WIN32" name="Chunk"></div>
		</div>
		
		<div class="chart" title="Frame Time Ms 1920 Res-low baseLine=18.19" type="wot_frame_time" baseLine="18.19">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_asia_korea_low_1920" benchmark="WIN32" name="WOT decoupled"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_asia_korea_low_1920" benchmark="WIN32" name="Chunk"></div>
		</div>
	</div>	
	<div class="chart-category">
		<h1><a href="http://ci.bigworldtech.com/job/test_wot_93_decoupled_performance_test/">Replay name: asia_korea - Loadtime Testing05-i7</a></h1>
		<h2><a href="http://ci.bigworldtech.com/job/test_wot_93_decoupled_performance_chunk_testing05/">Chunk job</a></h2>
		<div class="chart" title="Loadtime Sec 1280 Res-Max baseLine=9" type="wot_loadtime" baseLine="9">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_asia_korea_max_1280" benchmark="WIN32" name="WOT decoupled" ></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_asia_korea_max_1280" benchmark="WIN32" name="Chunk" ></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1280 Res-Med baseLine=8.71" type="wot_loadtime" baseLine="8.71">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_asia_korea_med_1280" benchmark="WIN32" name="WOT decoupled"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_asia_korea_med_1280" benchmark="WIN32" name="Chunk"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1280 Res-Low baseLine=8.1" type="wot_loadtime" baseLine="8.1">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_asia_korea_low_1280" benchmark="WIN32" name="WOT decoupled"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_asia_korea_low_1280" benchmark="WIN32" name="Chunk"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1920 Res-Max baseLine=8.86" type="wot_loadtime" baseLine="8.86">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_asia_korea_max_1920" benchmark="WIN32" name="WOT decoupled"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_asia_korea_max_1920" benchmark="WIN32" name="Chunk"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1920 Res-Med baseLine=8.49" type="wot_loadtime" baseLine="8.49">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_asia_korea_med_1920" benchmark="WIN32" name="WOT decoupled"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_asia_korea_med_1920" benchmark="WIN32" name="Chunk"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1920 Res-Low baseLine=8.1" type="wot_loadtime" baseLine="8.1">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_asia_korea_low_1920" benchmark="WIN32" name="WOT decoupled"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_asia_korea_low_1920" benchmark="WIN32" name="Chunk"></div>
		</div>
	
	</div>


	<div class="chart-category">
		<h1><a href="http://ci.bigworldtech.com/job/test_wot_93_decoupled_performance_testing06/">Replay name: asia_korea - Frame Time Testing06-i7 AMD</a></h1>
		<h2><a href="http://ci.bigworldtech.com/job/test_wot_93_decoupled_performance_chunk_testing06/">Chunk job</a></h2>
		<div class="chart" title="Frame Time Ms 1280 Res-Max baseLine=34.45" type="wot_frame_time" baseLine="34.45">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_asia_korea_max_1280" benchmark="WIN32" name="WOT decoupled" ></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_asia_korea_max_1280" benchmark="WIN32" name="Chunk" ></div>
		</div>
		
		<div class="chart" title="Frame Time Ms 1280 Res-Med baseLine=29.56" type="wot_frame_time" baseLine="29.56">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_asia_korea_med_1280" benchmark="WIN32" name="WOT decoupled"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_asia_korea_med_1280" benchmark="WIN32" name="Chunk"></div>
		</div>
		
		<div class="chart" title="Frame Time Ms 1280 Res-Low baseLine=19.11" type="wot_frame_time" baseLine="19.11">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_asia_korea_low_1280" benchmark="WIN32" name="WOT decoupled"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_asia_korea_low_1280" benchmark="WIN32" name="Chunk"></div>
		</div>
		
		<div class="chart" title="Frame Time Ms 1920 Res-Max baseLine=35.16" type="wot_frame_time" baseLine="35.16">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_asia_korea_max_1920" benchmark="WIN32" name="WOT decoupled"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_asia_korea_max_1920" benchmark="WIN32" name="Chunk"></div>
		</div>
		
		<div class="chart" title="Frame Time Ms 1920 Res-Med baseLine=29.08" type="wot_frame_time" baseLine="29.08">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_asia_korea_med_1920" benchmark="WIN32" name="WOT decoupled"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_asia_korea_med_1920" benchmark="WIN32" name="Chunk"></div>
		</div>
		
		<div class="chart" title="Frame Time Ms 1920 Res-low baseLine=19.59" type="wot_frame_time" baseLine="19.59">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_asia_korea_low_1920" benchmark="WIN32" name="WOT decoupled"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_asia_korea_low_1920" benchmark="WIN32" name="Chunk"></div>
		</div>
	</div>	
	<div class="chart-category">
		<h1><a href="http://ci.bigworldtech.com/job/test_wot_93_decoupled_performance_test/">Replay name: asia_korea - Loadtime Testing06-i7 AMD</a></h1>
		<h2><a href="http://ci.bigworldtech.com/job/test_wot_93_decoupled_performance_chunk_testing05/">Chunk job</a></h2>
		<div class="chart" title="Loadtime Sec 1280 Res-Max baseLine=8.64" type="wot_loadtime" baseLine="8.64">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_asia_korea_max_1280" benchmark="WIN32" name="WOT decoupled" ></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_asia_korea_max_1280" benchmark="WIN32" name="Chunk" ></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1280 Res-Med baseLine=8.18" type="wot_loadtime" baseLine="8.18">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_asia_korea_med_1280" benchmark="WIN32" name="WOT decoupled"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_asia_korea_med_1280" benchmark="WIN32" name="Chunk"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1280 Res-Low baseLine=8.01" type="wot_loadtime" baseLine="8.01">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_asia_korea_low_1280" benchmark="WIN32" name="WOT decoupled"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_asia_korea_low_1280" benchmark="WIN32" name="Chunk"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1920 Res-Max baseLine=8.68" type="wot_loadtime" baseLine="8.68">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_asia_korea_max_1920" benchmark="WIN32" name="WOT decoupled"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_asia_korea_max_1920" benchmark="WIN32" name="Chunk"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1920 Res-Med baseLine=8.14" type="wot_loadtime" baseLine="8.14">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_asia_korea_med_1920" benchmark="WIN32" name="WOT decoupled"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_asia_korea_med_1920" benchmark="WIN32" name="Chunk"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1920 Res-Low baseLine=8.17" type="wot_loadtime" baseLine="8.17">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_asia_korea_low_1920" benchmark="WIN32" name="WOT decoupled"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_asia_korea_low_1920" benchmark="WIN32" name="Chunk"></div>
		</div>
	
	</div>	

	<div class="chart-category">
		<h1><a href="http://ci.bigworldtech.com/job/test_wot_93_decoupled_performance_testing07/">Replay name: asia_korea - Frame Time Testing07-Core 2 Duo</a></h1>
		<h2><a href="http://ci.bigworldtech.com/job/test_wot_93_decoupled_performance_chunk_testing07/">Chunk job</a></h2>
		<div class="chart" title="Frame Time Ms 1280 Res-Max baseLine=120.05" type="wot_frame_time" baseLine="120.05">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_asia_korea_max_1280" benchmark="WIN32" name="WOT decoupled" ></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_asia_korea_max_1280" benchmark="WIN32" name="Chunk" ></div>
		</div>
		
		<div class="chart" title="Frame Time Ms 1280 Res-Med baseLine=73.93" type="wot_frame_time" baseLine="73.93">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_asia_korea_med_1280" benchmark="WIN32" name="WOT decoupled"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_asia_korea_med_1280" benchmark="WIN32" name="Chunk"></div>
		</div>
		
		<div class="chart" title="Frame Time Ms 1280 Res-Low baseLine=40.01" type="wot_frame_time" baseLine="40.01">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_asia_korea_low_1280" benchmark="WIN32" name="WOT decoupled"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_asia_korea_low_1280" benchmark="WIN32" name="Chunk"></div>
		</div>
		
		<div class="chart" title="Frame Time Ms 1920 Res-Max baseLine=87.75" type="wot_frame_time" baseLine="87.75">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_asia_korea_max_1920" benchmark="WIN32" name="WOT decoupled"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_asia_korea_max_1920" benchmark="WIN32" name="Chunk"></div>
		</div>
		
		<div class="chart" title="Frame Time Ms 1920 Res-Med baseLine=65.09" type="wot_frame_time" baseLine="65.09">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_asia_korea_med_1920" benchmark="WIN32" name="WOT decoupled"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_asia_korea_med_1920" benchmark="WIN32" name="Chunk"></div>
		</div>
		
		<div class="chart" title="Frame Time Ms 1920 Res-low baseLine=42.07" type="wot_frame_time" baseLine="42.07">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_asia_korea_low_1920" benchmark="WIN32" name="WOT decoupled"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_asia_korea_low_1920" benchmark="WIN32" name="Chunk"></div>
		</div>
	</div>	
	<div class="chart-category">
		<h1><a href="http://ci.bigworldtech.com/job/test_wot_93_decoupled_performance_test/">Replay name: asia_korea - Loadtime Testing07-Core 2 Duo</a></h1>
		<h2><a href="http://ci.bigworldtech.com/job/test_wot_93_decoupled_performance_chunk_testing05/">Chunk job</a></h2>
		<div class="chart" title="Loadtime Sec 1280 Res-Max baseLine=41.16" type="wot_loadtime" baseLine="41.16">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_asia_korea_max_1280" benchmark="WIN32" name="WOT decoupled" ></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_asia_korea_max_1280" benchmark="WIN32" name="Chunk" ></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1280 Res-Med baseLine=16.89" type="wot_loadtime" baseLine="16.89">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_asia_korea_med_1280" benchmark="WIN32" name="WOT decoupled"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_asia_korea_med_1280" benchmark="WIN32" name="Chunk"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1280 Res-Low baseLine=15.74" type="wot_loadtime" baseLine="15.74">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_asia_korea_low_1280" benchmark="WIN32" name="WOT decoupled"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_asia_korea_low_1280" benchmark="WIN32" name="Chunk"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1920 Res-Max baseLine=37.34" type="wot_loadtime" baseLine="37.34">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_asia_korea_max_1920" benchmark="WIN32" name="WOT decoupled"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_asia_korea_max_1920" benchmark="WIN32" name="Chunk"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1920 Res-Med baseLine=16.84" type="wot_loadtime" baseLine="16.84">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_asia_korea_med_1920" benchmark="WIN32" name="WOT decoupled"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_asia_korea_med_1920" benchmark="WIN32" name="Chunk"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1920 Res-Low baseLine=15.8" type="wot_loadtime" baseLine="15.8">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_asia_korea_low_1920" benchmark="WIN32" name="WOT decoupled"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_asia_korea_low_1920" benchmark="WIN32" name="Chunk"></div>
		</div>
	
	</div>	
	
	
	<div class="chart-category">
		<h1><a href="http://ci.bigworldtech.com/job/test_wot_93_decoupled_performance_testing08/">Replay name: asia_korea - Frame Time Testing08-i5</a></h1>
		<h2><a href="http://ci.bigworldtech.com/job/test_wot_93_decoupled_performance_chunk_testing08/">Chunk job</a></h2>
		<div class="chart" title="Frame Time Ms 1280 Res-Max baseLine=32.88" type="wot_frame_time" baseLine="32.88">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_asia_korea_max_1280" benchmark="WIN32" name="WOT decoupled" ></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_asia_korea_max_1280" benchmark="WIN32" name="Chunk" ></div>
		</div>
		
		<div class="chart" title="Frame Time Ms 1280 Res-Med baseLine=27.87" type="wot_frame_time" baseLine="27.87">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_asia_korea_med_1280" benchmark="WIN32" name="WOT decoupled"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_asia_korea_med_1280" benchmark="WIN32" name="Chunk"></div>
		</div>
		
		<div class="chart" title="Frame Time Ms 1280 Res-Low baseLine=31.867" type="wot_frame_time" baseLine="31.867">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_asia_korea_low_1280" benchmark="WIN32" name="WOT decoupled"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_asia_korea_low_1280" benchmark="WIN32" name="Chunk"></div>
		</div>
		
		<div class="chart" title="Frame Time Ms 1920 Res-Max baseLine=32.2" type="wot_frame_time" baseLine="32.2">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_asia_korea_max_1920" benchmark="WIN32" name="WOT decoupled"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_asia_korea_max_1920" benchmark="WIN32" name="Chunk"></div>
		</div>
		
		<div class="chart" title="Frame Time Ms 1920 Res-Med baseLine=28.37" type="wot_frame_time" baseLine="28.37">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_asia_korea_med_1920" benchmark="WIN32" name="WOT decoupled"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_asia_korea_med_1920" benchmark="WIN32" name="Chunk"></div>
		</div>
		
		<div class="chart" title="Frame Time Ms 1920 Res-low baseLine=39.619" type="wot_frame_time" baseLine="39.619">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_asia_korea_low_1920" benchmark="WIN32" name="WOT decoupled"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_asia_korea_low_1920" benchmark="WIN32" name="Chunk"></div>
		</div>
	</div>	
	<div class="chart-category">
		<h1><a href="http://ci.bigworldtech.com/job/test_wot_93_decoupled_performance_test/">Replay name: asia_korea - Loadtime Testing08-i5</a></h1>
		<h2><a href="http://ci.bigworldtech.com/job/test_wot_93_decoupled_performance_chunk_testing05/">Chunk job</a></h2>
		<div class="chart" title="Loadtime Sec 1280 Res-Max baseLine=9.33" type="wot_loadtime" baseLine="9.33">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_asia_korea_max_1280" benchmark="WIN32" name="WOT decoupled" ></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_asia_korea_max_1280" benchmark="WIN32" name="Chunk" ></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1280 Res-Med baseLine=8.85" type="wot_loadtime" baseLine="8.85">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_asia_korea_med_1280" benchmark="WIN32" name="WOT decoupled"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_asia_korea_med_1280" benchmark="WIN32" name="Chunk"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1280 Res-Low baseLine=12.8596" type="wot_loadtime" baseLine="12.8596">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_asia_korea_low_1280" benchmark="WIN32" name="WOT decoupled"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_asia_korea_low_1280" benchmark="WIN32" name="Chunk"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1920 Res-Max baseLine=9.15" type="wot_loadtime" baseLine="9.15">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_asia_korea_max_1920" benchmark="WIN32" name="WOT decoupled"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_asia_korea_max_1920" benchmark="WIN32" name="Chunk"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1920 Res-Med baseLine=9.32" type="wot_loadtime" baseLine="9.32">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_asia_korea_med_1920" benchmark="WIN32" name="WOT decoupled"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_asia_korea_med_1920" benchmark="WIN32" name="Chunk"></div>
		</div>
		
		<div class="chart" title="Loadtime Sec 1920 Res-Low baseLine=8.75303" type="wot_loadtime" baseLine="8.75303">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_asia_korea_low_1920" benchmark="WIN32" name="WOT decoupled"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_asia_korea_low_1920" benchmark="WIN32" name="Chunk"></div>
		</div>
	
	</div>	


		
	<div class="chart-category">	
		<h1><a href="http://ci.bigworldtech.com/job/test_wot_93_decoupled_memory_test/">Replay name: 51_asia - Cat tests Testing05</a></h1>
		<h2><a href="http://ci.bigworldtech.com/job/test_wot_93_decoupled_performance_chunk_testing05/">Chunk job</a></h2>
		<div class="chart" title="Available Virtual Memory (KB) max settings" type="wot_total_mem" baseLine="0">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1920_51_asia" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1280_51_asia" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1920_51_asia" benchmark="WIN32" name="chunk_max_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1280_51_asia" benchmark="WIN32" name="chunk_max_1280"></div>
		</div>	
		
		<div class="chart" title="Frame time  max settings" type="wot_frame_time" baseLine="0">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1920_51_asia" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1280_51_asia" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1920_51_asia" benchmark="WIN32" name="chunk_max_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1280_51_asia" benchmark="WIN32" name="chunk_max_1280"></div>
		</div>
	</div>
	<div>
		<div class="chart" title="Available Virtual Memory (KB) med settings" type="wot_total_mem" baseLine="0">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1920_51_asia" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1280_51_asia" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1920_51_asia" benchmark="WIN32" name="chunk_med_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1280_51_asia" benchmark="WIN32" name="chunk_med_1280"></div>
		</div>	
		
		<div class="chart" title="Frame time med settings" type="wot_frame_time" baseLine="0">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1920_51_asia" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1280_51_asia" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1920_51_asia" benchmark="WIN32" name="chunk_med_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1280_51_asia" benchmark="WIN32" name="chunk_med_1280"></div>
		</div>	
	</div>
	<div>	
		<div class="chart" title="Available Virtual Memory (KB)  med settings" type="wot_total_mem" baseLine="0">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1920_51_asia" benchmark="WIN32" name="low_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1280_51_asia" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1920_51_asia" benchmark="WIN32" name="chunk_low_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1280_51_asia" benchmark="WIN32" name="chunk_low_1280"></div>
		</div>	
		
		<div class="chart" title="Frame time low settings" type="wot_frame_time" baseLine="0">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1920_51_asia" benchmark="WIN32" name="low_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1280_51_asia" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1920_51_asia" benchmark="WIN32" name="chunk_low_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1280_51_asia" benchmark="WIN32" name="chunk_low_1280"></div>
		</div>	
	</div>
	
	<div class="chart-category">	
		<h1><a href="http://ci.bigworldtech.com/job/test_wot_93_decoupled_memory_test/">Replay name: 17_munchen - Cat tests Testing05</a></h1>
		<h2><a href="http://ci.bigworldtech.com/job/test_wot_93_decoupled_performance_chunk_testing05/">Chunk job</a></h2>
		
		<div class="chart" title="Available Virtual Memory (KB) max settings" type="wot_total_mem" baseLine="0">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1920_17_munchen" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1280_17_munchen" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1920_17_munchen" benchmark="WIN32" name="chunk_max_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1280_17_munchen" benchmark="WIN32" name="chunk_max_1280"></div>
		</div>	
		
		<div class="chart" title="Frame time  max settings" type="wot_frame_time" baseLine="0">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1920_17_munchen" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1280_17_munchen" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1920_17_munchen" benchmark="WIN32" name="chunk_max_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1280_17_munchen" benchmark="WIN32" name="chunk_max_1280"></div>
		</div>
	</div>
	<div>
		<div class="chart" title="Available Virtual Memory (KB) med settings" type="wot_total_mem" baseLine="0">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1920_17_munchen" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1280_17_munchen" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1920_17_munchen" benchmark="WIN32" name="chunk_med_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1280_17_munchen" benchmark="WIN32" name="chunk_med_1280"></div>
		</div>	
		
		<div class="chart" title="Frame time med settings" type="wot_frame_time" baseLine="0">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1920_17_munchen" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1280_17_munchen" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1920_17_munchen" benchmark="WIN32" name="chunk_med_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1280_17_munchen" benchmark="WIN32" name="chunk_med_1280"></div>
		</div>	
	</div>
	<div>	
		<div class="chart" title="Available Virtual Memory (KB)  low settings" type="wot_total_mem" baseLine="0">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1920_17_munchen" benchmark="WIN32" name="low_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1280_17_munchen" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1920_17_munchen" benchmark="WIN32" name="chunk_low_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1280_17_munchen" benchmark="WIN32" name="chunk_low_1280"></div>
		</div>	
		
		<div class="chart" title="Frame time  low settings" type="wot_frame_time" baseLine="0">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1920_17_munchen" benchmark="WIN32" name="low_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1280_17_munchen" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1920_17_munchen" benchmark="WIN32" name="chunk_low_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1280_17_munchen" benchmark="WIN32" name="chunk_low_1280"></div>
		</div>	
	</div>
	
	<div class="chart-category">	
		<h1><a href="http://ci.bigworldtech.com/job/test_wot_93_decoupled_memory_test/">Replay name: 00_tank_tutorial - Cat tests Testing05</a></h1>
		<h2><a href="http://ci.bigworldtech.com/job/test_wot_93_decoupled_performance_chunk_testing05/">Chunk job</a></h2>
		<div class="chart" title="Available Virtual Memory (KB) max settings" type="wot_total_mem" baseLine="0">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1920_00_tank_tutorial" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1280_00_tank_tutorial" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1920_00_tank_tutorial" benchmark="WIN32" name="chunk_max_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1280_00_tank_tutorial" benchmark="WIN32" name="chunk_max_1280"></div>
		</div>	
		
		<div class="chart" title="Frame time  max settings" type="wot_frame_time" baseLine="0">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1920_00_tank_tutorial" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1280_00_tank_tutorial" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1920_00_tank_tutorial" benchmark="WIN32" name="chunk_max_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1280_00_tank_tutorial" benchmark="WIN32" name="chunk_max_1280"></div>
		</div>
	</div>
	<div>
		<div class="chart" title="Available Virtual Memory (KB) med settings" type="wot_total_mem" baseLine="0">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1920_00_tank_tutorial" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1280_00_tank_tutorial" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1920_00_tank_tutorial" benchmark="WIN32" name="chunk_med_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1280_00_tank_tutorial" benchmark="WIN32" name="chunk_med_1280"></div>
		</div>	
		
		<div class="chart" title="Frame time med settings" type="wot_frame_time" baseLine="0">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1920_00_tank_tutorial" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1280_00_tank_tutorial" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1920_00_tank_tutorial" benchmark="WIN32" name="chunk_med_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1280_00_tank_tutorial" benchmark="WIN32" name="chunk_med_1280"></div>
		</div>	
	</div>
	<div>	
		<div class="chart" title="Available Virtual Memory (KB)  low settings" type="wot_total_mem" baseLine="0">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1920_00_tank_tutorial" benchmark="WIN32" name="low_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1280_00_tank_tutorial" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1920_00_tank_tutorial" benchmark="WIN32" name="chunk_low_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1280_00_tank_tutorial" benchmark="WIN32" name="chunk_low_1280"></div>
		</div>	
		
		<div class="chart" title="Frame time  low settings" type="wot_frame_time" baseLine="0">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1920_00_tank_tutorial" benchmark="WIN32" name="low_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1280_00_tank_tutorial" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1920_00_tank_tutorial" benchmark="WIN32" name="chunk_low_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1280_00_tank_tutorial" benchmark="WIN32" name="chunk_low_1280"></div>
		</div>	
	</div>
	
	<div class="chart-category">	
		<h1><a href="http://ci.bigworldtech.com/job/test_wot_93_decoupled_memory_test/">Replay name: 87_ruinberg_on_fire - Cat tests Testing05</a></h1>
		<h2><a href="http://ci.bigworldtech.com/job/test_wot_93_decoupled_performance_chunk_testing05/">Chunk job</a></h2>
		<div class="chart" title="Available Virtual Memory (KB) max settings" type="wot_total_mem" baseLine="0">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1920_87_ruinberg_on_fire" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1280_87_ruinberg_on_fire" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1920_87_ruinberg_on_fire" benchmark="WIN32" name="chunk_max_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1280_87_ruinberg_on_fire" benchmark="WIN32" name="chunk_max_1280"></div>
		</div>	
		
		<div class="chart" title="Frame time  max settings" type="wot_frame_time" baseLine="0">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1920_87_ruinberg_on_fire" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1280_87_ruinberg_on_fire" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1920_87_ruinberg_on_fire" benchmark="WIN32" name="chunk_max_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1280_87_ruinberg_on_fire" benchmark="WIN32" name="chunk_max_1280"></div>
		</div>
	</div>
	<div>
		<div class="chart" title="Available Virtual Memory (KB) med settings" type="wot_total_mem" baseLine="0">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1920_87_ruinberg_on_fire" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1280_87_ruinberg_on_fire" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1920_87_ruinberg_on_fire" benchmark="WIN32" name="chunk_med_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1280_87_ruinberg_on_fire" benchmark="WIN32" name="chunk_med_1280"></div>
		</div>	
		
		<div class="chart" title="Frame time med settings" type="wot_frame_time" baseLine="0">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1920_87_ruinberg_on_fire" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1280_87_ruinberg_on_fire" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1920_87_ruinberg_on_fire" benchmark="WIN32" name="chunk_med_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1280_87_ruinberg_on_fire" benchmark="WIN32" name="chunk_med_1280"></div>
		</div>	
	</div>
	<div>	
		<div class="chart" title="Available Virtual Memory (KB)  low settings" type="wot_total_mem" baseLine="0">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1920_87_ruinberg_on_fire" benchmark="WIN32" name="low_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1280_87_ruinberg_on_fire" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1920_87_ruinberg_on_fire" benchmark="WIN32" name="chunk_low_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1280_87_ruinberg_on_fire" benchmark="WIN32" name="chunk_low_1280"></div>
		</div>	
		
		<div class="chart" title="Frame time  low settings" type="wot_frame_time" baseLine="0">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1920_87_ruinberg_on_fire" benchmark="WIN32" name="low_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1280_87_ruinberg_on_fire" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1920_87_ruinberg_on_fire" benchmark="WIN32" name="chunk_low_1920"></div>
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1280_87_ruinberg_on_fire" benchmark="WIN32" name="chunk_low_1280"></div>
		</div>	
	</div>
			
	<div class="chart-category">	
		<h1><a href="http://ci.bigworldtech.com/job/test_wot_93_decoupled_memory_testing06/">Replay name: 51_asia - Cat tests Testing06-i7 AMD</a></h1>
		<h2><a href="http://ci.bigworldtech.com/job/test_wot_93_decoupled_performance_chunk_testing06/">Chunk job</a></h2>
		<div class="chart" title="Available Virtual Memory (KB) max settings" type="wot_total_mem" baseLine="0">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1920_51_asia" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1280_51_asia" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1920_51_asia" benchmark="WIN32" name="chunk_max_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1280_51_asia" benchmark="WIN32" name="chunk_max_1280"></div>
		</div>	
		
		<div class="chart" title="Frame time  max settings" type="wot_frame_time" baseLine="0">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1920_51_asia" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1280_51_asia" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1920_51_asia" benchmark="WIN32" name="chunk_max_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1280_51_asia" benchmark="WIN32" name="chunk_max_1280"></div>
		</div>
	</div>
	<div>
		<div class="chart" title="Available Virtual Memory (KB) med settings" type="wot_total_mem" baseLine="0">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1920_51_asia" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1280_51_asia" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1920_51_asia" benchmark="WIN32" name="chunk_med_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1280_51_asia" benchmark="WIN32" name="chunk_med_1280"></div>
		</div>	
		
		<div class="chart" title="Frame time med settings" type="wot_frame_time" baseLine="0">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1920_51_asia" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1280_51_asia" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1920_51_asia" benchmark="WIN32" name="chunk_med_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1280_51_asia" benchmark="WIN32" name="chunk_med_1280"></div>
		</div>	
	</div>
	<div>	
		<div class="chart" title="Available Virtual Memory (KB)  low settings" type="wot_total_mem" baseLine="0">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1920_51_asia" benchmark="WIN32" name="low_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1280_51_asia" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1920_51_asia" benchmark="WIN32" name="chunk_low_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1280_51_asia" benchmark="WIN32" name="chunk_low_1280"></div>
		</div>	
		
		<div class="chart" title="Frame time  low settings" type="wot_frame_time" baseLine="0">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1920_51_asia" benchmark="WIN32" name="low_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1280_51_asia" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1920_51_asia" benchmark="WIN32" name="chunk_low_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1280_51_asia" benchmark="WIN32" name="chunk_low_1280"></div>
		</div>	
	</div>
	
	<div class="chart-category">	
		<h1><a href="http://ci.bigworldtech.com/job/test_wot_93_decoupled_memory_testing06/">Replay name: 17_munchen - Cat tests Testing06-i7 AMD</a></h1>
		<h2><a href="http://ci.bigworldtech.com/job/test_wot_93_decoupled_performance_chunk_testing06/">Chunk job</a></h2>
		
		<div class="chart" title="Available Virtual Memory (KB) max settings" type="wot_total_mem" baseLine="0">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1920_17_munchen" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1280_17_munchen" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1920_17_munchen" benchmark="WIN32" name="chunk_max_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1280_17_munchen" benchmark="WIN32" name="chunk_max_1280"></div>
		</div>	
		
		<div class="chart" title="Frame time  max settings" type="wot_frame_time" baseLine="0">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1920_17_munchen" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1280_17_munchen" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1920_17_munchen" benchmark="WIN32" name="chunk_max_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1280_17_munchen" benchmark="WIN32" name="chunk_max_1280"></div>
		</div>
	</div>
	<div>
		<div class="chart" title="Available Virtual Memory (KB) med settings" type="wot_total_mem" baseLine="0">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1920_17_munchen" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1280_17_munchen" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1920_17_munchen" benchmark="WIN32" name="chunk_med_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1280_17_munchen" benchmark="WIN32" name="chunk_med_1280"></div>
		</div>	
		
		<div class="chart" title="Frame time med settings" type="wot_frame_time" baseLine="0">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1920_17_munchen" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1280_17_munchen" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1920_17_munchen" benchmark="WIN32" name="chunk_med_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1280_17_munchen" benchmark="WIN32" name="chunk_med_1280"></div>
		</div>	
	</div>
	<div>	
		<div class="chart" title="Available Virtual Memory (KB)  low settings" type="wot_total_mem" baseLine="0">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1920_17_munchen" benchmark="WIN32" name="low_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1280_17_munchen" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1920_17_munchen" benchmark="WIN32" name="chunk_low_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1280_17_munchen" benchmark="WIN32" name="chunk_low_1280"></div>
		</div>	
		
		<div class="chart" title="Frame time  low settings" type="wot_frame_time" baseLine="0">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1920_17_munchen" benchmark="WIN32" name="low_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1280_17_munchen" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1920_17_munchen" benchmark="WIN32" name="chunk_low_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1280_17_munchen" benchmark="WIN32" name="chunk_low_1280"></div>
		</div>	
	</div>
	
	<div class="chart-category">	
		<h1><a href="http://ci.bigworldtech.com/job/test_wot_93_decoupled_memory_testing06/">Replay name: 00_tank_tutorial - Cat tests Testing06-i7 AMD</a></h1>
		<h2><a href="http://ci.bigworldtech.com/job/test_wot_93_decoupled_performance_chunk_testing06/">Chunk job</a></h2>
		<div class="chart" title="Available Virtual Memory (KB) max settings" type="wot_total_mem" baseLine="0">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1920_00_tank_tutorial" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1280_00_tank_tutorial" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1920_00_tank_tutorial" benchmark="WIN32" name="chunk_max_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1280_00_tank_tutorial" benchmark="WIN32" name="chunk_max_1280"></div>
		</div>	
		
		<div class="chart" title="Frame time  max settings" type="wot_frame_time" baseLine="0">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1920_00_tank_tutorial" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1280_00_tank_tutorial" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1920_00_tank_tutorial" benchmark="WIN32" name="chunk_max_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1280_00_tank_tutorial" benchmark="WIN32" name="chunk_max_1280"></div>
		</div>
	</div>
	<div>
		<div class="chart" title="Available Virtual Memory (KB) med settings" type="wot_total_mem" baseLine="0">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1920_00_tank_tutorial" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1280_00_tank_tutorial" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1920_00_tank_tutorial" benchmark="WIN32" name="chunk_med_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1280_00_tank_tutorial" benchmark="WIN32" name="chunk_med_1280"></div>
		</div>	
		
		<div class="chart" title="Frame time med settings" type="wot_frame_time" baseLine="0">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1920_00_tank_tutorial" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1280_00_tank_tutorial" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1920_00_tank_tutorial" benchmark="WIN32" name="chunk_med_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1280_00_tank_tutorial" benchmark="WIN32" name="chunk_med_1280"></div>
		</div>	
	</div>
	<div>	
		<div class="chart" title="Available Virtual Memory (KB)  low settings" type="wot_total_mem" baseLine="0">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1920_00_tank_tutorial" benchmark="WIN32" name="low_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1280_00_tank_tutorial" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1920_00_tank_tutorial" benchmark="WIN32" name="chunk_low_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1280_00_tank_tutorial" benchmark="WIN32" name="chunk_low_1280"></div>
		</div>	
		
		<div class="chart" title="Frame time  low settings" type="wot_frame_time" baseLine="0">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1920_00_tank_tutorial" benchmark="WIN32" name="low_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1280_00_tank_tutorial" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1920_00_tank_tutorial" benchmark="WIN32" name="chunk_low_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1280_00_tank_tutorial" benchmark="WIN32" name="chunk_low_1280"></div>
		</div>	
	</div>
	
	<div class="chart-category">	
		<h1><a href="http://ci.bigworldtech.com/job/test_wot_93_decoupled_memory_testing06/">Replay name: 87_ruinberg_on_fire - Cat tests Testing06-i7 AMD</a></h1>
		<h2><a href="http://ci.bigworldtech.com/job/test_wot_93_decoupled_performance_chunk_testing06/">Chunk job</a></h2>
		<div class="chart" title="Available Virtual Memory (KB) max settings" type="wot_total_mem" baseLine="0">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1920_87_ruinberg_on_fire" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1280_87_ruinberg_on_fire" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1920_87_ruinberg_on_fire" benchmark="WIN32" name="chunk_max_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1280_87_ruinberg_on_fire" benchmark="WIN32" name="chunk_max_1280"></div>
		</div>	
		
		<div class="chart" title="Frame time  max settings" type="wot_frame_time" baseLine="0">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1920_87_ruinberg_on_fire" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1280_87_ruinberg_on_fire" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1920_87_ruinberg_on_fire" benchmark="WIN32" name="chunk_max_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1280_87_ruinberg_on_fire" benchmark="WIN32" name="chunk_max_1280"></div>
		</div>
	</div>
	<div>
		<div class="chart" title="Available Virtual Memory (KB) med settings" type="wot_total_mem" baseLine="0">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1920_87_ruinberg_on_fire" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1280_87_ruinberg_on_fire" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1920_87_ruinberg_on_fire" benchmark="WIN32" name="chunk_med_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1280_87_ruinberg_on_fire" benchmark="WIN32" name="chunk_med_1280"></div>
		</div>	
		
		<div class="chart" title="Frame time med settings" type="wot_frame_time" baseLine="0">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1920_87_ruinberg_on_fire" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1280_87_ruinberg_on_fire" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1920_87_ruinberg_on_fire" benchmark="WIN32" name="chunk_med_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1280_87_ruinberg_on_fire" benchmark="WIN32" name="chunk_med_1280"></div>
		</div>	
	</div>
	<div>	
		<div class="chart" title="Available Virtual Memory (KB)  low settings" type="wot_total_mem" baseLine="0">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1920_87_ruinberg_on_fire" benchmark="WIN32" name="low_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1280_87_ruinberg_on_fire" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1920_87_ruinberg_on_fire" benchmark="WIN32" name="chunk_low_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1280_87_ruinberg_on_fire" benchmark="WIN32" name="chunk_low_1280"></div>
		</div>	
		
		<div class="chart" title="Frame time  low settings" type="wot_frame_time" baseLine="0">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1920_87_ruinberg_on_fire" benchmark="WIN32" name="low_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1280_87_ruinberg_on_fire" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1920_87_ruinberg_on_fire" benchmark="WIN32" name="chunk_low_1920"></div>
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1280_87_ruinberg_on_fire" benchmark="WIN32" name="chunk_low_1280"></div>
		</div>	
	</div>


	<div class="chart-category">	
		<h1><a href="http://ci.bigworldtech.com/job/test_wot_93_decoupled_memory_testing07/">Replay name: 51_asia - Cat tests Testing07-Core 2 Duo</a></h1>
		<h2><a href="http://ci.bigworldtech.com/job/test_wot_93_decoupled_performance_chunk_testing07/">Chunk job</a></h2>
		<div class="chart" title="Available Virtual Memory (KB) max settings" type="wot_total_mem" baseLine="0">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1920_51_asia" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1280_51_asia" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1920_51_asia" benchmark="WIN32" name="chunk_max_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1280_51_asia" benchmark="WIN32" name="chunk_max_1280"></div>
		</div>	
		
		<div class="chart" title="Frame time  max settings" type="wot_frame_time" baseLine="0">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1920_51_asia" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1280_51_asia" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1920_51_asia" benchmark="WIN32" name="chunk_max_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1280_51_asia" benchmark="WIN32" name="chunk_max_1280"></div>
		</div>
	</div>
	<div>
		<div class="chart" title="Available Virtual Memory (KB) med settings" type="wot_total_mem" baseLine="0">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1920_51_asia" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1280_51_asia" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1920_51_asia" benchmark="WIN32" name="chunk_med_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1280_51_asia" benchmark="WIN32" name="chunk_med_1280"></div>
		</div>	
		
		<div class="chart" title="Frame time med settings" type="wot_frame_time" baseLine="0">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1920_51_asia" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1280_51_asia" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1920_51_asia" benchmark="WIN32" name="chunk_med_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1280_51_asia" benchmark="WIN32" name="chunk_med_1280"></div>
		</div>	
	</div>
	<div>	
		<div class="chart" title="Available Virtual Memory (KB)  low settings" type="wot_total_mem" baseLine="0">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1920_51_asia" benchmark="WIN32" name="low_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1280_51_asia" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1920_51_asia" benchmark="WIN32" name="chunk_low_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1280_51_asia" benchmark="WIN32" name="chunk_low_1280"></div>
		</div>	
		
		<div class="chart" title="Frame time  low settings" type="wot_frame_time" baseLine="0">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1920_51_asia" benchmark="WIN32" name="low_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1280_51_asia" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1920_51_asia" benchmark="WIN32" name="chunk_low_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1280_51_asia" benchmark="WIN32" name="chunk_low_1280"></div>
		</div>	
	</div>
	
	<div class="chart-category">	
		<h1><a href="http://ci.bigworldtech.com/job/test_wot_93_decoupled_memory_testing07/">Replay name: 17_munchen - Cat tests Testing07-Core 2 Duo</a></h1>
		<h2><a href="http://ci.bigworldtech.com/job/test_wot_93_decoupled_performance_chunk_testing07/">Chunk job</a></h2>
		
		<div class="chart" title="Available Virtual Memory (KB) max settings" type="wot_total_mem" baseLine="0">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1920_17_munchen" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1280_17_munchen" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1920_17_munchen" benchmark="WIN32" name="chunk_max_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1280_17_munchen" benchmark="WIN32" name="chunk_max_1280"></div>
		</div>	
		
		<div class="chart" title="Frame time  max settings" type="wot_frame_time" baseLine="0">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1920_17_munchen" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1280_17_munchen" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1920_17_munchen" benchmark="WIN32" name="chunk_max_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1280_17_munchen" benchmark="WIN32" name="chunk_max_1280"></div>
		</div>
	</div>
	<div>
		<div class="chart" title="Available Virtual Memory (KB) med settings" type="wot_total_mem" baseLine="0">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1920_17_munchen" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1280_17_munchen" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1920_17_munchen" benchmark="WIN32" name="chunk_med_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1280_17_munchen" benchmark="WIN32" name="chunk_med_1280"></div>
		</div>	
		
		<div class="chart" title="Frame time med settings" type="wot_frame_time" baseLine="0">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1920_17_munchen" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1280_17_munchen" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1920_17_munchen" benchmark="WIN32" name="chunk_med_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1280_17_munchen" benchmark="WIN32" name="chunk_med_1280"></div>
		</div>	
	</div>
	<div>	
		<div class="chart" title="Available Virtual Memory (KB)  low settings" type="wot_total_mem" baseLine="0">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1920_17_munchen" benchmark="WIN32" name="low_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1280_17_munchen" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1920_17_munchen" benchmark="WIN32" name="chunk_low_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1280_17_munchen" benchmark="WIN32" name="chunk_low_1280"></div>
		</div>	
		
		<div class="chart" title="Frame time  low settings" type="wot_frame_time" baseLine="0">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1920_17_munchen" benchmark="WIN32" name="low_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1280_17_munchen" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1920_17_munchen" benchmark="WIN32" name="chunk_low_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1280_17_munchen" benchmark="WIN32" name="chunk_low_1280"></div>
		</div>	
	</div>
	
	<div class="chart-category">	
		<h1><a href="http://ci.bigworldtech.com/job/test_wot_93_decoupled_memory_testing07/">Replay name: 00_tank_tutorial - Cat tests Testing07-Core 2 Duo</a></h1>
		<h2><a href="http://ci.bigworldtech.com/job/test_wot_93_decoupled_performance_chunk_testing07/">Chunk job</a></h2>
		<div class="chart" title="Available Virtual Memory (KB) max settings" type="wot_total_mem" baseLine="0">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1920_00_tank_tutorial" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1280_00_tank_tutorial" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1920_00_tank_tutorial" benchmark="WIN32" name="chunk_max_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1280_00_tank_tutorial" benchmark="WIN32" name="chunk_max_1280"></div>
		</div>	
		
		<div class="chart" title="Frame time  max settings" type="wot_frame_time" baseLine="0">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1920_00_tank_tutorial" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1280_00_tank_tutorial" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1920_00_tank_tutorial" benchmark="WIN32" name="chunk_max_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1280_00_tank_tutorial" benchmark="WIN32" name="chunk_max_1280"></div>
		</div>
	</div>
	<div>
		<div class="chart" title="Available Virtual Memory (KB) med settings" type="wot_total_mem" baseLine="0">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1920_00_tank_tutorial" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1280_00_tank_tutorial" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1920_00_tank_tutorial" benchmark="WIN32" name="chunk_med_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1280_00_tank_tutorial" benchmark="WIN32" name="chunk_med_1280"></div>
		</div>	
		
		<div class="chart" title="Frame time med settings" type="wot_frame_time" baseLine="0">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1920_00_tank_tutorial" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1280_00_tank_tutorial" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1920_00_tank_tutorial" benchmark="WIN32" name="chunk_med_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1280_00_tank_tutorial" benchmark="WIN32" name="chunk_med_1280"></div>
		</div>	
	</div>
	<div>	
		<div class="chart" title="Available Virtual Memory (KB)  low settings" type="wot_total_mem" baseLine="0">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1920_00_tank_tutorial" benchmark="WIN32" name="low_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1280_00_tank_tutorial" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1920_00_tank_tutorial" benchmark="WIN32" name="chunk_low_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1280_00_tank_tutorial" benchmark="WIN32" name="chunk_low_1280"></div>
		</div>	
		
		<div class="chart" title="Frame time  low settings" type="wot_frame_time" baseLine="0">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1920_00_tank_tutorial" benchmark="WIN32" name="low_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1280_00_tank_tutorial" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1920_00_tank_tutorial" benchmark="WIN32" name="chunk_low_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1280_00_tank_tutorial" benchmark="WIN32" name="chunk_low_1280"></div>
		</div>	
	</div>
	
	<div class="chart-category">	
		<h1><a href="http://ci.bigworldtech.com/job/test_wot_93_decoupled_memory_testing07/">Replay name: 87_ruinberg_on_fire - Cat tests Testing07-Core 2 Duo</a></h1>
		<h2><a href="http://ci.bigworldtech.com/job/test_wot_93_decoupled_performance_chunk_testing07/">Chunk job</a></h2>
		<div class="chart" title="Available Virtual Memory (KB) max settings" type="wot_total_mem" baseLine="0">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1920_87_ruinberg_on_fire" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1280_87_ruinberg_on_fire" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1920_87_ruinberg_on_fire" benchmark="WIN32" name="chunk_max_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1280_87_ruinberg_on_fire" benchmark="WIN32" name="chunk_max_1280"></div>
		</div>	
		
		<div class="chart" title="Frame time  max settings" type="wot_frame_time" baseLine="0">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1920_87_ruinberg_on_fire" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1280_87_ruinberg_on_fire" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1920_87_ruinberg_on_fire" benchmark="WIN32" name="chunk_max_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1280_87_ruinberg_on_fire" benchmark="WIN32" name="chunk_max_1280"></div>
		</div>
	</div>
	<div>
		<div class="chart" title="Available Virtual Memory (KB) med settings" type="wot_total_mem" baseLine="0">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1920_87_ruinberg_on_fire" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1280_87_ruinberg_on_fire" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1920_87_ruinberg_on_fire" benchmark="WIN32" name="chunk_med_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1280_87_ruinberg_on_fire" benchmark="WIN32" name="chunk_med_1280"></div>
		</div>	
		
		<div class="chart" title="Frame time med settings" type="wot_frame_time" baseLine="0">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1920_87_ruinberg_on_fire" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1280_87_ruinberg_on_fire" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1920_87_ruinberg_on_fire" benchmark="WIN32" name="chunk_med_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1280_87_ruinberg_on_fire" benchmark="WIN32" name="chunk_med_1280"></div>
		</div>	
	</div>
	<div>	
		<div class="chart" title="Available Virtual Memory (KB)  low settings" type="wot_total_mem" baseLine="0">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1920_87_ruinberg_on_fire" benchmark="WIN32" name="low_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1280_87_ruinberg_on_fire" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1920_87_ruinberg_on_fire" benchmark="WIN32" name="chunk_low_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1280_87_ruinberg_on_fire" benchmark="WIN32" name="chunk_low_1280"></div>
		</div>	
		
		<div class="chart" title="Frame time  low settings" type="wot_frame_time" baseLine="0">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1920_87_ruinberg_on_fire" benchmark="WIN32" name="low_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1280_87_ruinberg_on_fire" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1920_87_ruinberg_on_fire" benchmark="WIN32" name="chunk_low_1920"></div>
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1280_87_ruinberg_on_fire" benchmark="WIN32" name="chunk_low_1280"></div>
		</div>	
	</div>
	
	<div class="chart-category">	
		<h1><a href="http://ci.bigworldtech.com/job/test_wot_93_decoupled_memory_testing08/">Replay name: 51_asia - Cat tests Testing08-i5</a></h1>
		<h2><a href="http://ci.bigworldtech.com/job/test_wot_93_decoupled_performance_chunk_testing08/">Chunk job</a></h2>
		<div class="chart" title="Available Virtual Memory (KB) max settings" type="wot_total_mem" baseLine="0">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1920_51_asia" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1280_51_asia" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1920_51_asia" benchmark="WIN32" name="chunk_max_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1280_51_asia" benchmark="WIN32" name="chunk_max_1280"></div>
		</div>	
		
		<div class="chart" title="Frame time  max settings" type="wot_frame_time" baseLine="0">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1920_51_asia" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1280_51_asia" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1920_51_asia" benchmark="WIN32" name="chunk_max_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1280_51_asia" benchmark="WIN32" name="chunk_max_1280"></div>
		</div>
	</div>
	<div>
		<div class="chart" title="Available Virtual Memory (KB) med settings" type="wot_total_mem" baseLine="0">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1920_51_asia" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1280_51_asia" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1920_51_asia" benchmark="WIN32" name="chunk_med_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1280_51_asia" benchmark="WIN32" name="chunk_med_1280"></div>
		</div>	
		
		<div class="chart" title="Frame time med settings" type="wot_frame_time" baseLine="0">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1920_51_asia" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1280_51_asia" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1920_51_asia" benchmark="WIN32" name="chunk_med_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1280_51_asia" benchmark="WIN32" name="chunk_med_1280"></div>
		</div>	
	</div>
	<div>	
		<div class="chart" title="Available Virtual Memory (KB)  low settings" type="wot_total_mem" baseLine="0">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1920_51_asia" benchmark="WIN32" name="low_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1280_51_asia" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1920_51_asia" benchmark="WIN32" name="chunk_low_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1280_51_asia" benchmark="WIN32" name="chunk_low_1280"></div>
		</div>	
		
		<div class="chart" title="Frame time  low settings" type="wot_frame_time" baseLine="0">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1920_51_asia" benchmark="WIN32" name="low_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1280_51_asia" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1920_51_asia" benchmark="WIN32" name="chunk_low_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1280_51_asia" benchmark="WIN32" name="chunk_low_1280"></div>
		</div>	
	</div>
	
	<div class="chart-category">	
		<h1><a href="http://ci.bigworldtech.com/job/test_wot_93_decoupled_memory_testing08/">Replay name: 17_munchen - Cat tests Testing08-i5</a></h1>
		<h2><a href="http://ci.bigworldtech.com/job/test_wot_93_decoupled_performance_chunk_testing08/">Chunk job</a></h2>
		
		<div class="chart" title="Available Virtual Memory (KB) max settings" type="wot_total_mem" baseLine="0">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1920_17_munchen" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1280_17_munchen" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1920_17_munchen" benchmark="WIN32" name="chunk_max_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1280_17_munchen" benchmark="WIN32" name="chunk_max_1280"></div>
		</div>	
		
		<div class="chart" title="Frame time  max settings" type="wot_frame_time" baseLine="0">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1920_17_munchen" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1280_17_munchen" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1920_17_munchen" benchmark="WIN32" name="chunk_max_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1280_17_munchen" benchmark="WIN32" name="chunk_max_1280"></div>
		</div>
	</div>
	<div>
		<div class="chart" title="Available Virtual Memory (KB) med settings" type="wot_total_mem" baseLine="0">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1920_17_munchen" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1280_17_munchen" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1920_17_munchen" benchmark="WIN32" name="chunk_med_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1280_17_munchen" benchmark="WIN32" name="chunk_med_1280"></div>
		</div>	
		
		<div class="chart" title="Frame time med settings" type="wot_frame_time" baseLine="0">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1920_17_munchen" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1280_17_munchen" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1920_17_munchen" benchmark="WIN32" name="chunk_med_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1280_17_munchen" benchmark="WIN32" name="chunk_med_1280"></div>
		</div>	
	</div>
	<div>	
		<div class="chart" title="Available Virtual Memory (KB)  low settings" type="wot_total_mem" baseLine="0">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1920_17_munchen" benchmark="WIN32" name="low_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1280_17_munchen" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1920_17_munchen" benchmark="WIN32" name="chunk_low_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1280_17_munchen" benchmark="WIN32" name="chunk_low_1280"></div>
		</div>	
		
		<div class="chart" title="Frame time  low settings" type="wot_frame_time" baseLine="0">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1920_17_munchen" benchmark="WIN32" name="low_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1280_17_munchen" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1920_17_munchen" benchmark="WIN32" name="chunk_low_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1280_17_munchen" benchmark="WIN32" name="chunk_low_1280"></div>
		</div>	
	</div>
	
	<div class="chart-category">	
		<h1><a href="http://ci.bigworldtech.com/job/test_wot_93_decoupled_memory_testing08/">Replay name: 00_tank_tutorial - Cat tests Testing08-i5</a></h1>
		<h2><a href="http://ci.bigworldtech.com/job/test_wot_93_decoupled_performance_chunk_testing08/">Chunk job</a></h2>
		<div class="chart" title="Available Virtual Memory (KB) max settings" type="wot_total_mem" baseLine="0">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1920_00_tank_tutorial" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1280_00_tank_tutorial" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1920_00_tank_tutorial" benchmark="WIN32" name="chunk_max_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1280_00_tank_tutorial" benchmark="WIN32" name="chunk_max_1280"></div>
		</div>	
		
		<div class="chart" title="Frame time  max settings" type="wot_frame_time" baseLine="0">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1920_00_tank_tutorial" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1280_00_tank_tutorial" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1920_00_tank_tutorial" benchmark="WIN32" name="chunk_max_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1280_00_tank_tutorial" benchmark="WIN32" name="chunk_max_1280"></div>
		</div>
	</div>
	<div>
		<div class="chart" title="Available Virtual Memory (KB) med settings" type="wot_total_mem" baseLine="0">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1920_00_tank_tutorial" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1280_00_tank_tutorial" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1920_00_tank_tutorial" benchmark="WIN32" name="chunk_med_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1280_00_tank_tutorial" benchmark="WIN32" name="chunk_med_1280"></div>
		</div>	
		
		<div class="chart" title="Frame time med settings" type="wot_frame_time" baseLine="0">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1920_00_tank_tutorial" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1280_00_tank_tutorial" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1920_00_tank_tutorial" benchmark="WIN32" name="chunk_med_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1280_00_tank_tutorial" benchmark="WIN32" name="chunk_med_1280"></div>
		</div>	
	</div>
	<div>	
		<div class="chart" title="Available Virtual Memory (KB)  low settings" type="wot_total_mem" baseLine="0">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1920_00_tank_tutorial" benchmark="WIN32" name="low_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1280_00_tank_tutorial" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1920_00_tank_tutorial" benchmark="WIN32" name="chunk_low_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1280_00_tank_tutorial" benchmark="WIN32" name="chunk_low_1280"></div>
		</div>	
		
		<div class="chart" title="Frame time  low settings" type="wot_frame_time" baseLine="0">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1920_00_tank_tutorial" benchmark="WIN32" name="low_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1280_00_tank_tutorial" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1920_00_tank_tutorial" benchmark="WIN32" name="chunk_low_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1280_00_tank_tutorial" benchmark="WIN32" name="chunk_low_1280"></div>
		</div>	
	</div>
	
	<div class="chart-category">	
		<h1><a href="http://ci.bigworldtech.com/job/test_wot_93_decoupled_memory_testing08/">Replay name: 87_ruinberg_on_fire - Cat tests Testing08-i5</a></h1>
		<h2><a href="http://ci.bigworldtech.com/job/test_wot_93_decoupled_performance_chunk_testing08/">Chunk job</a></h2>
		<div class="chart" title="Available Virtual Memory (KB) max settings" type="wot_total_mem" baseLine="0">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1920_87_ruinberg_on_fire" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1280_87_ruinberg_on_fire" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1920_87_ruinberg_on_fire" benchmark="WIN32" name="chunk_max_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1280_87_ruinberg_on_fire" benchmark="WIN32" name="chunk_max_1280"></div>
		</div>	
		
		<div class="chart" title="Frame time  max settings" type="wot_frame_time" baseLine="0">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1920_87_ruinberg_on_fire" benchmark="WIN32" name="max_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_max_1280_87_ruinberg_on_fire" benchmark="WIN32" name="max_1280"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1920_87_ruinberg_on_fire" benchmark="WIN32" name="chunk_max_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_max_1280_87_ruinberg_on_fire" benchmark="WIN32" name="chunk_max_1280"></div>
		</div>
	</div>
	<div>
		<div class="chart" title="Available Virtual Memory (KB) med settings" type="wot_total_mem" baseLine="0">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1920_87_ruinberg_on_fire" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1280_87_ruinberg_on_fire" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1920_87_ruinberg_on_fire" benchmark="WIN32" name="chunk_med_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1280_87_ruinberg_on_fire" benchmark="WIN32" name="chunk_med_1280"></div>
		</div>	
		
		<div class="chart" title="Frame time med settings" type="wot_frame_time" baseLine="0">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1920_87_ruinberg_on_fire" benchmark="WIN32" name="med_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_med_1280_87_ruinberg_on_fire" benchmark="WIN32" name="med_1280"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1920_87_ruinberg_on_fire" benchmark="WIN32" name="chunk_med_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_med_1280_87_ruinberg_on_fire" benchmark="WIN32" name="chunk_med_1280"></div>
		</div>	
	</div>
	<div>	
		<div class="chart" title="Available Virtual Memory (KB)  low settings" type="wot_total_mem" baseLine="0">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1920_87_ruinberg_on_fire" benchmark="WIN32" name="low_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1280_87_ruinberg_on_fire" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1920_87_ruinberg_on_fire" benchmark="WIN32" name="chunk_low_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1280_87_ruinberg_on_fire" benchmark="WIN32" name="chunk_low_1280"></div>
		</div>	
		
		<div class="chart" title="Frame time  low settings" type="wot_frame_time" baseLine="0">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1920_87_ruinberg_on_fire" benchmark="WIN32" name="low_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_FREE_MEM_low_1280_87_ruinberg_on_fire" benchmark="WIN32" name="low_1280"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1920_87_ruinberg_on_fire" benchmark="WIN32" name="chunk_low_1920"></div>
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="decoupled_chunk_FREE_MEM_low_1280_87_ruinberg_on_fire" benchmark="WIN32" name="chunk_low_1280"></div>
		</div>	
	</div>
	
	
	<div style="clear:both"></div>
	<p>
		<small>[Page generated by <?php echo $hostname?>:<?php echo __FILE__;?>]</small>
	</p>
</body>
</html>
