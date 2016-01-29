import os, sys
import shutil
import util
import socket
import database
import reporter
import open_automate
import stat

from util import BWTestingError
from constants import *
from database import ReferenceValue, FrameRateResult

sys.path.append("tools")
from graphite_client.graphite_metric_list import GraphiteMetricList
from graphite_client.graphite_metric import GraphiteMetric
from graphite_client.graphite_event import GraphiteEvent
from graphite_client.graphite_connector import GraphitePickleConnector
from graphite_client.graphite_connector import GraphiteStat
TEST_OUTPUT_FILE = "test_log.log"

OO_STANDARD_TEST = "testAdvancedRunBenchmark"

REFERENCE_CALCULATION_RUN_COUNT = 2

LOCAL_HOST_NAME = socket.gethostname().split('.')[0]

def _resetTestOutput():
	try:
		os.remove( TEST_OUTPUT_FILE )
	except OSError:
		pass


def _fpsResultInMS( resultValues ):
	fpsResults = [ float(x) for x in resultValues["FPS"] ]
	avg = sum( fpsResults ) / len ( fpsResults )
	return 1000.0 / avg

def _initGraphite():
	graphite = GraphitePickleConnector( GRAPHITE_HOST )
	graphite.set_http_port( GRAPHITE_PORT )
	return graphite

def _sendGraphiteRefValue( stat_path, branchName, buildConfig,
		benchmarkName, value ):
	graphite = _initGraphite()
	attributes = {
		'branchName': GraphiteStat.clean( branchName ),
		'buildConfig': GraphiteStat.clean( buildConfig ),
		'benchmarkName': GraphiteStat.clean( benchmarkName ),
		'hostname': GraphiteStat.clean( LOCAL_HOST_NAME ),
		'type': GRAPHITE_STAT_REFVALUE,
		'value': value
	}
	ref_val_stat_path = ".".join( stat_path + [GRAPHITE_STAT_REFVALUE] )
	event = GraphiteEvent( "Reference Value",
		" ".join( ['perf', ref_val_stat_path] ), attributes  )
	graphite.send_event( event )
	
def _sendGraphiteValue( stat_path, branchName, buildConfig,
		benchmarkName, changelist, value ):
	graphite = _initGraphite()
	val_stat_path = ".".join( stat_path + [GRAPHITE_STAT_VALUE] )
	annotation = "Perf %s" % val_stat_path
	attributes = {
		'branchName': GraphiteStat.clean( branchName ),
		'buildConfig': GraphiteStat.clean( buildConfig ),
		'benchmarkName': GraphiteStat.clean( benchmarkName ),
		'hostname': GraphiteStat.clean( LOCAL_HOST_NAME ),
		'changelist': changelist,
		'value': value
	}
	metric = GraphiteMetric(val_stat_path, value )
	graphite.connect()
	graphite.send_annotated_metric( metric, annotation,
		" ".join( ['perf', val_stat_path] ), attributes )

def _getGraphiteEvents( tags, from_d, until_d):
	graphite = _initGraphite()
	return graphite.get_events( ['perf',".".join( tags )], from_d, until_d )

def _createCleanStatPath( pathArray ):
	cleanPath = []
	for e in pathArray:
		cleanPath.append( GraphiteStat.clean( e ) )
	return cleanPath

def _calculateReferenceValue( buildConfig, exeType, flags, branchName, exePath,
		resPath, ooScript, scriptIndex, dbSession, submitToGraphite,
		graphite_stat_path, benchmarkTestName ):
	# We don't want to clobber any test results that we have so far. Backup
	# the test output file, then reset for a clean reference test.
	try:
		if os.path.exists( TEST_OUTPUT_FILE+".bak" ):
			os.remove( TEST_OUTPUT_FILE+".bak" )
		os.rename( TEST_OUTPUT_FILE, TEST_OUTPUT_FILE+".bak" )
	except OSError:
		pass
		
	# Run the test (a few times to get an average)
	for x in range( REFERENCE_CALCULATION_RUN_COUNT ):
		open_automate.runScript( exePath, flags, resPath, ooScript, scriptIndex )
	
	# Parse results
	testResults = util.parseResultsFile( TEST_OUTPUT_FILE )
	flag = False
	for benchmarkName, resultValues in testResults.iteritems():
		if benchmarkName == benchmarkTestName:
			flag = True	
		calculatedValue = _fpsResultInMS( resultValues )
		print "\tReference for %s:%s (%s %s) is now %.2fms" % \
			(branchName, benchmarkName, buildConfig, exeType, calculatedValue)
			
		ReferenceValue.set( dbSession, buildConfig, exeType, branchName, benchmarkName, calculatedValue )
		if submitToGraphite:
			_sendGraphiteRefValue( graphite_stat_path, branchName, buildConfig,
				benchmarkName, calculatedValue )
	if not flag:
		raise BWTestingError(
				"Failed to calculate reference value for %s:%s (%s %s).	reference value was nod found in %s" % \
				(benchmarkTestName, branchName, buildConfig, exeType, TEST_OUTPUT_FILE)
			)
	# Double check everything got set (sanity check)
	benchmarkNames = open_automate.collectBenchmarkNames( resPath, OO_STANDARD_TEST, scriptIndex )
	for benchmarkName in benchmarkNames:
		if ReferenceValue.get( dbSession, buildConfig, exeType, branchName, benchmarkName ) is None:
			raise BWTestingError(
					"Failed to calculate reference value for %s:%s (%s %s)." % \
					(branchName, benchmarkName, buildConfig, exeType)
				)
	
	# Restore the backup test output
	_resetTestOutput()
	try:
		os.rename( TEST_OUTPUT_FILE+".bak", TEST_OUTPUT_FILE )
	except OSError:
		pass


def _processResults( buildConfig, exeType, branchName, dbSession,
		submitToGraphite, benchmarkAllNames, changelist ):
	testResults = util.parseResultsFile( TEST_OUTPUT_FILE )
	
	for benchmarkName in benchmarkAllNames:
		if not benchmarkName in testResults:
			raise BWTestingError( "No reference value for '%s:%s' (%s)" % \
						(branchName, benchmarkName, buildConfig, exeType ) )
			
	# Insert results into test database, and at the same time make a list of
	# which tests are considered failures based on their performance.
	failedBenchmarks = []
	for benchmarkName, resultValues in testResults.iteritems():
		stat_path =  GRAPHITE_STAT_PREFIX_FPS + _createCleanStatPath( [
			branchName, buildConfig, benchmarkName, exeType, LOCAL_HOST_NAME] )
		# Get value to compare ourselves against
		referenceValue = ReferenceValue.get( dbSession, buildConfig, exeType, branchName, benchmarkName )
		if referenceValue is None:
			raise BWTestingError( "No reference value for '%s:%s' (%s)" % \
						(branchName, benchmarkName, buildConfig, exeType ) )
		
		referenceValue = float( referenceValue.value )
		
		# Get current value in milliseconds		
		calculatedValue = _fpsResultInMS( resultValues )
		
		# Add to DB
		FrameRateResult.addResult( dbSession, buildConfig, exeType, branchName, benchmarkName, calculatedValue, changelist )
		if submitToGraphite:
			_sendGraphiteValue( stat_path, branchName, buildConfig,
				benchmarkName, changelist, calculatedValue )
		
		# Have we failed against the reference value? ie. outside 10% difference
		diffPct = (calculatedValue - referenceValue) / referenceValue
		if abs(diffPct) > 0.1:
			failedBenchmarks.append( benchmarkName )
		
	testReport = ""
	success = (len(failedBenchmarks) == 0)
		
	# Display a list of what failed up front
	if not success:
		testReport += "The following tests failed:\n"
		testReport += "____\n"
		for elem in failedBenchmarks:
			testReport += elem + "\n"
		testReport += "\n" * 2

	# Build the rest of the report (full list of successes and failures)
	testReport += "Complete report:\n"
	testReport += "____ \n"

	for benchmarkName, resultValues in testResults.iteritems():
		testReport += "\n"
		testReport += benchmarkName
		
		if benchmarkName in failedBenchmarks:
			testReport += " (FAILED)"
		
		testReport += "\n"

		if benchmarkName in testResults:
			calculatedValue = _fpsResultInMS( resultValues )

			referenceValue = ReferenceValue.get( dbSession, buildConfig, exeType, branchName, benchmarkName )
			if referenceValue is None:
				raise BWTestingError( "No reference value for '%s:%s' (%s %s)" % \
							(branchName, benchmarkName, buildConfig, exeType ) )
							
			referenceValue = float( referenceValue.value )
			
			pctDiff = ((referenceValue - calculatedValue) / calculatedValue) * 100.0
			testReport += "Current: %.4fms, Reference: %.4fms, Improvement: %+.2f%%\n" % \
							(calculatedValue, referenceValue, pctDiff)
		else:
			testReport += "Data Missing\n"

	print testReport
	
	tag = "Framerate test (%s %s)" % (buildConfig, exeType)
	
	return reporter.Report( success, tag, testReport, branchName )


def _runInternal( branchName, flags, buildConfig, exeType, exePath, resPath, 
			enableJobSystem, compiled_space, reportHolder,
			dbSession, changelist, primer_run, submitToGraphite ):
	# Be sure to flush out any existing test results
	_resetTestOutput()

	# Resolve paths for various configuration XML files
	referencePreferences = util.resolveDataFilePath( "preferences.xml" )
	preferences = os.path.normpath(
			os.path.join( os.path.dirname( exePath ), "preferences.xml" )
		)
		
	if not os.path.exists( referencePreferences ):
		raise BWTestingError( "Could not find reference preferences at '%s'." % referencePreferences )
		
	# Copy across our reference preferences.xml.
	# i.e. make sure we are running full screen with no vsync
	if os.path.exists(preferences):
		os.chmod(preferences, stat.S_IWUSR)
	shutil.copyfile( referencePreferences, preferences )
	
	# Setup appropriate engine configuration
	xmlPath = util.engineConfigXML( resPath )
	if util.replaceLineInFile( xmlPath, xmlPath, 
		"<spaceType> COMPILED_SPACE </spaceType>", 
		"<spaceType> CHUNK_SPACE </spaceType>" ):
		print "Replace <spaceType> COMPILED_SPACE </spaceType> with <spaceType> CHUNK_SPACE </spaceType>"

	if util.replaceLineInFile( xmlPath, xmlPath, 
			"<maxFrameRate>		75	</maxFrameRate>", 
			"<maxFrameRate>		0	</maxFrameRate>" ):
		print "Replace <maxFrameRate>		75	</maxFrameRate> with <maxFrameRate>		0	</maxFrameRate>"

		
	scriptIndex = 0	
	
	# Run a primer run once at the beginning and ignore the result 
	# This will 'prime' the cache with the data we're interested in
	if primer_run:
		print "--run primer run--"
		print "The results won't be saved"
		while open_automate.checkScriptExists( resPath, OO_STANDARD_TEST, scriptIndex ):
			open_automate.runScript( exePath, flags, resPath, OO_STANDARD_TEST, scriptIndex, primer_run )
			scriptIndex += 1
		# flush out results
		_resetTestOutput()
		scriptIndex = 0	
		print "--End primer run--"
		
	# Run the actual tests. There should be a finite number of numbered
	# benchmark XML files, so continue trying to launch testing until we
	# no longer have a valid XML.
	benchmarkAllNames = []
	while open_automate.checkScriptExists( resPath, OO_STANDARD_TEST, scriptIndex ):
		# Check to see if a reference value has been calculated for this test combination.
		referenceValueSet = True
		benchmarkNames = open_automate.collectBenchmarkNames( resPath, OO_STANDARD_TEST, scriptIndex )
		unsetReferences = []
		unsetGraphiteReferences = []
		# Caution. Benchmark XML files have a single benchmarkName. This
		# iteration (and arrays) may need to be removed, depending on why
		# it is here in the first place (comment author does not know). SvenS
		for benchmarkName in benchmarkNames:
			stat_path =  GRAPHITE_STAT_PREFIX_FPS + _createCleanStatPath( [
				branchName, buildConfig, benchmarkName, exeType,
				LOCAL_HOST_NAME] )
			benchmarkAllNames.append( benchmarkName )
			referenceValue = ReferenceValue.get( dbSession, buildConfig, exeType, branchName, benchmarkName )
			if submitToGraphite:
				gReferenceValues = _getGraphiteEvents( stat_path +
					[GRAPHITE_STAT_REFVALUE], GRAPHITE_TEN_YEARS_AGO, None )
				if len(gReferenceValues) < 1:
					unsetGraphiteReferences.append( benchmarkName )
			if referenceValue is None:
				unsetReferences.append( benchmarkName )
		if len(unsetReferences) > 0 and len(unsetGraphiteReferences) > 0:
			print
			print "-----"
			print "Reference values not set for:", ", ".join( unsetReferences )
			print "BEGIN reference calculation."
			
			_calculateReferenceValue( buildConfig, exeType, flags, branchName,
				exePath, resPath, OO_STANDARD_TEST, scriptIndex, dbSession,
				submitToGraphite,
				".".join( stat_path + [GRAPHITE_STAT_REFVALUE] ), benchmarkName )
					
			print "END reference calculation."
			print "-----"
			print
		elif len(unsetReferences) > 0 and len(unsetGraphiteReferences) < 1:
			ReferenceValue.set( dbSession, buildConfig, exeType, branchName,
				benchmarkName, gReferenceValues[0] )
		elif len(unsetReferences) < 1 and len(unsetGraphiteReferences) > 0:
			if submitToGraphite:
				_sendGraphiteRefValue( stat_path, branchName, buildConfig,
					benchmarkName, referenceValue.value )
	
		# Run actual test.
		open_automate.runScript( exePath, flags, resPath, OO_STANDARD_TEST, scriptIndex )
		scriptIndex += 1
	
	if util.replaceLineInFile( xmlPath, xmlPath, 
			"<maxFrameRate>		0	</maxFrameRate>", 
			"<maxFrameRate>		75	</maxFrameRate>" ):
		print "Replace <maxFrameRate>		0	</maxFrameRate> with <maxFrameRate>		75	</maxFrameRate>"
	# Process results
	reportHolder.addReport( _processResults( buildConfig, exeType, branchName,
		dbSession, submitToGraphite, benchmarkAllNames, changelist ) )


def run( buildConfig, exeType, dbSession, reportHolder, branchTag, testName,
		compiled_space, changelist, primer_run, submitToGraphite, flags):
	global OO_STANDARD_TEST
	if ( testName != "" ):
		OO_STANDARD_TEST = testName
		
	exePath = util.resolveClientExecutable( buildConfig, exeType )
	resPath = os.path.normpath( os.path.join( util.packageRoot(), GAME_RESOURCE_PATH ) )

	print "test type: framerate"
	print "build configuration:", buildConfig
	print "executable type:", exeType
	print "executable path:", exePath
	print "resource path:", resPath
	
	if branchTag:
		print "branch tag:", branchTag
	
	bwversion = util.bigworldVersion()
	
	branchName = "bw_%(versionMajor)d_%(versionMinor)d_full"
	if compiled_space:
		branchName = "bw_%(versionMajor)d_%(versionMinor)d_compiled_space"
	branchName = branchName % \
		{
			"versionMajor":bwversion[0],
			"versionMinor":bwversion[1],
			"versionPatch":bwversion[2]
		}
		
	if branchTag:
		branchName += "_" + branchTag
		
	print
	print "Testing branch: %s" % branchName
	_runInternal( branchName,
			flags,
			buildConfig,
			exeType,
			exePath, 
			resPath, 
			True,
			compiled_space,
			reportHolder, 
			dbSession, changelist, primer_run, submitToGraphite )
