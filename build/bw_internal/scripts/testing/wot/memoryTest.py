################################################################################

###################  This should run from /game/bin/client/win32  ##############

################################################################################

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

try:
	import database
	from database import WotPerformanceTestResult
except:
	pass

TIMEOUT = 0
CSV_TIMEOUT = 600

FILE_NAME = "Batch"

GAME_PATH = os.getcwd()
CAT_DIR = os.path.join(GAME_PATH, "..", "tools" , "cat")
OUT_PATH = os.path.join(CAT_DIR, "out")
PREFERENCES_FILE = os.path.join( GAME_PATH, "preferences.xml" )

EXE_HYBRID = "WorldOfTanks.exe"

def initPathGlobals(catDir):
	global OUT_PATH
	OUT_PATH = os.path.join(catDir, "out")
		
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
	
def runTest( exe, quick, flags):
	test = "offlineTest"
	if quick:
		test = "offlineQuickTest"
	command = Command( exe + " -sa " + test + " -sa cat/all_maps.xml " + flags )
	print command.run( timeout = TIMEOUT, program = exe )		
	return command.returncode
		
def analyzeResults( exeType, changelist, database_type, res, branchName ):
	maps = []
	mems = []
	tpfs = []

	result_path = ""
	for dir in os.listdir(OUT_PATH):

		if not os.path.isdir(os.path.join(OUT_PATH, dir)):
			continue

		result_path = os.path.join(OUT_PATH, dir)
		mem_result_path = os.path.join(result_path, "vmemory", "overall.csv")
		tpf_result_path = os.path.join(result_path, "tpf", "overall.csv")
		
		i = 0 
		while not os.path.exists(mem_result_path):
			time.sleep(5)
			i = i + 1
			print "waiting for WOT to generate " + mem_result_path
			if i > CSV_TIMEOUT:
				print "TIMEOUT Error: can't find " + mem_result_path
				return 1

		i = 0 
		while not os.path.exists(tpf_result_path):
			time.sleep(5)
			i = i + 1
			print "waiting for WOT to generate " + tpf_result_path
			if i > CSV_TIMEOUT:
				print "TIMEOUT Error: can't find " + tpf_result_path
				return 1

		f = open( mem_result_path, "r" )
		maps.extend(f.readline().split()[1:])
		mems.extend(f.readline().split()[2:])
		f.close()

		f = open( tpf_result_path, "r" )
		f.readline() # skip maps
		tpfs.extend(f.readline().split()[2:])
		f.close()

	if result_path == "":
		print "Error: no results have been saved to %s\n" %OUT_PATH
		return 1
	
	num = len(maps)
	if num == 0:
		return 1

	tpfsByMap =  dict((map, []) for map in maps)
	memsByMap =  dict((map, []) for map in maps)
	for i in range( num ):
		try:
			tpfsByMap[maps[i]].extend( [float( tpfs[i].replace(',', '.') )] )
		except ValueError:
			pass
		try:
			memsByMap[maps[i]].extend( [int( mems[i].split(',')[0] )] )
		except ValueError:
			pass

	print "---------- Summary ----------"
	for map in tpfsByMap:
		
		# calculate tpf average and average deviation
		tpfAvg = -1.0
		tpfAvgDev = 0.0
		if tpfsByMap[map]:
			tpfAvg = sum( tpfsByMap[map] ) / len( tpfsByMap[map] ) 
			for tpf in tpfsByMap[map]:
				tpfAvgDev = tpfAvgDev + abs(tpf - tpfAvg)
			tpfAvgDev = tpfAvgDev / len( tpfsByMap[map] )

		# calculate mem average and average deviation
		memAvg = -1.0
		memAvgDev = 0.0
		if memsByMap[map]:
			memAvg = sum( memsByMap[map] ) / len( memsByMap[map] )
			for mem in memsByMap[map]:
				memAvgDev = memAvgDev + abs(mem - memAvg)
			memAvgDev = memAvgDev / len( memsByMap[map] )

		print "%s, %.3f, %.3f, %.2f%%, %.3f, %.3f, %.2f%%" % ( map, \
			tpfAvg, tpfAvgDev, tpfAvgDev / tpfAvg * 100.0, \
			memAvg, memAvgDev, memAvgDev / memAvg * 100.0 )

		if database_type == DB_PRODUCTION:
			dbSession = database.createSession( database_type )
			# Add to MYSQL server
			WotPerformanceTestResult.addResult( dbSession, changelist, exeType, map, 0, int( memAvg ), 0, float( tpfAvg ), 0, OK_STATE, branchName + "_" + map )
	print "-----------------------------"	
	
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
	

def memoryTest():

	usage = "usage: %prog [options]"
	
	parser = optparse.OptionParser( usage )
					   					   
	parser.add_option( "--changelist",
					   dest = "changelist", default=0,
					   help = "Specify perforce changelist number")
					   
	parser.add_option( "-b", "--branch_name",
					   dest = "branchName", default="Internal",
					   help = "Specify the branch name")
					   
	parser.add_option( "-d", "--database",
					dest = "database_type", default=DB_MEMORY,
					help = "Database engine: " + "|".join( DB_TYPES ) \
							+ " (default="+DB_MEMORY+")" )
							
	parser.add_option( "--preferences",
					dest = "preferences_file", default=None,
					help = "Specify the preferences.xml to use" )
					
	parser.add_option( "--savePreferences",
					action = "store_true",
					dest = "print_preferences", default=False,
					help = "copy preferences.xml to the output directory" )
							
	parser.add_option( "-f", "--flags",
					dest = "flags", default="",
					help = "add a list of flags to the client command line, " \
							+ "separated by semicolon -flag;value;--flag2;flag3;value" )
							
	parser.add_option( "--compiled_space",
						action = "store_true",
						dest = "compiled_space", default=False,
						help = "Change space type from chunk space to compiled space" )
																		
	parser.add_option( "--timeout",
						dest = "programTimeout", default=0,
						help = "Add a timeout for the script" )
						
	parser.add_option( "--cat_directory",
						dest = "cat_directory", default=CAT_DIR,
						help = "Specify the cat directory. default %s" % CAT_DIR)
						
	parser.add_option( "--new_directory_structure",
						action = "store_true",
						dest = "new_directory_structure", default=False,
						help = "Use the new directory structure" )
						
	parser.add_option( "--executable",
						dest = "executable", default=EXE_HYBRID,
						help = "Specify executable name. default: %s" % EXE_HYBRID  )

	parser.add_option( "--runs",
						dest = "passes", default = 1,
						help = "Specify the number of test runs" )

	parser.add_option( "--quick",
						action = "store_true",					
						dest = "quick", default = False,
						help = "Run the accelerated test" )
																	
	(options, args) = parser.parse_args()
	global TIMEOUT
	TIMEOUT = options.programTimeout
	res = 0

	#Change the path to the new directory structure
	if options.new_directory_structure:
		initPathGlobals(os.path.join(GAME_PATH, "..", "..", "tools", "cat"))
	else:
		initPathGlobals(options.cat_directory)
		
	if not os.path.exists(options.executable) :
		print "\nCant find %s, please make sure this file exists in the working directory.\n" %options.executable
		parser.print_help()
		return 1
		
	flags = " ".join( options.flags.split(";") )
		
	if options.database_type not in DB_TYPES :
		print "\nPlease specify a database from the following list: " + ("|").join(DB_TYPES) + "\n"
		parser.print_help()
		return 1

	
	branchName = options.branchName
	if options.compiled_space:
		branchName = branchName + "_compiled_space"
		flags += " -spaceType COMPILED_SPACE"

	if os.path.exists(OUT_PATH):
		print "Clearing " + OUT_PATH + " ..."
		for file in os.listdir(OUT_PATH):
			file_path = os.path.join(OUT_PATH, file)
			try:
				if os.path.isdir(file_path):
					shutil.rmtree(file_path)
				else:
					os.unlink(file_path)
			except Exception, e:
				print "Error clearing %s!" % OUT_PATH
				return 1
	else:
		os.mkdir(OUT_PATH)
	
	if options.preferences_file != None:
		changePreferencesFile(options.preferences_file)
		
	res = 0
	for npass in range(0, int(options.passes)):
		res = runTest( options.executable, options.quick, flags )
		if res:
			break

		jobRunning = True
		while jobRunning:
			output = subprocess.check_output( 'tasklist /fi "Imagename eq %s"' %options.executable, stderr=subprocess.STDOUT, shell=True )
			if "INFO: No tasks are running which match the specified criteria." in output:
				jobRunning = False
			else:
				print "Waiting for %s..." % options.executable
				time.sleep(10)
	if not res:
		res = analyzeResults( "Hybrid", options.changelist, options.database_type, res, branchName )
	
	if options.preferences_file != None:
		changeBackPreferencesFile
	return res

if __name__ == "__main__":
	sys.exit( memoryTest() )
