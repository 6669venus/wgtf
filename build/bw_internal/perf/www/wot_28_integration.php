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
						console.log(value);
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
		
		width: 43%; 
		height: 300px;
		
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
	<div id="popup-chart" style="display:none;"></div>	
	
	<p>
	<img src="disclaimer.gif" height="60"  align="left" hspace="20"><ul>
		<li>This is our WOT trunk and BW 2.8 integration performance target</li>
		<li>32Bit Hybrid WOT client</li>
		<li>Zero in the graph are days on which the test failed to run.</li>
		<li>For a summary, please visit: <a href="wot_28_integration_summary.php">https://perf/wot_28_integration_summary.php</a> </li>
		<li><a href="https://confluence.wargaming.net/display/BWT/Performance+comparision+results">Information on testing methodology</a></li>
		<li>//wot/remote/branches/bigworldtech/wot_trunk_bwclient_2.8_bwserver_2.9_integration</li>
		<li>//wot/remote/trunk/ @188962 is used as a baseline (red line)</li>
	</ul>
	</p>
	
	
	<div class="chart-category">
		<h1><a href="http://ci/job/test_wot_28_integration_memory_testing05/">Cat tests Testing05</a></h1>
		<h2>Map: 23_westfeld</h2>
		<h2>Available Virtual Memory</h2>
		<div class="chart" title="Available Virtual Memory (KB) max settings 1920 Res baseLine=3235717" type="wot_total_mem" baseLine="3235717">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1920_23_westfeld" benchmark="WIN32" name="max_1920"></div>			
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) max settings Res 1280 baseLine=3215528" type="wot_total_mem" baseLine="3215528">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1280_23_westfeld" benchmark="WIN32" name="max_1280"></div>	
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) med settings 1920 Res baseLine=3334032" type="wot_total_mem" baseLine="3334032">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1920_23_westfeld" benchmark="WIN32" name="med_1920"></div>		
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) med settings Res 1280 baseLine=3330933" type="wot_total_mem" baseLine="3330933">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1280_23_westfeld" benchmark="WIN32" name="med_1280"></div>	
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) low settings 1920 Res baseLine=3354752" type="wot_total_mem" baseLine="3354752">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_23_westfeld" benchmark="WIN32" name="low_1920"></div>		
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) low settings Res 1280 baseLine=3358941" type="wot_total_mem" baseLine="3358941">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_23_westfeld" benchmark="WIN32" name="low_1280"></div>	
		</div>	
	</div>
	
	<div class="chart-category">
		<h1><a href="http://ci/job/test_wot_28_integration_memory_testing05/">Cat tests Testing05</a></h1>
		<h2>Map: 28_desert</h2>
		<h2>Available Virtual Memory</h2>
		<div class="chart" title="Available Virtual Memory (KB) max settings 1920 Res baseLine=3263031" type="wot_total_mem" baseLine="3263031">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1920_28_desert" benchmark="WIN32" name="max_1920"></div>			
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) max settings Res 1280 baseLine=3247558" type="wot_total_mem" baseLine="3247558">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1280_28_desert" benchmark="WIN32" name="max_1280"></div>	
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) med settings 1920 Res baseLine=3358826" type="wot_total_mem" baseLine="3358826">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1920_28_desert" benchmark="WIN32" name="med_1920"></div>		
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) med settings Res 1280 baseLine=3355563" type="wot_total_mem" baseLine="3355563">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1280_28_desert" benchmark="WIN32" name="med_1280"></div>	
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) low settings 1920 Res baseLine=3397337" type="wot_total_mem" baseLine="3397337">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_28_desert" benchmark="WIN32" name="low_1920"></div>		
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) low settings Res 1280 baseLine=3404523" type="wot_total_mem" baseLine="3404523">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_28_desert" benchmark="WIN32" name="low_1280"></div>	
		</div>	
	</div>
	
	<div class="chart-category">
		<h1><a href="http://ci/job/test_wot_28_integration_memory_testing05/">Cat tests Testing05</a></h1>
		<h2>Map: 17_munchen</h2>
		<h2>Available Virtual Memory</h2>
		<div class="chart" title="Available Virtual Memory (KB) max settings 1920 Res baseLine=3161991" type="wot_total_mem" baseLine="3161991">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1920_17_munchen" benchmark="WIN32" name="max_1920"></div>			
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) max settings Res 1280 baseLine=3141408" type="wot_total_mem" baseLine="3141408">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1280_17_munchen" benchmark="WIN32" name="max_1280"></div>	
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) med settings 1920 Res baseLine=3264999" type="wot_total_mem" baseLine="3264999">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1920_17_munchen" benchmark="WIN32" name="med_1920"></div>		
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) med settings Res 1280 baseLine=3260734" type="wot_total_mem" baseLine="3260734">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1280_17_munchen" benchmark="WIN32" name="med_1280"></div>	
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) low settings 1920 Res baseLine=3300957" type="wot_total_mem" baseLine="3300957">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_17_munchen" benchmark="WIN32" name="low_1920"></div>		
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) low settings Res 1280 baseLine=3305378" type="wot_total_mem" baseLine="3305378">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_17_munchen" benchmark="WIN32" name="low_1280"></div>	
		</div>	
	</div>
	
	<div class="chart-category">
		<h1><a href="http://ci/job/test_wot_28_integration_memory_testing05/">Cat tests Testing05</a></h1>
		<h2>Map: 87_ruinberg_on_fire</h2>
		<h2>Available Virtual Memory</h2>
		<div class="chart" title="Available Virtual Memory (KB) max settings 1920 Res baseLine=3146399" type="wot_total_mem" baseLine="3146399">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1920_87_ruinberg_on_fire" benchmark="WIN32" name="max_1920"></div>			
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) max settings Res 1280 baseLine=3112551" type="wot_total_mem" baseLine="3112551">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1280_87_ruinberg_on_fire" benchmark="WIN32" name="max_1280"></div>	
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) med settings 1920 Res baseLine=3245579" type="wot_total_mem" baseLine="3245579">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1920_87_ruinberg_on_fire" benchmark="WIN32" name="med_1920"></div>		
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) med settings Res 1280 baseLine=3242643" type="wot_total_mem" baseLine="3242643">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1280_87_ruinberg_on_fire" benchmark="WIN32" name="med_1280"></div>	
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) low settings 1920 Res baseLine=3310537" type="wot_total_mem" baseLine="3310537">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_87_ruinberg_on_fire" benchmark="WIN32" name="low_1920"></div>		
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) low settings Res 1280 baseLine=3301795" type="wot_total_mem" baseLine="3301795">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_87_ruinberg_on_fire" benchmark="WIN32" name="low_1280"></div>	
		</div>	
	</div>
	
	<div class="chart-category">
		<h1><a href="http://ci/job/test_wot_28_integration_memory_testing06/">Cat tests Testing06</a></h1>
		<h2>Map: 23_westfeld</h2>
		<h2>Available Virtual Memory</h2>
		<div class="chart" title="Available Virtual Memory (KB) max settings 1920 Res baseLine=3169309" type="wot_total_mem" baseLine="3169309">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1920_23_westfeld" benchmark="WIN32" name="max_1920"></div>			
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) max settings Res 1280 baseLine=3167545" type="wot_total_mem" baseLine="3167545">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1280_23_westfeld" benchmark="WIN32" name="max_1280"></div>	
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) med settings 1920 Res baseLine=3276221" type="wot_total_mem" baseLine="3276221">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1920_23_westfeld" benchmark="WIN32" name="med_1920"></div>		
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) med settings Res 1280 baseLine=3282180" type="wot_total_mem" baseLine="3282180">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1280_23_westfeld" benchmark="WIN32" name="med_1280"></div>	
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) low settings 1920 Res baseLine=3316201" type="wot_total_mem" baseLine="3316201">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_23_westfeld" benchmark="WIN32" name="low_1920"></div>		
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) low settings Res 1280 baseLine=3321078" type="wot_total_mem" baseLine="3321078">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_23_westfeld" benchmark="WIN32" name="low_1280"></div>	
		</div>	
	</div>
	
	<div class="chart-category">
		<h1><a href="http://ci/job/test_wot_28_integration_memory_testing06/">Cat tests Testing06</a></h1>
		<h2>Map: 28_desert</h2>
		<h2>Available Virtual Memory</h2>
		<div class="chart" title="Available Virtual Memory (KB) max settings 1920 Res baseLine=3191453" type="wot_total_mem" baseLine="3191453">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1920_28_desert" benchmark="WIN32" name="max_1920"></div>			
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) max settings Res 1280 baseLine=3200671" type="wot_total_mem" baseLine="3200671">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1280_28_desert" benchmark="WIN32" name="max_1280"></div>	
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) med settings 1920 Res baseLine=3305198" type="wot_total_mem" baseLine="3305198">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1920_28_desert" benchmark="WIN32" name="med_1920"></div>		
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) med settings Res 1280 baseLine=3315849" type="wot_total_mem" baseLine="3315849">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1280_28_desert" benchmark="WIN32" name="med_1280"></div>	
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) low settings 1920 Res baseLine=3352034" type="wot_total_mem" baseLine="3352034">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_28_desert" benchmark="WIN32" name="low_1920"></div>		
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) low settings Res 1280 baseLine=3356970" type="wot_total_mem" baseLine="3356970">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_28_desert" benchmark="WIN32" name="low_1280"></div>	
		</div>	
	</div>
	
	<div class="chart-category">
		<h1><a href="http://ci/job/test_wot_28_integration_memory_testing06/">Cat tests Testing06</a></h1>
		<h2>Map: 17_munchen</h2>
		<h2>Available Virtual Memory</h2>
		<div class="chart" title="Available Virtual Memory (KB) max settings 1920 Res baseLine=3090225" type="wot_total_mem" baseLine="3090225">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1920_17_munchen" benchmark="WIN32" name="max_1920"></div>			
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) max settings Res 1280 baseLine=3100748" type="wot_total_mem" baseLine="3100748">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1280_17_munchen" benchmark="WIN32" name="max_1280"></div>	
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) med settings 1920 Res baseLine=3209064" type="wot_total_mem" baseLine="3209064">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1920_17_munchen" benchmark="WIN32" name="med_1920"></div>		
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) med settings Res 1280 baseLine=3195864" type="wot_total_mem" baseLine="3195864">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1280_17_munchen" benchmark="WIN32" name="med_1280"></div>	
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) low settings 1920 Res baseLine=3224196" type="wot_total_mem" baseLine="3224196">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_17_munchen" benchmark="WIN32" name="low_1920"></div>		
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) low settings Res 1280 baseLine=3236054" type="wot_total_mem" baseLine="3236054">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_17_munchen" benchmark="WIN32" name="low_1280"></div>	
		</div>	
	</div>
	
	<div class="chart-category">
		<h1><a href="http://ci/job/test_wot_28_integration_memory_testing06/">Cat tests Testing06</a></h1>
		<h2>Map: 87_ruinberg_on_fire</h2>
		<h2>Available Virtual Memory</h2>
		<div class="chart" title="Available Virtual Memory (KB) max settings 1920 Res baseLine=3051597" type="wot_total_mem" baseLine="3051597">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1920_87_ruinberg_on_fire" benchmark="WIN32" name="max_1920"></div>			
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) max settings Res 1280 baseLine=3050459" type="wot_total_mem" baseLine="3050459">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1280_87_ruinberg_on_fire" benchmark="WIN32" name="max_1280"></div>	
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) med settings 1920 Res baseLine=3173668" type="wot_total_mem" baseLine="3173668">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1920_87_ruinberg_on_fire" benchmark="WIN32" name="med_1920"></div>		
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) med settings Res 1280 baseLine=3177428" type="wot_total_mem" baseLine="3177428">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1280_87_ruinberg_on_fire" benchmark="WIN32" name="med_1280"></div>	
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) low settings 1920 Res baseLine=3236078" type="wot_total_mem" baseLine="3236078">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_87_ruinberg_on_fire" benchmark="WIN32" name="low_1920"></div>		
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) low settings Res 1280 baseLine=3244615" type="wot_total_mem" baseLine="3244615">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_87_ruinberg_on_fire" benchmark="WIN32" name="low_1280"></div>	
		</div>	
	</div>
	
	<div class="chart-category">
		<h1><a href="http://ci/job/test_wot_28_integration_memory_testing07/">Cat tests Testing07</a></h1>
		<h2>Map: 23_westfeld</h2>
		<h2>Available Virtual Memory</h2>
		<div class="chart" title="Available Virtual Memory (KB) max settings 1920 Res baseLine=3255138" type="wot_total_mem" baseLine="3255138">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1920_23_westfeld" benchmark="WIN32" name="max_1920"></div>			
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) max settings Res 1280 baseLine=3253144" type="wot_total_mem" baseLine="3253144">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1280_23_westfeld" benchmark="WIN32" name="max_1280"></div>	
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) med settings 1920 Res baseLine=3375523" type="wot_total_mem" baseLine="3375523">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1920_23_westfeld" benchmark="WIN32" name="med_1920"></div>		
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) med settings Res 1280 baseLine=3345847" type="wot_total_mem" baseLine="3345847">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1280_23_westfeld" benchmark="WIN32" name="med_1280"></div>	
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) low settings 1920 Res baseLine=3353229" type="wot_total_mem" baseLine="3353229">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_23_westfeld" benchmark="WIN32" name="low_1920"></div>		
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) low settings Res 1280 baseLine=3380071" type="wot_total_mem" baseLine="3380071">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_23_westfeld" benchmark="WIN32" name="low_1280"></div>	
		</div>	
	</div>
	
	<div class="chart-category">
		<h1><a href="http://ci/job/test_wot_28_integration_memory_testing07/">Cat tests Testing07</a></h1>
		<h2>Map: 28_desert</h2>
		<h2>Available Virtual Memory</h2>
		<div class="chart" title="Available Virtual Memory (KB) max settings 1920 Res baseLine=3283862" type="wot_total_mem" baseLine="3283862">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1920_28_desert" benchmark="WIN32" name="max_1920"></div>			
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) max settings Res 1280 baseLine=3276929" type="wot_total_mem" baseLine="3276929">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1280_28_desert" benchmark="WIN32" name="max_1280"></div>	
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) med settings 1920 Res baseLine=3379314" type="wot_total_mem" baseLine="3379314">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1920_28_desert" benchmark="WIN32" name="med_1920"></div>		
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) med settings Res 1280 baseLine=3370927" type="wot_total_mem" baseLine="3370927">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1280_28_desert" benchmark="WIN32" name="med_1280"></div>	
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) low settings 1920 Res baseLine=3409218" type="wot_total_mem" baseLine="3409218">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_28_desert" benchmark="WIN32" name="low_1920"></div>		
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) low settings Res 1280 baseLine=3417244" type="wot_total_mem" baseLine="3417244">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_28_desert" benchmark="WIN32" name="low_1280"></div>	
		</div>	
	</div>
	
	<div class="chart-category">
		<h1><a href="http://ci/job/test_wot_28_integration_memory_testing07/">Cat tests Testing07</a></h1>
		<h2>Map: 17_munchen</h2>
		<h2>Available Virtual Memory</h2>
		<div class="chart" title="Available Virtual Memory (KB) max settings 1920 Res baseLine=3162767" type="wot_total_mem" baseLine="3162767">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1920_17_munchen" benchmark="WIN32" name="max_1920"></div>			
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) max settings Res 1280 baseLine=3165107" type="wot_total_mem" baseLine="3165107">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1280_17_munchen" benchmark="WIN32" name="max_1280"></div>	
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) med settings 1920 Res baseLine=3297728" type="wot_total_mem" baseLine="3297728">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1920_17_munchen" benchmark="WIN32" name="med_1920"></div>		
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) med settings Res 1280 baseLine=3296138" type="wot_total_mem" baseLine="3296138">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1280_17_munchen" benchmark="WIN32" name="med_1280"></div>	
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) low settings 1920 Res baseLine=3333073" type="wot_total_mem" baseLine="3333073">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_17_munchen" benchmark="WIN32" name="low_1920"></div>		
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) low settings Res 1280 baseLine=3334034" type="wot_total_mem" baseLine="3334034">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_17_munchen" benchmark="WIN32" name="low_1280"></div>	
		</div>	
	</div>
	
	<div class="chart-category">
		<h1><a href="http://ci/job/test_wot_28_integration_memory_testing07/">Cat tests Testing07</a></h1>
		<h2>Map: 87_ruinberg_on_fire</h2>
		<h2>Available Virtual Memory</h2>
		<div class="chart" title="Available Virtual Memory (KB) max settings 1920 Res baseLine=3137098" type="wot_total_mem" baseLine="3137098">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1920_87_ruinberg_on_fire" benchmark="WIN32" name="max_1920"></div>			
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) max settings Res 1280 baseLine=3139916" type="wot_total_mem" baseLine="3139916">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1280_87_ruinberg_on_fire" benchmark="WIN32" name="max_1280"></div>	
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) med settings 1920 Res baseLine=3281104" type="wot_total_mem" baseLine="3281104">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1920_87_ruinberg_on_fire" benchmark="WIN32" name="med_1920"></div>		
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) med settings Res 1280 baseLine=3266482" type="wot_total_mem" baseLine="3266482">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1280_87_ruinberg_on_fire" benchmark="WIN32" name="med_1280"></div>	
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) low settings 1920 Res baseLine=3312623" type="wot_total_mem" baseLine="3312623">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_87_ruinberg_on_fire" benchmark="WIN32" name="low_1920"></div>		
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) low settings Res 1280 baseLine=3306518" type="wot_total_mem" baseLine="3306518">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_87_ruinberg_on_fire" benchmark="WIN32" name="low_1280"></div>	
		</div>	
	</div>
	
	
	<div class="chart-category">
		<h1><a href="http://ci/job/test_wot_28_integration_memory_testing08/">Cat tests Testing08</a></h1>
		<h2>Map: 23_westfeld</h2>
		<h2>Available Virtual Memory</h2>
		<div class="chart" title="Available Virtual Memory (KB) max settings 1920 Res baseLine=3340857" type="wot_total_mem" baseLine="3340857">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_23_westfeld" benchmark="WIN32" name="low_1920"></div>		
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) low settings Res 1280 baseLine=3276508" type="wot_total_mem" baseLine="3276508">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_23_westfeld" benchmark="WIN32" name="low_1280"></div>	
		</div>	
	</div>
	
	<div class="chart-category">
		<h1><a href="http://ci/job/test_wot_28_integration_memory_testing08/">Cat tests Testing08</a></h1>
		<h2>Map: 28_desert</h2>
		<h2>Available Virtual Memory</h2>
		<div class="chart" title="Available Virtual Memory (KB) max settings 1920 Res baseLine=3362696" type="wot_total_mem" baseLine="3362696">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_28_desert" benchmark="WIN32" name="low_1920"></div>		
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) low settings Res 1280 baseLine=3363921" type="wot_total_mem" baseLine="3363921">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_28_desert" benchmark="WIN32" name="low_1280"></div>	
		</div>	
	</div>
	
	<div class="chart-category">
		<h1><a href="http://ci/job/test_wot_28_integration_memory_testing08/">Cat tests Testing08</a></h1>
		<h2>Map: 17_munchen</h2>
		<h2>Available Virtual Memory</h2>
		<div class="chart" title="Available Virtual Memory (KB) max settings 1920 Res baseLine=3264432" type="wot_total_mem" baseLine="3264432">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_17_munchen" benchmark="WIN32" name="low_1920"></div>		
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) low settings Res 1280 baseLine=3276508" type="wot_total_mem" baseLine="3276508">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_17_munchen" benchmark="WIN32" name="low_1280"></div>	
		</div>	
	</div>
	
	<div class="chart-category">
		<h1><a href="http://ci/job/test_wot_28_integration_memory_testing08/">Cat tests Testing08</a></h1>
		<h2>Map: 87_ruinberg_on_fire</h2>
		<h2>Available Virtual Memory</h2>
		<div class="chart" title="Available Virtual Memory (KB) max settings 1920 Res baseLine=3269230" type="wot_total_mem" baseLine="3269230">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_87_ruinberg_on_fire" benchmark="WIN32" name="low_1920"></div>		
		</div>	
		
		<div class="chart" title="Available Virtual Memory (KB) low settings Res 1280 baseLine=13250429" type="wot_total_mem" baseLine="3250429">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_87_ruinberg_on_fire" benchmark="WIN32" name="low_1280"></div>	
		</div>	
	</div>
	
		
	<div class="chart-category">
		<h1><a href="http://ci/job/test_wot_28_integration_memory_testing05/">Cat tests Testing05</a></h1>
		<h2>Map: 23_westfeld</h2>
		<h2>Frame time</h2>
		<div class="chart" title="Frame time (ms) max settings 1920 Res baseLine=7.91335" type="wot_frame_time" baseLine="7.91335">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1920_23_westfeld" benchmark="WIN32" name="max_1920"></div>			
		</div>	
		
		<div class="chart" title="Frame time (ms) max settings Res 1280 baseLine=8.71229" type="wot_frame_time" baseLine="8.71229">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1280_23_westfeld" benchmark="WIN32" name="max_1280"></div>	
		</div>	
		
		<div class="chart" title="Frame time (ms) med settings 1920 Res baseLine=7.84116" type="wot_frame_time" baseLine="7.84116">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1920_23_westfeld" benchmark="WIN32" name="med_1920"></div>		
		</div>	
		
		<div class="chart" title="Frame time (ms) med settings Res 1280 baseLine=7.85879" type="wot_frame_time" baseLine="7.85879">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1280_23_westfeld" benchmark="WIN32" name="med_1280"></div>	
		</div>	
		
		<div class="chart" title="Frame time (ms) max settings 1920 Res baseLine=7.84142" type="wot_frame_time" baseLine="7.84142">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_23_westfeld" benchmark="WIN32" name="low_1920"></div>		
		</div>	
		
		<div class="chart" title="Frame time (ms) low settings Res 1280 baseLine=7.84121" type="wot_frame_time" baseLine="7.84121">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_23_westfeld" benchmark="WIN32" name="low_1280"></div>	
		</div>	
	</div>
	
	<div class="chart-category">
		<h1><a href="http://ci/job/test_wot_28_integration_memory_testing05/">Cat tests Testing05</a></h1>
		<h2>Map: 28_desert</h2>
		<h2>Frame time</h2>
		<div class="chart" title="Frame time (ms) max settings 1920 Res baseLine=8.37679" type="wot_frame_time" baseLine="8.37679">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1920_28_desert" benchmark="WIN32" name="max_1920"></div>			
		</div>	
		
		<div class="chart" title="Frame time (ms) max settings Res 1280 baseLine=9.15269" type="wot_frame_time" baseLine="9.15269">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1280_28_desert" benchmark="WIN32" name="max_1280"></div>	
		</div>	
		
		<div class="chart" title="Frame time (ms) med settings 1920 Res baseLine=7.99742" type="wot_frame_time" baseLine="7.99742">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1920_28_desert" benchmark="WIN32" name="med_1920"></div>		
		</div>	
		
		<div class="chart" title="Frame time (ms) med settings Res 1280 baseLine=8.03198" type="wot_frame_time" baseLine="8.03198">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1280_28_desert" benchmark="WIN32" name="med_1280"></div>	
		</div>	
		
		<div class="chart" title="Frame time (ms) max settings 1920 Res baseLine=7.84092" type="wot_frame_time" baseLine="7.84092">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_28_desert" benchmark="WIN32" name="low_1920"></div>		
		</div>	
		
		<div class="chart" title="Frame time (ms) low settings Res 1280 baseLine=7.84126" type="wot_frame_time" baseLine="7.84126">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_28_desert" benchmark="WIN32" name="low_1280"></div>	
		</div>	
	</div>
	
	<div class="chart-category">
		<h1><a href="http://ci/job/test_wot_28_integration_memory_testing05/">Cat tests Testing05</a></h1>
		<h2>Map: 17_munchen</h2>
		<h2>Frame time</h2>
		<div class="chart" title="Frame time (ms) max settings 1920 Res baseLine=8.2638" type="wot_frame_time" baseLine="8.2638">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1920_17_munchen" benchmark="WIN32" name="max_1920"></div>			
		</div>	
		
		<div class="chart" title="Frame time (ms) max settings Res 1280 baseLine=8.94446" type="wot_frame_time" baseLine="8.94446">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1280_17_munchen" benchmark="WIN32" name="max_1280"></div>	
		</div>	
		
		<div class="chart" title="Frame time (ms) med settings 1920 Res baseLine=7.94986" type="wot_frame_time" baseLine="7.94986">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1920_17_munchen" benchmark="WIN32" name="med_1920"></div>		
		</div>	
		
		<div class="chart" title="Frame time (ms) med settings Res 1280 baseLine=7.96063" type="wot_frame_time" baseLine="7.96063">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1280_17_munchen" benchmark="WIN32" name="med_1280"></div>	
		</div>	
		
		<div class="chart" title="Frame time (ms) max settings 1920 Res baseLine=7.84114" type="wot_frame_time" baseLine="7.84114">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_17_munchen" benchmark="WIN32" name="low_1920"></div>		
		</div>	
		
		<div class="chart" title="Frame time (ms) low settings Res 1280 baseLine=7.84083" type="wot_frame_time" baseLine="7.84083">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_17_munchen" benchmark="WIN32" name="low_1280"></div>	
		</div>	
	</div>
	
	<div class="chart-category">
		<h1><a href="http://ci/job/test_wot_28_integration_memory_testing05/">Cat tests Testing05</a></h1>
		<h2>Map: 87_ruinberg_on_fire</h2>
		<h2>Frame time</h2>
		<div class="chart" title="Frame time (ms) max settings 1920 Res baseLine=8.08583" type="wot_frame_time" baseLine="8.08583">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1920_87_ruinberg_on_fire" benchmark="WIN32" name="max_1920"></div>			
		</div>	
		
		<div class="chart" title="Frame time (ms) max settings Res 1280 baseLine=9.52114" type="wot_frame_time" baseLine="9.52114">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1280_87_ruinberg_on_fire" benchmark="WIN32" name="max_1280"></div>	
		</div>	
		
		<div class="chart" title="Frame time (ms) med settings 1920 Res baseLine=7.84143" type="wot_frame_time" baseLine="7.84143">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1920_87_ruinberg_on_fire" benchmark="WIN32" name="med_1920"></div>		
		</div>	
		
		<div class="chart" title="Frame time (ms) med settings Res 1280 baseLine=7.96094" type="wot_frame_time" baseLine="7.96094">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1280_87_ruinberg_on_fire" benchmark="WIN32" name="med_1280"></div>	
		</div>	
		
		<div class="chart" title="Frame time (ms) max settings 1920 Res baseLine=7.84131" type="wot_frame_time" baseLine="7.84131">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_87_ruinberg_on_fire" benchmark="WIN32" name="low_1920"></div>		
		</div>	
		
		<div class="chart" title="Frame time (ms) low settings Res 1280 baseLine=7.84126" type="wot_frame_time" baseLine="7.84126">
			<div class="column" hostname="TESTING05" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_87_ruinberg_on_fire" benchmark="WIN32" name="low_1280"></div>	
		</div>	
	</div>
	
	<div class="chart-category">
		<h1><a href="http://ci/job/test_wot_28_integration_memory_testing06/">Cat tests Testing06</a></h1>
		<h2>Map: 23_westfeld</h2>
		<h2>Frame time</h2>
		<div class="chart" title="Frame time (ms) max settings 1920 Res baseLine=11.8066" type="wot_frame_time" baseLine="11.8066">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1920_23_westfeld" benchmark="WIN32" name="max_1920"></div>			
		</div>	
		
		<div class="chart" title="Frame time (ms) max settings Res 1280 baseLine=8.72515" type="wot_frame_time" baseLine="8.72515">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1280_23_westfeld" benchmark="WIN32" name="max_1280"></div>	
		</div>	
		
		<div class="chart" title="Frame time (ms) med settings 1920 Res baseLine=8.56894" type="wot_frame_time" baseLine="8.56894">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1920_23_westfeld" benchmark="WIN32" name="med_1920"></div>		
		</div>	
		
		<div class="chart" title="Frame time (ms) med settings Res 1280 baseLine=7.87244" type="wot_frame_time" baseLine="7.87244">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1280_23_westfeld" benchmark="WIN32" name="med_1280"></div>	
		</div>	
		
		<div class="chart" title="Frame time (ms) max settings 1920 Res baseLine=7.84139" type="wot_frame_time" baseLine="7.84139">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_23_westfeld" benchmark="WIN32" name="low_1920"></div>		
		</div>	
		
		<div class="chart" title="Frame time (ms) low settings Res 1280 baseLine=7.84153" type="wot_frame_time" baseLine="7.84153">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_23_westfeld" benchmark="WIN32" name="low_1280"></div>	
		</div>	
	</div>
	
	<div class="chart-category">
		<h1><a href="http://ci/job/test_wot_28_integration_memory_testing06/">Cat tests Testing06</a></h1>
		<h2>Map: 28_desert</h2>
		<h2>Frame time</h2>
		<div class="chart" title="Frame time (ms) max settings 1920 Res baseLine=11.893" type="wot_frame_time" baseLine="11.893">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1920_28_desert" benchmark="WIN32" name="max_1920"></div>			
		</div>	
		
		<div class="chart" title="Frame time (ms) max settings Res 1280 baseLine=9.01818" type="wot_frame_time" baseLine="9.01818">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1280_28_desert" benchmark="WIN32" name="max_1280"></div>	
		</div>	
		
		<div class="chart" title="Frame time (ms) med settings 1920 Res baseLine=8.79074" type="wot_frame_time" baseLine="8.79074">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1920_28_desert" benchmark="WIN32" name="med_1920"></div>		
		</div>	
		
		<div class="chart" title="Frame time (ms) med settings Res 1280 baseLine=8.1535" type="wot_frame_time" baseLine="8.1535">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1280_28_desert" benchmark="WIN32" name="med_1280"></div>	
		</div>	
		
		<div class="chart" title="Frame time (ms) max settings 1920 Res baseLine=7.84118" type="wot_frame_time" baseLine="7.84118">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_28_desert" benchmark="WIN32" name="low_1920"></div>		
		</div>	
		
		<div class="chart" title="Frame time (ms) low settings Res 1280 baseLine=7.84108" type="wot_frame_time" baseLine="7.84108">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_28_desert" benchmark="WIN32" name="low_1280"></div>	
		</div>	
	</div>
	
	<div class="chart-category">
		<h1><a href="http://ci/job/test_wot_28_integration_memory_testing06/">Cat tests Testing06</a></h1>
		<h2>Map: 17_munchen</h2>
		<h2>Frame time</h2>
		<div class="chart" title="Frame time (ms) max settings 1920 Res baseLine=11.6486" type="wot_frame_time" baseLine="11.6486">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1920_17_munchen" benchmark="WIN32" name="max_1920"></div>			
		</div>	
		
		<div class="chart" title="Frame time (ms) max settings Res 1280 baseLine=8.87849" type="wot_frame_time" baseLine="8.87849">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1280_17_munchen" benchmark="WIN32" name="max_1280"></div>	
		</div>	
		
		<div class="chart" title="Frame time (ms) med settings 1920 Res baseLine=8.87026" type="wot_frame_time" baseLine="8.87026">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1920_17_munchen" benchmark="WIN32" name="med_1920"></div>		
		</div>	
		
		<div class="chart" title="Frame time (ms) med settings Res 1280 baseLine=8.0662" type="wot_frame_time" baseLine="8.0662">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1280_17_munchen" benchmark="WIN32" name="med_1280"></div>	
		</div>	
		
		<div class="chart" title="Frame time (ms) max settings 1920 Res baseLine=7.8416" type="wot_frame_time" baseLine="7.8416">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_17_munchen" benchmark="WIN32" name="low_1920"></div>		
		</div>	
		
		<div class="chart" title="Frame time (ms) low settings Res 1280 baseLine=7.8412" type="wot_frame_time" baseLine="7.8412">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_17_munchen" benchmark="WIN32" name="low_1280"></div>	
		</div>	
	</div>
	
	<div class="chart-category">
		<h1><a href="http://ci/job/test_wot_28_integration_memory_testing06/">Cat tests Testing06</a></h1>
		<h2>Map: 87_ruinberg_on_fire</h2>
		<h2>Frame time</h2>
		<div class="chart" title="Frame time (ms) max settings 1920 Res baseLine=13.1136" type="wot_frame_time" baseLine="13.1136">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1920_87_ruinberg_on_fire" benchmark="WIN32" name="max_1920"></div>			
		</div>	
		
		<div class="chart" title="Frame time (ms) max settings Res 1280 baseLine=9.84157" type="wot_frame_time" baseLine="9.84157">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1280_87_ruinberg_on_fire" benchmark="WIN32" name="max_1280"></div>	
		</div>	
		
		<div class="chart" title="Frame time (ms) med settings 1920 Res baseLine=9.49446" type="wot_frame_time" baseLine="9.49446">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1920_87_ruinberg_on_fire" benchmark="WIN32" name="med_1920"></div>		
		</div>	
		
		<div class="chart" title="Frame time (ms) med settings Res 1280 baseLine=8.09046" type="wot_frame_time" baseLine="8.09046">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1280_87_ruinberg_on_fire" benchmark="WIN32" name="med_1280"></div>	
		</div>	
		
		<div class="chart" title="Frame time (ms) max settings 1920 Res baseLine=7.84126" type="wot_frame_time" baseLine="7.84126">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_87_ruinberg_on_fire" benchmark="WIN32" name="low_1920"></div>		
		</div>	
		
		<div class="chart" title="Frame time (ms) low settings Res 1280 baseLine=7.84094" type="wot_frame_time" baseLine="7.84094">
			<div class="column" hostname="TESTING06" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_87_ruinberg_on_fire" benchmark="WIN32" name="low_1280"></div>	
		</div>	
	</div>
	
	<div class="chart-category">
		<h1><a href="http://ci/job/test_wot_28_integration_memory_testing07/">Cat tests Testing07</a></h1>
		<h2>Map: 23_westfeld</h2>
		<h2>Frame time</h2>
		<div class="chart" title="Frame time (ms) max settings 1920 Res baseLine=30.1766" type="wot_frame_time" baseLine="30.1766">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1920_23_westfeld" benchmark="WIN32" name="max_1920"></div>			
		</div>	
		
		<div class="chart" title="Frame time (ms) max settings Res 1280 baseLine=46.3174" type="wot_frame_time" baseLine="46.3174">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1280_23_westfeld" benchmark="WIN32" name="max_1280"></div>	
		</div>	
		
		<div class="chart" title="Frame time (ms) med settings 1920 Res baseLine=18.1907" type="wot_frame_time" baseLine="18.1907">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1920_23_westfeld" benchmark="WIN32" name="med_1920"></div>		
		</div>	
		
		<div class="chart" title="Frame time (ms) med settings Res 1280 baseLine=26.97" type="wot_frame_time" baseLine="26.97">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1280_23_westfeld" benchmark="WIN32" name="med_1280"></div>	
		</div>	
		
		<div class="chart" title="Frame time (ms) max settings 1920 Res baseLine=7.89653" type="wot_frame_time" baseLine="7.89653">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_23_westfeld" benchmark="WIN32" name="low_1920"></div>		
		</div>	
		
		<div class="chart" title="Frame time (ms) low settings Res 1280 baseLine=7.90935" type="wot_frame_time" baseLine="7.90935">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_23_westfeld" benchmark="WIN32" name="low_1280"></div>	
		</div>	
	</div>
	
	<div class="chart-category">
		<h1><a href="http://ci/job/test_wot_28_integration_memory_testing07/">Cat tests Testing07</a></h1>
		<h2>Map: 28_desert</h2>
		<h2>Frame time</h2>
		<div class="chart" title="Frame time (ms) max settings 1920 Res baseLine=29.1282" type="wot_frame_time" baseLine="29.1282">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1920_28_desert" benchmark="WIN32" name="max_1920"></div>			
		</div>	
		
		<div class="chart" title="Frame time (ms) max settings Res 1280 baseLine=54.4798" type="wot_frame_time" baseLine="54.4798">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1280_28_desert" benchmark="WIN32" name="max_1280"></div>	
		</div>	
		
		<div class="chart" title="Frame time (ms) med settings 1920 Res baseLine=18.6815" type="wot_frame_time" baseLine="18.6815">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1920_28_desert" benchmark="WIN32" name="med_1920"></div>		
		</div>	
		
		<div class="chart" title="Frame time (ms) med settings Res 1280 baseLine=26.9059" type="wot_frame_time" baseLine="26.9059">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1280_28_desert" benchmark="WIN32" name="med_1280"></div>	
		</div>	
		
		<div class="chart" title="Frame time (ms) max settings 1920 Res baseLine=7.93171" type="wot_frame_time" baseLine="7.93171">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_28_desert" benchmark="WIN32" name="low_1920"></div>		
		</div>	
		
		<div class="chart" title="Frame time (ms) low settings Res 1280 baseLine=7.96535" type="wot_frame_time" baseLine="7.96535">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_28_desert" benchmark="WIN32" name="low_1280"></div>	
		</div>	
	</div>
	
	<div class="chart-category">
		<h1><a href="http://ci/job/test_wot_28_integration_memory_testing07/">Cat tests Testing07</a></h1>
		<h2>Map: 17_munchen</h2>
		<h2>Frame time</h2>
		<div class="chart" title="Frame time (ms) max settings 1920 Res baseLine=34.507" type="wot_frame_time" baseLine="34.507">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1920_17_munchen" benchmark="WIN32" name="max_1920"></div>			
		</div>	
		
		<div class="chart" title="Frame time (ms) max settings Res 1280 baseLine=57.94" type="wot_frame_time" baseLine="57.94">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1280_17_munchen" benchmark="WIN32" name="max_1280"></div>	
		</div>	
		
		<div class="chart" title="Frame time (ms) med settings 1920 Res baseLine=19.5158" type="wot_frame_time" baseLine="19.5158">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1920_17_munchen" benchmark="WIN32" name="med_1920"></div>		
		</div>	
		
		<div class="chart" title="Frame time (ms) med settings Res 1280 baseLine=27.8659" type="wot_frame_time" baseLine="27.8659">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1280_17_munchen" benchmark="WIN32" name="med_1280"></div>	
		</div>	
		
		<div class="chart" title="Frame time (ms) max settings 1920 Res baseLine=8.17283" type="wot_frame_time" baseLine="8.17283">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_17_munchen" benchmark="WIN32" name="low_1920"></div>		
		</div>	
		
		<div class="chart" title="Frame time (ms) low settings Res 1280 baseLine=8.37554" type="wot_frame_time" baseLine="8.37554">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_17_munchen" benchmark="WIN32" name="low_1280"></div>	
		</div>	
	</div>
	
	<div class="chart-category">
		<h1><a href="http://ci/job/test_wot_28_integration_memory_testing07/">Cat tests Testing07</a></h1>
		<h2>Map: 87_ruinberg_on_fire</h2>
		<h2>Frame time</h2>
		<div class="chart" title="Frame time (ms) max settings 1920 Res baseLine=35.7753" type="wot_frame_time" baseLine="35.7753">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1920_87_ruinberg_on_fire" benchmark="WIN32" name="max_1920"></div>			
		</div>	
		
		<div class="chart" title="Frame time (ms) max settings Res 1280 baseLine=55.0223" type="wot_frame_time" baseLine="55.0223">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_max_1280_87_ruinberg_on_fire" benchmark="WIN32" name="max_1280"></div>	
		</div>	
		
		<div class="chart" title="Frame time (ms) med settings 1920 Res baseLine=21.8993" type="wot_frame_time" baseLine="21.8993">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1920_87_ruinberg_on_fire" benchmark="WIN32" name="med_1920"></div>		
		</div>	
		
		<div class="chart" title="Frame time (ms) med settings Res 1280 baseLine=31.5842" type="wot_frame_time" baseLine="31.5842">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_med_1280_87_ruinberg_on_fire" benchmark="WIN32" name="med_1280"></div>	
		</div>	
		
		<div class="chart" title="Frame time (ms) max settings 1920 Res baseLine=8.01743" type="wot_frame_time" baseLine="8.01743">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_87_ruinberg_on_fire" benchmark="WIN32" name="low_1920"></div>		
		</div>	
		
		<div class="chart" title="Frame time (ms) low settings Res 1280 baseLine=8.23163" type="wot_frame_time" baseLine="8.23163">
			<div class="column" hostname="TESTING07" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_87_ruinberg_on_fire" benchmark="WIN32" name="low_1280"></div>	
		</div>	
	</div>
	
	<div class="chart-category">
		<h1><a href="http://ci/job/test_wot_28_integration_memory_testing08/">Cat tests Testing08</a></h1>
		<h2>Map: 23_westfeld</h2>
		<h2>Frame time</h2>
		<div class="chart" title="Frame time (ms) max settings 1920 Res baseLine=20.8378" type="wot_frame_time" baseLine="20.8378">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_23_westfeld" benchmark="WIN32" name="low_1920"></div>		
		</div>	
		
		<div class="chart" title="Frame time (ms) low settings Res 1280 baseLine=15.2703" type="wot_frame_time" baseLine="15.2703">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_23_westfeld" benchmark="WIN32" name="low_1280"></div>	
		</div>	
	</div>
	
	<div class="chart-category">
		<h1><a href="http://ci/job/test_wot_28_integration_memory_testing08/">Cat tests Testing08</a></h1>
		<h2>Map: 28_desert</h2>
		<h2>Frame time</h2>
		<div class="chart" title="Frame time (ms) max settings 1920 Res baseLine=18.5812" type="wot_frame_time" baseLine="18.5812">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_28_desert" benchmark="WIN32" name="low_1920"></div>		
		</div>	
		
		<div class="chart" title="Frame time (ms) low settings Res 1280 baseLine=13.3904" type="wot_frame_time" baseLine="13.3904">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_28_desert" benchmark="WIN32" name="low_1280"></div>	
		</div>	
	</div>
	
	<div class="chart-category">
		<h1><a href="http://ci/job/test_wot_28_integration_memory_testing08/">Cat tests Testing08</a></h1>
		<h2>Map: 17_munchen</h2>
		<h2>Frame time</h2>
		<div class="chart" title="Frame time (ms) max settings 1920 Res baseLine=19.112" type="wot_frame_time" baseLine="19.112">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_17_munchen" benchmark="WIN32" name="low_1920"></div>		
		</div>	
		
		<div class="chart" title="Frame time (ms) low settings Res 1280 baseLine=13.6567" type="wot_frame_time" baseLine="13.6567">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_17_munchen" benchmark="WIN32" name="low_1280"></div>	
		</div>	
	</div>
	
	<div class="chart-category">
		<h1><a href="http://ci/job/test_wot_28_integration_memory_testing08/">Cat tests Testing08</a></h1>
		<h2>Map: 87_ruinberg_on_fire</h2>
		<h2>Frame time</h2>
		<div class="chart" title="Frame time (ms) max settings 1920 Res baseLine=22.7128" type="wot_frame_time" baseLine="22.7128">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1920_87_ruinberg_on_fire" benchmark="WIN32" name="low_1920"></div>		
		</div>	
		
		<div class="chart" title="Frame time (ms) low settings Res 1280 baseLine=16.3741" type="wot_frame_time" baseLine="16.3741">
			<div class="column" hostname="TESTING08" configuration="WIN32" executable="Hybrid" branch="bwclient_2.8_bwserver_2.9_FREE_MEM_low_1280_87_ruinberg_on_fire" benchmark="WIN32" name="low_1280"></div>	
		</div>	
	</div>
	
	<div style="clear:both"></div>
	<p>
		<small>[Page generated by <?php echo $hostname?>:<?php echo __FILE__;?>]</small>
	</p>
</body>
</html>
