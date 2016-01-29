######################################################################################

############# This file should be in compiled_spaces\testing\wot\ ######################

######################################################################################

import optparse
import os
import subprocess
import sys
import time
import threading
import shutil
sys.path.append(os.path.join(os.path.dirname(os.path.realpath(__file__)), ".."))
import util
import stat
from constants import *

import database
from database import WotPerformanceTestResult

TIMEOUT = 0
PROFILE_ANALYZER_TIMEOUT = 600

FILE_NAME = "Batch"

def initPathGlobals(root, new_directory_structure):
	global PACKAGE_ROOT, GAME_RESOURCE_PATH, GAME_PATH, PREFERENCES_FILE
	PACKAGE_ROOT = root
	if new_directory_structure:
		GAME_RESOURCE_PATH= os.path.join( PACKAGE_ROOT, "game", "res", "wot" )
		GAME_PATH = os.path.join( PACKAGE_ROOT, "game", "bin", "client", "win32" )
	else:
		GAME_RESOURCE_PATH= os.path.join( PACKAGE_ROOT, "game", "tankfield", "res" )
		GAME_PATH = os.path.join( PACKAGE_ROOT, "game", "tankfield", "game" )
		
	PREFERENCES_FILE = os.path.join( GAME_PATH, "preferences.xml" )

exeMapping = {EXE_DEBUG : 'WoT_d.exe', 
		EXE_RELEASE : 'WorldOfTanks.exe',
		EXE_CONSUMER : 'WorldOfTanks_consumer.exe'};

#run program with timeout
class Command(object):
	def __init__(self, cmd):
		self.cmd = cmd
		self.process = ""#None
		
	def run(self, timeout, program):
		def target():
			self.returncode = 0
			try:
				print self.cmd
				self.subprocess_output = subprocess.check_output( self.cmd, 
										stderr=subprocess.STDOUT, shell=True )										
			except subprocess.CalledProcessError, e:
				self.subprocess_output = e.output
				self.returncode = e.returncode

		start_time = time.time()
		thread = threading.Thread(target=target)
		thread.start()
		
		if not timeout == 0 :
			thread.join(timeout)
		else:
			thread.join()
			
		if thread.is_alive():
			subprocess.check_output("taskkill /im " + program + " /f")
			thread.join()
			end_time = time.time()
			timeToRun = end_time - start_time
			self.subprocess_output =  "TIMEOUT ERROR. Total Time: " + str(timeToRun)
			self.returncode = 1
			
		return self.subprocess_output
	
def setEnvVariable():
	os.environ['UNIPROF_ENABLE'] = 't'
	os.environ['UNIPROF_AUTOSTART'] = 't'
	
	engineXMLPath = util.engineConfigXML( GAME_RESOURCE_PATH )
	if util.replaceLineInFile( engineXMLPath, engineXMLPath, 
		"<enableEndStopApp>false</enableEndStopApp>", 
		"<enableEndStopApp>true</enableEndStopApp>\n\t\t<skipReplayDialogs>true</skipReplayDialogs>" ):
		print "Replace <enableEndStopApp>false</enableEndStopApp> with <enableEndStopApp>true</enableEndStopApp>\n<skipReplayDialogs>true</skipReplayDialogs>"
	
def revertEnvVariable():
	engineXMLPath = util.engineConfigXML( GAME_RESOURCE_PATH )
	if util.replaceLineInFile( engineXMLPath, engineXMLPath, 
		"<enableEndStopApp>true</enableEndStopApp>",
		"<enableEndStopApp>false</enableEndStopApp>" ):
		print "Replace <enableEndStopApp>true</enableEndStopApp> with <enableEndStopApp>false</enableEndStopApp>"
		
	if util.replaceLineInFile( engineXMLPath, engineXMLPath, 
		"<skipReplayDialogs>true</skipReplayDialogs>",
		None ):
		
		print "Remove <skipReplayDialogs>true</skipReplayDialogs>"
		


def runTest( exe, replay , flags, replayTimeout):
	command = Command(exe + " " + replayTimeout + " " + replay + " -noConversion " + flags)
	print command.run( timeout=TIMEOUT, program=exe )
	# if output.strip() != "":
		# print output
		# return 1
		
	return command.returncode

	
def addToDB( dbType, changelist, executable, replayName, peakAllocatedMB, totalAllocatedMB, loadTimeSec, frameTimeMs, fps, successState, branchName ):
	print "---------- Summary ----------"
	print "Average frame time " + str(frameTimeMs) + " ms"
	print "Average fps " + str(fps)
	print "Average memory peak " + str(peakAllocatedMB) + " MB"
	print "Total allocations " + str(totalAllocatedMB)
	print "Average load time " + str(loadTimeSec) + " sec"
	print "-----------------------------"
	
	if dbType == DB_PRODUCTION:
		dbSession = database.createSession( dbType )
		# Add to database server
		WotPerformanceTestResult.addResult( dbSession, changelist, executable, replayName, peakAllocatedMB, totalAllocatedMB, loadTimeSec, frameTimeMs, fps, successState, branchName )
		
def analyzeResults(uplFile, outputFileName, numberRun, exeType, replayName, changelist, database_type, res, branchName, tests ):

	def analyzeBfps(l):	
		# frame time
		avg = 0
		if l.startswith("Total:"):
			avg += float((l.split("(")[1]).split("fps")[0])
		return avg
		
	def analyzeBf(l):	
		# frame time
		avg = 0
		if l.startswith("Total:"):
			avg += float((l.split("=")[1]).split("ms")[0])
		return avg
		
	def analyzeBm(l):	
		# memory peak
		avg = 0
		if l.startswith("Total:"):
			avg = float((l.split("=")[1]).split("MB")[0])
		return avg
		
	def analyzeBmT(l):	
		# memory peak
		avg = 0
		if ".upl: total allocs = " in l:
			avg = int(int((l.split(",")[0]).split("=")[-1]) / numberRun)
		return avg
	
	def analyzeBl(l):
		#loadTime 
		avg = 0
		if l.startswith("Space loading started:") or l.startswith("Space loading started:"):
			avg -= ((float((l.split(":")[-1]).split("ms")[0])) / 1000 ) / numberRun
		elif l.startswith("space loaded:"):
			avg = ((float((l.split(":")[-1]).split("ms")[0])) / 1000 ) / numberRun
		return avg
	
	def analyzeFile(file, analyzeFunc):
		f = open( file, "r" )
		avg = 0
		for l in f:
			avg += analyzeFunc(l)
			print l
		f.close()
		return avg
		
	bfRes = 0
	bfsRes = 0
	bmRes = 0
	bmTRes = 0
	blRes = 0
	
	if res == 0:
		cmd = "..\\game64\\profile_analyzer.exe -c=..\\game64\\analyzer_config.json"
		
		if "bf" in tests:
			bfOut = outputFileName + ".bf.txt"
			command = Command(cmd + " -bf " + uplFile + " > " + bfOut )
			print command.run(timeout=PROFILE_ANALYZER_TIMEOUT, program="profile_analyzer.exe" )
			res += command.returncode
			bfRes = analyzeFile(bfOut, analyzeBf)
			bfsRes = analyzeFile(bfOut, analyzeBfps)
		
		if "bm" in tests:		
			bmOut = outputFileName + ".bm.txt"
			command = Command(cmd + " -bm " + uplFile + " > " + bmOut )
			print command.run(timeout=PROFILE_ANALYZER_TIMEOUT, program="profile_analyzer.exe" )
			res += command.returncode
			bmRes = analyzeFile(bmOut, analyzeBm)
			bmTRes = analyzeFile(bmOut, analyzeBmT)
		
		if "bl" in tests:		
			blOut = outputFileName + ".bl.txt"
			command = Command(cmd + " -bl " + uplFile + " > " + blOut )
			print command.run(timeout=PROFILE_ANALYZER_TIMEOUT, program="profile_analyzer.exe" )
			res += command.returncode
			blRes = analyzeFile(blOut, analyzeBl)
		
	if res == 0:
		status = OK_STATE
	else:
		status = FAIL_STAT
			
	addToDB( database_type, changelist, exeType, replayName, bmRes, bmTRes,\
			blRes , bfRes , bfsRes , status, branchName )
	
	return res

def changePreferencesFile( newFile ):
	if os.path.exists(newFile):
		if os.path.exists(PREFERENCES_FILE):
			os.chmod( PREFERENCES_FILE, stat.S_IWRITE )

			if os.path.exists(PREFERENCES_FILE + "_bk"):
				os.chmod( PREFERENCES_FILE + "_bk", stat.S_IWRITE )
				os.unlink(PREFERENCES_FILE + "_bk")
				
			shutil.copy (PREFERENCES_FILE, PREFERENCES_FILE + "_bk")
			os.unlink(PREFERENCES_FILE)
		shutil.copy (newFile, PREFERENCES_FILE)
		print "Replace " + PREFERENCES_FILE + " with " + newFile
	else:
		print "Error: can't find " + newFile + " using old settings."


def changeBackPreferencesFile():
	if os.path.exists(PREFERENCES_FILE + "_bk"):
		os.chmod( PREFERENCES_FILE, stat.S_IWRITE )
		os.unlink(PREFERENCES_FILE)
		shutil.copy (PREFERENCES_FILE + "_bk", PREFERENCES_FILE )
		os.unlink(PREFERENCES_FILE + "_bk")
	
def savePreferencesFile(output, name):
	destFile = os.path.join(output, name)
	if os.path.exists(destFile):
		os.chmod( destFile, stat.S_IWRITE )
		os.unlink(destFile)
	shutil.copy(PREFERENCES_FILE , destFile )
	

def performanceTests():

	usage = "usage: %prog -r [replay_path] --output [output_directory] [options]"
	
	parser = optparse.OptionParser( usage )
	
	parser.add_option( "-r", "--replay", type="string",
					   dest = "replay", default=None,
					   help = "Replay path\n"  )

	parser.add_option( "-o", "--output", type="string",
					   dest = "output", default=None,
					   help = "Output directory" )
					   
	parser.add_option( "-p", "--package-root", type="string",
					   dest = "root", default=None,
					   help = "The package root directory" )

	parser.add_option( "-n", "--number", type="int",
					   dest = "numberRun", default="1",
					   help = "Number of times to run the test" )
					   
	parser.add_option( "-e", "--exeType", type="string",
					   dest = "exeType", default=EXE_RELEASE,
					   help = "Select client exe type  " + ("|").join(EXE_TYPES))
					   
	parser.add_option( "--changelist", type="int",
					   dest = "changelist", default=0,
					   help = "Specify perforce changelist number")
					   
	parser.add_option( "-b", "--branch_name", type="string",
					   dest = "branchName", default="Internal",
					   help = "Specify the branch name")
					   
	parser.add_option( "-d", "--database", type="string",
					dest = "database_type", default=DB_MEMORY,
					help = "Database engine: " + "|".join( DB_TYPES ) \
							+ " (default="+DB_MEMORY+")" )
							
	parser.add_option( "--preferences", type="string",
					dest = "preferences_file", default=None,
					help = "Specify the preferences.xml to use" )
					
	parser.add_option( "--savePreferences",
					action = "store_true",
					dest = "print_preferences", default=False,
					help = "copy preferences.xml to the output directory" )

	parser.add_option( "--executable",
						dest = "executable", default=None,
						help = "Specify executable name."  )

	parser.add_option( "-f", "--flags", type="string",
					dest = "flags", default="",
					help = "add a list of flags to the client command line, " \
							+ "separated by semicolon -flag;value;--flag2;flag3;value" )
							
	parser.add_option( "--compiled_space",
						action = "store_true",
						dest = "compiled_space", default=False,
						help = "Change space type from chunk space to compiled space" )
						
	parser.add_option( "--no_analyze",
						action = "store_true",
						dest = "no_analyze", default=False,
						help = "Don't run profile_analyze.exe" )
						
	parser.add_option( "-t", "--replayTimeout", type="int",
						dest = "replayTimeout", default=0,
						help = "Add replay timeout, default none" )
						
	parser.add_option( "--timeout", type="int",
						dest = "programTimeout", default=0,
						help = "Add a timeout for the script" )
						
	parser.add_option( "--tests", type="string",
						dest = "tests", default="bf;bl;bm",
						help = "bf for frame rate, bl for load time bm for memory\n" \
								"each separate by semicolon default-bf;bl;bm" )
								
	parser.add_option( "--new_directory_structure",
						action = "store_true",
						dest = "new_directory_structure", default=False,
						help = "Use the new directory structure" )
			
	(options, args) = parser.parse_args()
	global TIMEOUT
	TIMEOUT = options.programTimeout
	
	#Change working directory to game\tankfield\game
	if options.root == None:
		initPathGlobals(os.path.join(os.path.dirname(os.path.realpath(__file__)), "..", ".."), options.new_directory_structure)
	else:
		initPathGlobals(options.root, options.new_directory_structure)
	
	os.chdir(GAME_PATH)
	
	flags = " ".join( options.flags.split(";") )

	if not options.tests == "bf;bl;bm":
		tests = options.tests.split(";")
		for test in tests:
			if not (test in ["bf","bl","bm"]):
				print "\nUnknown test - %s \n" % test
				parser.print_help()
				return 1
	
	if options.replay == None:
		print "\nPlease specify replay path\n"
		parser.print_help()
		return 1
		
	if options.database_type not in DB_TYPES :
		print "\nPlease specify a database from the following list: " + ("|").join(DB_MEMORY) + "\n"
		parser.print_help()
		return 1

	if not options.exeType in EXE_TYPES:
		print "\nPlease specify exe type from the following list: " + ("|").join(EXE_TYPES) + "\n"
		parser.print_help()
		return 1
		
	if options.output == None:
		print "\nPlease specify the output folder --output [output_directory] \n"
		parser.print_help()
		return 1
	elif not os.path.exists(options.output):
		os.mkdir(options.output)  
			
	if options.preferences_file != None:
		changePreferencesFile(options.preferences_file)
	
	branchName = options.branchName
	if options.compiled_space:
		branchName = branchName + "_compiled_space"
		flags += " -spaceType COMPILED_SPACE"
	LOG_FILE_NAME = FILE_NAME + "_" + branchName	
	
	if options.replayTimeout != 0:
		replayTimeout = "-sa replayTimeout -sa " + str(options.replayTimeout)
	else:
		replayTimeout = ""
	setEnvVariable()
	
	
	NSETVar = ""
	
	for i in range(1,options.numberRun + 1):
		print "\nrunning " + str(i) + "/" + str(options.numberRun)
		logName = os.path.join(options.output, LOG_FILE_NAME + "_%s.upl" % (i) )
		os.environ['UNIPROF_LOGNAME'] = logName
		NSETVar = NSETVar + " " + logName
		os.environ['NSET'] = NSETVar
		if options.executable != None:
			res = runTest( options.executable, options.replay, flags, replayTimeout )
		else:
			res = runTest( exeMapping[options.exeType], options.replay, flags, replayTimeout )
		if res:
			break

	replayFileName = os.path.splitext(os.path.basename(options.replay))[0]
	
	if not (options.no_analyze):
		res += analyzeResults( NSETVar, os.path.join(options.output, LOG_FILE_NAME), \
					int(options.numberRun), options.exeType, replayFileName , \
					options.changelist, options.database_type, res, branchName, \
					options.tests )		

	revertEnvVariable()
	
	if options.print_preferences:
		savePreferencesFile(options.output, "preferences_" + branchName + ".xml")
		
	if options.preferences_file != None:
		changeBackPreferencesFile()
	
	return res

if __name__ == "__main__":
	sys.exit( performanceTests() )
