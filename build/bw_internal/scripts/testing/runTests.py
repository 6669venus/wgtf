import csv
import glob
import optparse
import os
from os.path import basename
from os.path import splitext
import shutil
import subprocess
import sys
import time
import threading
import reporter
import util
import stat
import socket
import datetime

import constants

sys.path.append("tools")
from graphite_client.graphite_metric_list import GraphiteMetricList
from graphite_client.graphite_metric import GraphiteMetric
from graphite_client.graphite_event import GraphiteEvent
from graphite_client.graphite_connector import GraphitePickleConnector
from graphite_client.graphite_connector import GraphiteStat

BATCH_COMPILER	= "batch_compiler"
WORLDEDITOR		= "worldeditor"
MODELEDITOR		= "modeleditor"
PARTICLEEDITOR	= "particleeditor"
NAVGEN			= "navgen"
GENERIC_APP		= "generic_app"
GENERIC_APP_TEST= "generic_app_test"
TOOLS			= [
	WORLDEDITOR,
	MODELEDITOR,
	PARTICLEEDITOR,
	NAVGEN
]
NEW_TOOLS		= [
	GENERIC_APP,
	GENERIC_APP_TEST
]
BWCLIENT		= "bwclient"
CLIENT			= [
	BWCLIENT
]
EXECUTABLES		= [
	WORLDEDITOR,
	MODELEDITOR,
	PARTICLEEDITOR,
	NAVGEN,
	BWCLIENT,
	GENERIC_APP,
	GENERIC_APP_TEST
]
TIMEOUT			= 600
LOCAL_HOST_NAME = socket.gethostname().split('.')[0]

PACKAGE_ROOT	= os.getcwd()
DATA_DIR		= os.path.join( os.path.dirname( __file__ ), "data" )
SMOKE_TESTS_DIR = os.path.join( DATA_DIR, "smoke_tests" )
GAME_RESOURCE_PATH= os.path.join( PACKAGE_ROOT, "game/res/fantasydemo" )
TOOLS_SCRIPT_DIR= os.path.join( PACKAGE_ROOT, "game/res/fantasydemo/scripts/editor" )
CLIENT_SCRIPT_DIR= os.path.join( PACKAGE_ROOT, "game/res/fantasydemo/scripts/client" )
SPACE_DIR= os.path.join( PACKAGE_ROOT, "game/res/fantasydemo/spaces" )

BAK_FILE		= "%s.bak"
MEM_STATS_FILE	= "memStats*.csv"
SPACE_SETTING_FILE = "space.settings"
PREFERENCES_FILE = "preferences.xml"
NO_CONVERSION_FLAG = "-noConversion"
MEM_DUMP_STATS_FLAG = "-memdumpstats"
UNATTENDED_FLAG = "-unattended"
DEFAULT_ARGS = "%s %s %s" % (NO_CONVERSION_FLAG, MEM_DUMP_STATS_FLAG, UNATTENDED_FLAG)

SLOT_NAME = "Slot name"
SLOT_NAME_TOTAL_ALLOC = "Total allocations (Heap + Pool + Internal)"
MEM_LEAK_BYTES = " Memory leaked (bytes)"
MEM_LEAK_COUNT = " Memory leak count "
MEM_PEAK_BYTES = " Peak allocation (bytes)"
MEM_TOTAL_ALLOC_COUNT = " Total allocation count"

def forceDelete( path ):
	if os.path.exists(path):
		os.chmod(path, stat.S_IWUSR)
		os.remove(path)

def processMemStats( exePath ):
	exeDir = os.path.dirname( exePath )
	memStatsPath = os.path.join( exeDir, MEM_STATS_FILE )

	mem_stat_files = glob.glob( memStatsPath )
	if len(mem_stat_files) == 0:
		print "Memory stats report not found for ", memStatsPath
		return [0, 0, 0, 0]

	memStatsFile = sorted( mem_stat_files )[-1]

	with open( memStatsFile ) as csvFile:
		for row in csv.DictReader( csvFile ):
			if row[SLOT_NAME] == SLOT_NAME_TOTAL_ALLOC:
				slot = row
				break

	forceDelete( memStatsFile )

	return [
		int( slot[MEM_LEAK_BYTES] ),
		int( slot[MEM_LEAK_COUNT] ),
		int( slot[MEM_PEAK_BYTES] ),
		int( slot[MEM_TOTAL_ALLOC_COUNT] ) ]

def _initGraphite():
	graphite = GraphitePickleConnector( constants.GRAPHITE_HOST )
	graphite.set_http_port( constants.GRAPHITE_PORT )
	return graphite

def _sendGraphiteValue( stat_path, testName, branchName, changelist, configuration,
			 successState, timeToRun, totalMemoryAllocations,
			 peakAllocatedBytes, memoryLeaks ):
	graphite = _initGraphite()
	successState_stat_path = ".".join(
		stat_path + [constants.GRAPHITE_STAT_SUCCESSSTATE] )
	timeToRun_stat_path = ".".join(
		stat_path + [constants.GRAPHITE_STAT_TIMETORUN] )
	peakAllocatedBytes_stat_path = ".".join(
		stat_path + [constants.GRAPHITE_STAT_PEAKALLOCBYTES] )
	totalMemoryAllocations_stat_path = ".".join(
		stat_path + [constants.GRAPHITE_STAT_TOTALMEMALLOCS] )
	memoryLeaks_stat_path = ".".join(
		stat_path + [constants.GRAPHITE_STAT_MEMLEAKCOUNT] )
	event_stat_path = ".".join( stat_path  )
	annotation = "Perf %s" % event_stat_path
	attributes = {
		'testName': GraphiteStat.clean( testName ),
		'branchName': GraphiteStat.clean( branchName ),
		'configuration': GraphiteStat.clean( configuration ),
		'successState': GraphiteStat.clean( successState ),
		'timeToRun': timeToRun,
		'totalMemoryAllocations': totalMemoryAllocations,
		'peakAllocatedBytes': peakAllocatedBytes,
		'memoryLeaks': memoryLeaks,
		'changelist': changelist
	}
	timestamp = datetime.datetime.today()
	successStatVal = 0 if successState == "OK" else 1
	successState_metric = GraphiteMetric( successState_stat_path,
										  successStatVal, timestamp )
	timeToRun_metric = GraphiteMetric( timeToRun_stat_path, timeToRun )
	peakAllocatedBytes_metric = GraphiteMetric( peakAllocatedBytes_stat_path,
		peakAllocatedBytes, timestamp )
	totalMemoryAllocations_metric = GraphiteMetric(
		totalMemoryAllocations_stat_path, totalMemoryAllocations, timestamp )
	memoryLeaks_metric = GraphiteMetric( memoryLeaks_stat_path, memoryLeaks,
		timestamp )
	metric_list = GraphiteMetricList()
	metric_list.name = event_stat_path
	metric_list.timestamp = timestamp
	metric_list.add_metric( successState_metric )
	metric_list.add_metric( timeToRun_metric )
	metric_list.add_metric( peakAllocatedBytes_metric )
	metric_list.add_metric( totalMemoryAllocations_metric )
	metric_list.add_metric( memoryLeaks_metric )
	graphite.connect()
	graphite.send_annotated_metric( metric_list, annotation,
		" ".join( ['perf', event_stat_path] ), attributes )

def _createCleanStatPath( pathArray ):
	cleanPath = []
	for e in pathArray:
		cleanPath.append( GraphiteStat.clean( e ) )

#run program with timeout
class Command(object):
	def __init__(self, cmd):
		self.cmd = cmd
		self.process = None
		self.returncode = 1
		self.subprocess_output = ""

	def run(self, timeout):
		def target():
			# Split the cmd string into a list of arguments for Popen
			import shlex
			args = shlex.split( self.cmd )

			try:
				self.process = subprocess.Popen( args,
					stdout=subprocess.PIPE,
					stderr=subprocess.STDOUT )
				(self.subprocess_output, unused_stderr) = \
					self.process.communicate()
				self.returncode = self.process.poll()
			except Exception, e:
				# This can happen if the exe_path does not exist
				print e

		start_time = time.time()
		thread = threading.Thread(target=target)
		thread.start()

		thread.join(timeout)
		if thread.is_alive():
			self.process.terminate()
			thread.join()
			end_time = time.time()
			timeToRun = end_time - start_time
			self.subprocess_output =  "TIMEOUT ERROR. Total Time: " + str(timeToRun)
			self.returncode = 1

		return self.subprocess_output


def runTest( target, test, reportHolder, branchName, changelist, dbType, flags, submitToGraphite ):

	testItems = []
	try:
		moduleName = "data.smoke_tests." + target
		import importlib
		testModule = importlib.import_module( moduleName )
		# Look up test
		testItems = testModule.TEST_ITEMS
	except ImportError, e:
		print e

	if not testItems:
		print "No test data for", target
		return

	for item in testItems:
		if item[ "target" ] != target:
			print "Error test item %s does not match %s\n" % \
				(item[ "target" ], target)
		else:
			_runTest(
				reportHolder,
				branchName,
				changelist,
				dbType,
				flags,
				item,
				submitToGraphite )


def _generateDefaultExePath( dirName, buildConfig, exeName ):
	binPath = os.path.join( "programming", "ngt", "bin", dirName, buildConfig.lower() )
	binPath = os.path.normpath( binPath )
	exePath = os.path.join( binPath, exeName )
	exePath = os.path.normpath( exePath )
	return (binPath, exePath)

def _generateTestOptionsPaths( target, options_file ):

	options_path = ""
	options_path_bak = ""

	if options_file:
		options_path = os.path.join( SMOKE_TESTS_DIR, target, options_file )
		options_path = os.path.normpath( options_path )

	if options_path != "":
		options_path_bak = BAK_FILE % ( options_path )
		options_path_bak = os.path.normpath( options_path_bak )
		forceDelete( options_path_bak )
		shutil.copy( options_path, options_path_bak )

	return ("", options_path, options_path_bak)

def _generateTestSettingsPaths( target, settings_file ):

	settings_path = ""
	settings_path_bak = ""

	if settings_file:
		settings_path = os.path.join( SMOKE_TESTS_DIR, target, settings_file )
		settings_path = os.path.normpath( settings_path )

	if settings_path != "":
		settings_path_bak = BAK_FILE % ( settings_path )
		settings_path_bak = os.path.normpath( settings_path_bak )
		forceDelete( settings_path_bak )
		shutil.copy( settings_path, settings_path_bak )

	return ("", settings_path, settings_path_bak)

def _generateTestPluginsPaths( target, binPath, pluginsFile ):
	pluginsBin = os.path.join( binPath, "plugins" )

	originalFile = os.path.join( pluginsBin, pluginsFile )
	backupFile = BAK_FILE % (originalFile,)
	if not os.path.exists( originalFile ):
		originalFile = ""

	testFile = os.path.join( SMOKE_TESTS_DIR, target, pluginsFile )
	if not os.path.exists( testFile ):
		testFile = ""

	return (originalFile, testFile, backupFile)

def _generateTestScriptPaths( target, scriptDir, scriptFile ):
	# copy the test script to the resource directory
	scriptFile = scriptFile + ".py"
	temp_script_path = os.path.join( scriptDir, scriptFile )
	temp_script_path = os.path.normpath( temp_script_path )

	scriptPath = os.path.join( SMOKE_TESTS_DIR, target, scriptFile )
	scriptPath = os.path.normpath( scriptPath )

	return (temp_script_path, scriptPath, "")

def _generateTestPreferencesPaths( binPath, target, preferencesFile ):
	preferences_file_path = os.path.join( binPath, PREFERENCES_FILE )
	preferences_file_path = os.path.normpath( preferences_file_path )

	testFile = os.path.join( SMOKE_TESTS_DIR, target, preferencesFile )
	testFile = os.path.normpath( testFile )
	backupFile = BAK_FILE % (preferences_file_path,)
	backupFile = os.path.normpath( backupFile )

	return (preferences_file_path, testFile, backupFile)

#def _generateTestSpacePaths( target, scriptName ):
#	#search for the space name
#	test_name = splitext( scriptName )[0]
#
#	test_name_array = test_name.split( '_' )
#	space_name = test_name_array[len( test_name_array )-1]
#
#	#search for a space setting file
#	temp_path = os.path.join( SPACE_DIR, space_name )
#	temp_path = os.path.normpath( temp_path )
#	org_space_setting_file = os.path.join( temp_path, SPACE_SETTING_FILE )
#	space_setting_path_bak = BAK_FILE % ( org_space_setting_file )
#
#	#search for the new space setting file
#	tmp_space_setting_file = "%s.%s"  % ( test_name, SPACE_SETTING_FILE )
#	new_space_setting_file = os.path.join( DATA_DIR, tmp_space_setting_file )
#	new_space_setting_file = os.path.normpath( new_space_setting_file )
#
#	return (org_space_setting_file,
#			new_space_setting_file,
#			space_setting_path_bak)

def _runTest(
	reportHolder,
	branchName,
	changelist,
	dbType,
	flags,
	item,
	submitToGraphite ):

	# -- Build command line
	(binPath, exe_path) = _generateDefaultExePath(
		item[ "bin" ],
		item[ "build config" ],
		item[ "exe" ] )

	# (Original file, Test file, Backup file)
	setupFiles = []

	cmd_args = DEFAULT_ARGS
	if item[ "args" ]:
		# Overwrite default args
		cmd_args = item[ "args" ]

	if item[ "options" ]:
		optionsPaths = \
			_generateTestOptionsPaths( item[ "target" ], item[ "options" ] )

		if optionsPaths[ 1 ]:
			setupFiles.append( optionsPaths )
			cmd_args += " --options \"%s\"" % (optionsPaths[ 1 ],)

	if item[ "settings" ]:
		settingsPaths = \
			_generateTestSettingsPaths( item[ "target" ], item[ "settings" ] )

		if settingsPaths[ 1 ]:
			setupFiles.append( settingsPaths )
			cmd_args += " --settings \"%s\"" % (settingsPaths[ 1 ],)

	if item[ "plugins" ]:
		testPluginsPaths = \
			_generateTestPluginsPaths(
				item[ "target" ], binPath, item[ "plugins" ] )

		if testPluginsPaths[ 1 ]:
			setupFiles.append( testPluginsPaths )
			cmd_args = cmd_args + " --config " + item[ "plugins" ]

	scriptDir = TOOLS_SCRIPT_DIR
	if item[ "target" ] == BWCLIENT:
		scriptDir = CLIENT_SCRIPT_DIR

	if item[ "script" ]:
		scriptPaths = \
			_generateTestScriptPaths(
				item[ "target" ], scriptDir, item[ "script" ] )

		if scriptPaths[ 1 ]:
			cmd_args += " --script " + item[ "script" ]
			setupFiles.append( scriptPaths )

	# copy the bwclient_preferences.xml to the bin folder
	if item[ "preferences" ]:
		preferencesPaths = \
			_generateTestPreferencesPaths(
				binPath, item[ "target" ], item[ "preferences" ] )

		if preferencesPaths[ 1 ]:
			setupFiles.append( preferencesPaths )

	timeout = TIMEOUT
	if item[ "timeout" ]:
		timeout = item[ "timeout" ]

	#if item[ "copy space settings" ]:
	#	spacePaths = \
	#		_generateTestSpacePaths( item[ "target" ], item[ "script" ] )

	#	if spacePaths[ 1 ]:
	#		setupFiles.append( spacePaths )

	cmd_args += " " + flags

	# -- Set up test files
	for (originalFile, testFile, backupFile) in setupFiles:
		if testFile and os.path.exists( testFile ):
			# Make a backup
			if backupFile:
				forceDelete( backupFile )
				if originalFile and os.path.exists( originalFile ):
					print "Moving %s to %s" % (originalFile, backupFile)
					shutil.move( originalFile, backupFile )
			# Copy test file over
			if originalFile:
				forceDelete( originalFile )
				print "Copying %s to %s" % (testFile, originalFile)
				shutil.copy( testFile, originalFile )

	# -- Run executable
	# Paths must be in quotes ""
	print "Running %s, timeout %ds..." % ( item[ "name" ], timeout )
	cmd = "\"%s\" %s" % (exe_path, cmd_args)
	print cmd

	start_time = time.time()
	command = Command(cmd)
	subprocess_output = command.run( timeout )
	res = command.returncode

	end_time = time.time()
	timeToRun = end_time - start_time
	totalMemoryAllocations = 0
	peakAllocatedBytes = 0
	memoryLeaks = 0

	# -- Clean up test files
	while setupFiles:
		(originalFile, testFile, backupFile) = setupFiles.pop()
		if os.path.exists( testFile ):
			forceDelete( originalFile )
		if os.path.exists( backupFile ):
			if originalFile:
				print "Moving %s to %s" % (backupFile, originalFile)
				shutil.move( backupFile, originalFile )
			else:
				forceDelete( backupFile )

		# Clean up pyc files
		if testFile.endswith( ".py" ):
			pycFile = originalFile + "c"
			forceDelete( pycFile )

	# -- Build report
	# check if the executable exited cleanly
	test_result = "\nComplete report: %s\n" % (item[ "target" ],)
	if res != 0:
		test_result += "FAILURE: %s did not cleanly shut down.\n" % \
			(item[ "target" ],)
		successState = constants.FAIL_STAT
	else:
		test_result += "%s cleanly shut down.\n" % (item[ "target" ],)
		successState = constants.OK_STATE

		# parse the output csv file for memory leaks
		memStatsCollected = (cmd_args.find( MEM_DUMP_STATS_FLAG ) != -1)
		if memStatsCollected:
			mem_stats = processMemStats( exe_path )
			if mem_stats[0] == 0:
				test_result += "no leaks detected.\n"
			elif not item[ "ignore leaks" ]:
				test_result += \
					"FAILURE: %s bytes leaked with a total of %s leaks.\n" % \
					(mem_stats[0], mem_stats[1])
				res = 1
			else:
				test_result += "IGNORED: %s bytes leaked with a total of %s leaks.\n" % ( mem_stats[0], mem_stats[1] )
			totalMemoryAllocations = mem_stats[3]
			peakAllocatedBytes = mem_stats[2]
			memoryLeaks = mem_stats[1]
		else:
			totalMemoryAllocations = 0
			peakAllocatedBytes = 0
			memoryLeaks = 0
			test_result += "Memory stats not collected\n"

	if res != 0 and item[ "known issue" ]:
		test_result += "IGNORED: known issue %s\n" % (item[ "known issue" ],)
		res = 0
	elif res == 0 and item[ "known issue" ]:
		test_result += ("WARNING: Test passed, but known issue %s "
			"is still in the test config, please remove it\n" %
			(item[ "known issue" ],))

	# Add subprocess output to the result
	test_result += subprocess_output.replace( "\r","" )

	print test_result

	# Add to DB
	addToDB( dbType,
		item[ "name" ],
		branchName,
		changelist,
		item[ "build config" ].upper(),
		successState,
		timeToRun,
		totalMemoryAllocations,
		peakAllocatedBytes,
		memoryLeaks,
		submitToGraphite )

	reportHolder.addReport( reporter.Report(
		res == 0, item[ "name" ], test_result, branchName ) )


def addToDB( dbType, test_name, branchName, changelist, configuration,
			 successState, timeToRun, totalMemoryAllocations,
			 peakAllocatedBytes, memoryLeaks, submitToGraphite ):
	if dbType == constants.DB_PRODUCTION:
		import database
		dbSession = database.createSession( dbType )
		# Add to MYSQL server
		database.SmokeTestResult.addResult( dbSession, test_name, branchName,
											changelist, configuration,
											successState, timeToRun,
											totalMemoryAllocations,
											peakAllocatedBytes, memoryLeaks )
		if submitToGraphite:
			# Send data to Graphite
			stat_path =  constants.GRAPHITE_STAT_PREFIX_SMOKE + \
						 _createCleanStatPath( [branchName, test_name,
						configuration, LOCAL_HOST_NAME] )
			_sendGraphiteValue( stat_path, test_name, branchName, changelist,
								configuration, successState, timeToRun,
								totalMemoryAllocations, peakAllocatedBytes,
								memoryLeaks )

def runTests():
	bwversion = util.bigworldVersion()
	usage = "usage: %prog [options] test"

	parser = optparse.OptionParser( usage )

	parser.add_option( "-e", "--executable",
					   dest = "executable", default=None,
					   help = "Specific executable to test\n%s" % ( EXECUTABLES ) )

	parser.add_option( "--no-tools",
						action = "store_false",
						dest = "tools",
						help = "Disable %s tools tests." % (TOOLS,) )
	parser.set_defaults( tools=True )

	parser.add_option( "--no-new-tools",
						action = "store_false",
						dest = "new_tools",
						help = "Disable %s tools tests." % (NEW_TOOLS,) )
	parser.set_defaults( new_tools=True )

	parser.add_option( "--no-client",
						action = "store_false",
						dest = "client",
						help = "Disable client tests." )
	parser.set_defaults( client=True )

	parser.add_option( "--no-batch-compiler",
						action = "store_false",
						dest = "batch_compiler",
						help = "Disable running batch compiler before the other tests." )
	parser.set_defaults( batch_compiler=True )

	parser.add_option( "--compiled_space",
						action = "store_true",
						dest = "compiled_space",
						help = "Change space type from chunk space to compiled space" )

	parser.add_option( "-g", "--submit_to_graphite",
					action = "store_true",
					dest = "submit_to_graphite", default=False,
					help = "Submit stats to graphite host: " + constants.GRAPHITE_HOST )

	parser.add_option( "-u", "--url ",
					   dest = "url", default=None,
					   help = "The URL for the results in jenkins" )

	parser.add_option( "-d", "--database",
					dest = "database_type", default=constants.DB_MEMORY,
					help = "Database engine: " + "|".join( constants.DB_TYPES ) \
							+ " (default=" + constants.DB_MEMORY + ")" )

	parser.add_option( "-b", "--branchName",
					dest = "branchName", default="",
					help = "Specify branch name, default=bw_%d_%d" % ( bwversion[0], bwversion[1] ) )

	parser.add_option( "--changelist",
					dest = "changelist", default=0, type=int,
					help = "Specify p4 changelist number" )

	(options, args) = parser.parse_args()

	branchName = "bw_%d_%d" % ( bwversion[0], bwversion[1] )

	if not options.branchName == "":
		branchName += "_" + options.branchName

	test = "smoketest"
	try:
		test = args[0].lower()
	except:
		pass

	if (options.executable != None and options.executable not in EXECUTABLES) \
		or options.database_type not in constants.DB_TYPES :
	   parser.print_help()
	   return 1

	dbType = options.database_type

	reportHolder = reporter.ReportHolder( "Automated Testing",
			"%s on %s" % ( test, branchName ), options.url, options.changelist )

	engineXMLPath = util.engineConfigXML( GAME_RESOURCE_PATH )


	if util.replaceLineInFile( engineXMLPath, engineXMLPath,
		"<spaceType> COMPILED_SPACE </spaceType>",
		"<spaceType> CHUNK_SPACE </spaceType>" ):
		print "Replace <spaceType> COMPILED_SPACE </spaceType> with <spaceType> CHUNK_SPACE </spaceType>"

	flags = ""
	if options.compiled_space:
		flags += "-spaceType COMPILED_SPACE"

	if options.executable != None:
		runTest( options.executable, test, reportHolder, branchName,
				 options.changelist, dbType, flags, options.submit_to_graphite)
	else:

		# Run BATCH_COMPILER before other tests
		if (options.batch_compiler and
			(options.tools or options.new_tools or options.client)):
			runTest( BATCH_COMPILER,
				test,
				reportHolder,
				branchName,
				options.changelist,
				dbType,
				flags,
				options.submit_to_graphite )

		for executable in EXECUTABLES:
			if executable in TOOLS and options.tools == True:
				runTest( executable, test, reportHolder, branchName,
					options.changelist, dbType, flags, options.submit_to_graphite)
			if executable in NEW_TOOLS and options.new_tools == True:
				runTest( executable, test, reportHolder, branchName,
					options.changelist, dbType, flags,
					options.submit_to_graphite)
			if executable in CLIENT and options.client == True:
				runTest( executable, test, reportHolder, branchName,
					options.changelist, dbType, flags,
					options.submit_to_graphite)


	reportHolder.sendMail()

	for report in reportHolder.reports:
		if not report.success:
			return 1

	return 0

if __name__ == "__main__":
	sys.exit( runTests() )
