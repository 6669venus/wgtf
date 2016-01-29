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
					// title: "Test Changelist",
					textStyle: {
						fontSize: 8
					} 
				},
				chartArea: {
					width: "80%" 
					// height: "70%" 
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
		<li>This is our main ARI performance target</li>
		<li>Zero in the graph are days on which the test failed to run.</li>
		<li><a href="https://confluence.wargaming.net/display/BWT/Performance+comparision+results">Information on testing methodology</a></li>
		<li>//wot/remote/trunk/ #167856 is used as a baseline (red line)</li>
	</ul>
	</p>
	
	<div class="chart-category">
		<h1><a href="http://ci.bigworldtech.com/view/ARI/job/test_wot_ari_dev_performance_test/">//wot/branches/features/ARI/dev/game/ (Hybrid) - Max settings - Windows7</a></h1>
		<div class="chart" style="width:45%" title="Frame Time Ms baseLine=15.166 ms" type="wot_frame_time" baseLine="15.166" >
			<div class="column" hostname="TESTING03" configuration="WIN32" executable="Hybrid" branch="P3_DEV_max__himmelsdorf" benchmark="WIN32" name="himmelsdorf"></div>
			<div class="column" hostname="TESTING03" configuration="WIN32" executable="Hybrid" branch="P3_DEV_enablewgfx_max__himmelsdorf" benchmark="WIN32" name="himmelsdorf_enablewgfx"></div>
		</div>
		<div class="chart" style="width:45%" title="Frame Time Ms baseLine=18.924 ms" type="wot_frame_time" baseLine="18.924" >
			<div class="column" hostname="TESTING03" configuration="WIN32" executable="Hybrid" branch="P3_DEV_max__komarin" benchmark="WIN32" name="komarin"></div>
			<div class="column" hostname="TESTING03" configuration="WIN32" executable="Hybrid" branch="P3_DEV_enablewgfx_max__komarin" benchmark="WIN32" name="komarin_enablewgfx"></div>
		</div>
		<div class="chart" style="width:45%" title="Frame Time Ms baseLine=12.983 ms" type="wot_frame_time" baseLine="12.983" >
			<div class="column" hostname="TESTING03" configuration="WIN32" executable="Hybrid" branch="P3_DEV_max__steppes" benchmark="WIN32" name="steppes"></div>
			<div class="column" hostname="TESTING03" configuration="WIN32" executable="Hybrid" branch="P3_DEV_enablewgfx_max__steppes" benchmark="WIN32" name="steppes_enablewgfx"></div>
		</div>
		<div class="chart" style="width:45%" title="Frame Time Ms baseLine=18.481 ms" type="wot_frame_time" baseLine="18.481" >
			<div class="column" hostname="TESTING03" configuration="WIN32" executable="Hybrid" branch="P3_DEV_max__winter" benchmark="WIN32" name="winter"></div>
			<div class="column" hostname="TESTING03" configuration="WIN32" executable="Hybrid" branch="P3_DEV_enablewgfx_max__winter" benchmark="WIN32" name="winter_enablewgfx"></div>
		</div>
	</div>
	
	
	<div class="chart-category">
		<h1><a href="http://ci.bigworldtech.com/view/ARI/job/test_wot_ari_dev_performance_test/">//wot/branches/features/ARI/dev/game/ (Hybrid) - High settings - Windows7</a></h1>
			<div class="chart" style="width:45%" title="Frame Time Ms baseLine=14.063 ms" type="wot_frame_time" baseLine="14.063" >
				<div class="column" hostname="TESTING03" configuration="WIN32" executable="Hybrid" branch="P3_DEV_high__himmelsdorf" benchmark="WIN32" name="himmelsdorf"></div>
				<div class="column" hostname="TESTING03" configuration="WIN32" executable="Hybrid" branch="P3_DEV_enablewgfx_high__himmelsdorf" benchmark="WIN32" name="himmelsdorf_enablewgfx"></div>
			</div>
			<div class="chart" style="width:45%" title="Frame Time Ms baseLine=17.324 ms" type="wot_frame_time" baseLine="17.324" >
				<div class="column" hostname="TESTING03" configuration="WIN32" executable="Hybrid" branch="P3_DEV_high__komarin" benchmark="WIN32" name="komarin"></div>
				<div class="column" hostname="TESTING03" configuration="WIN32" executable="Hybrid" branch="P3_DEV_enablewgfx_high__komarin" benchmark="WIN32" name="komarin_enablewgfx"></div>
			</div>
			<div class="chart" style="width:45%" title="Frame Time Ms baseLine=12.051 ms" type="wot_frame_time" baseLine="12.051" >
				<div class="column" hostname="TESTING03" configuration="WIN32" executable="Hybrid" branch="P3_DEV_high__steppes" benchmark="WIN32" name="steppes"></div>
				<div class="column" hostname="TESTING03" configuration="WIN32" executable="Hybrid" branch="P3_DEV_enablewgfx_high__steppes" benchmark="WIN32" name="steppes_enablewgfx"></div>
			</div>
			<div class="chart" style="width:45%" title="Frame Time Ms baseLine=17.198 ms" type="wot_frame_time" baseLine="17.198" >
				<div class="column" hostname="TESTING03" configuration="WIN32" executable="Hybrid" branch="P3_DEV_high__winter" benchmark="WIN32" name="winter"></div>
				<div class="column" hostname="TESTING03" configuration="WIN32" executable="Hybrid" branch="P3_DEV_enablewgfx_high__winter" benchmark="WIN32" name="winter_enablewgfx"></div>
			</div>
		</div>

	</div>
	
	<div class="chart-category">
		<h1><a href="http://ci.bigworldtech.com/view/ARI/job/test_wot_ari_dev_performance_test/">//wot/branches/features/ARI/dev/game/ (Hybrid) - Low settings - Windows7</a></h1>
			<div class="chart" style="width:45%" title="Frame Time Ms baseLine=9.157 ms" type="wot_frame_time" baseLine="9.157" >
				<div class="column" hostname="TESTING03" configuration="WIN32" executable="Hybrid" branch="P3_DEV_low__himmelsdorf" benchmark="WIN32" name="himmelsdorf"></div>
				<div class="column" hostname="TESTING03" configuration="WIN32" executable="Hybrid" branch="P3_DEV_enablewgfx_low__himmelsdorf" benchmark="WIN32" name="himmelsdorf_enablewgfx"></div>
			</div>
			<div class="chart" style="width:45%" title="Frame Time Ms baseLine=9.642 ms" type="wot_frame_time" baseLine="9.642" >
				<div class="column" hostname="TESTING03" configuration="WIN32" executable="Hybrid" branch="P3_DEV_low__komarin" benchmark="WIN32" name="komarin"></div>
				<div class="column" hostname="TESTING03" configuration="WIN32" executable="Hybrid" branch="P3_DEV_enablewgfx_low__komarin" benchmark="WIN32" name="komarin_enablewgfx"></div>
			</div>
			<div class="chart" style="width:45%" title="Frame Time Ms baseLine=9.038 ms" type="wot_frame_time" baseLine="9.038" >
				<div class="column" hostname="TESTING03" configuration="WIN32" executable="Hybrid" branch="P3_DEV_low__steppes" benchmark="WIN32" name="steppes"></div>
				<div class="column" hostname="TESTING03" configuration="WIN32" executable="Hybrid" branch="P3_DEV_enablewgfx_low__steppes" benchmark="WIN32" name="steppes_enablewgfx"></div>
			</div>
			<div class="chart" style="width:45%" title="Frame Time Ms baseLine=9.039 ms" type="wot_frame_time" baseLine="9.039" >
				<div class="column" hostname="TESTING03" configuration="WIN32" executable="Hybrid" branch="P3_DEV_low__winter" benchmark="WIN32" name="winter"></div>
				<div class="column" hostname="TESTING03" configuration="WIN32" executable="Hybrid" branch="P3_DEV_enablewgfx_low__winter" benchmark="WIN32" name="winter_enablewgfx"></div>
			</div>
		</div>
	</div>
	
	
	<div class="chart-category">
		<h1><a href="http://ci.bigworldtech.com/view/ARI/job/test_wot_ari_dev_XP_analyze_upl/">//wot/branches/features/ARI/dev/game/ (Hybrid) - XP </a></h1>
			<div class="chart" style="width:45%" title="Frame Time Ms baseLine=83.2 ms" type="wot_frame_time"  baseLine="83.2" >
				<div class="column" hostname="TESTING02" configuration="WIN32" executable="Hybrid" branch="XP_Batch_P3_DEV_high__himmelsdorf" benchmark="WIN32" name="high_himmelsdorf"></div>
			</div>
			<div class="chart" style="width:45%" title="Frame Time Ms baseLine=70.037" type="wot_frame_time" baseLine="70.037" >
				<div class="column" hostname="TESTING02" configuration="WIN32" executable="Hybrid" branch="XP_Batch_P3_DEV_low__himmelsdorf" benchmark="WIN32" name="low_himmelsdorf"></div>
			</div>
			<div class="chart" style="width:45%" title="Frame Time Ms baseLine=20.365 ms" type="wot_frame_time"  baseLine="20.365" >
				<div class="column" hostname="TESTING02" configuration="WIN32" executable="Hybrid" branch="XP_Batch_P3_DEV_high__steppes" benchmark="WIN32" name="high_steppes"></div>
			</div>
			<div class="chart" style="width:45%" title="Frame Time Ms baseLine=19.123" type="wot_frame_time" baseLine="19.123" >
				<div class="column" hostname="TESTING02" configuration="WIN32" executable="Hybrid" branch="XP_Batch_P3_DEV_low__steppes" benchmark="WIN32" name="low_steppes"></div>
			</div>
		</div>
	</div>
		
	<div class="chart-category">
		<h1><a href="http://ci.bigworldtech.com/job/test_wot_ari_dev_memory_tests/">Available Virtual Memory (KB) //wot/branches/features/ARI/dev/game/ (Hybrid) - Windows7, 51_asia</a> </h1>
		<div class="chart" style="width:30%" title="Available Virtual Memory (KB) - Max settings baseLine=3052199" type="wot_total_mem" baseLine="3052199">
			<div class="column" hostname="TESTING04" configuration="WIN32" executable="Hybrid" branch="ARI_FREE_MEM_max_51_asia" benchmark="WIN32" name="Ari dev"></div>
			<div class="column" hostname="TESTING04" configuration="WIN32" executable="Hybrid" branch="ARI_FREE_MEM_enablewgfx_max_51_asia" benchmark="WIN32" name="Ari dev_enablewgfx"></div>
		</div>

		<div class="chart" style="width:30%" title="Available Virtual Memory (KB) - High settings baseLine=3143007" type="wot_total_mem" baseLine="3143007">
			<div class="column" hostname="TESTING04" configuration="WIN32" executable="Hybrid" branch="ARI_FREE_MEM_high_51_asia" benchmark="WIN32" name="Ari dev"></div>
			<div class="column" hostname="TESTING04" configuration="WIN32" executable="Hybrid" branch="ARI_FREE_MEM_enablewgfx_high_51_asia" benchmark="WIN32" name="Ari dev_enablewgfx"></div>
		</div>
		
		<div class="chart" style="width:30%" title="Available Virtual Memory (KB) - Low settings baseLine=3208302" type="wot_total_mem" baseLine="3208302">
			<div class="column" hostname="TESTING04" configuration="WIN32" executable="Hybrid" branch="ARI_FREE_MEM_low_51_asia" benchmark="WIN32" name="Ari dev"></div>
			<div class="column" hostname="TESTING04" configuration="WIN32" executable="Hybrid" branch="ARI_FREE_MEM_enablewgfx_low_51_asia" benchmark="WIN32" name="Ari dev_enablewgfx"></div>
		</div>
	</div>
	
	<div class="chart-category">
		<h1><a href="http://ci.bigworldtech.com/job/test_wot_ari_dev_memory_tests/">Frame Time Ms (CAT test) //wot/branches/features/ARI/dev/game/ (Hybrid) - Windows7, 51_asia</a> </h1>
		<div class="chart" style="width:30%" title="Frame Time Ms - Max settings baseLine=7.85161538462" type="wot_frame_time" baseLine="7.85161538462">
			<div class="column" hostname="TESTING04" configuration="WIN32" executable="Hybrid" branch="ARI_FREE_MEM_max_51_asia" benchmark="WIN32" name="Ari dev"></div>
			<div class="column" hostname="TESTING04" configuration="WIN32" executable="Hybrid" branch="ARI_FREE_MEM_enablewgfx_max_51_asia" benchmark="WIN32" name="Ari dev_enablewgfx"></div>
		</div>

		<div class="chart" style="width:30%" title="Frame Time Ms - High settings baseLine=7.84214102564" type="wot_frame_time" baseLine="7.84214102564">
			<div class="column" hostname="TESTING04" configuration="WIN32" executable="Hybrid" branch="ARI_FREE_MEM_high_51_asia" benchmark="WIN32" name="Ari dev"></div>
			<div class="column" hostname="TESTING04" configuration="WIN32" executable="Hybrid" branch="ARI_FREE_MEM_enablewgfx_high_51_asia" benchmark="WIN32" name="Ari dev_enablewgfx"></div>
		</div>
		
		<div class="chart" style="width:30%" title="Frame Time Ms - Low settings baseLine=7.84048717949" type="wot_frame_time" baseLine="7.84048717949">
			<div class="column" hostname="TESTING04" configuration="WIN32" executable="Hybrid" branch="ARI_FREE_MEM_low_51_asia" benchmark="WIN32" name="Ari dev"></div>
			<div class="column" hostname="TESTING04" configuration="WIN32" executable="Hybrid" branch="ARI_FREE_MEM_enablewgfx_low_51_asia" benchmark="WIN32" name="Ari dev_enablewgfx"></div>
		</div>
	</div>
							
	<div style="clear:both"></div>
	<p>
		<small>[Page generated by <?php echo $hostname?>:<?php echo __FILE__;?>]</small>
	</p>
</body>
</html>
