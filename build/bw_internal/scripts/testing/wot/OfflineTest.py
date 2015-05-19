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

TIMEOUT = 0

FILE_NAME = "Batch"

GAME_PATH = os.getcwd()
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
	
def runTest( exe, flags):
	command = Command( exe + " -sa offline -sa spaces/hangar_premium_v2 -sa spaces/92_stalingrad " + flags )
	print command.run( timeout = TIMEOUT, program = exe )		
	return command.returncode
		
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
	

def offlineTest():

	usage = "usage: %prog [options]"
	
	parser = optparse.OptionParser( usage )
					   					   
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
						
	parser.add_option( "--executable",
						dest = "executable", default=EXE_HYBRID,
						help = "Specify executable name. default: %s" % EXE_HYBRID  )

	(options, args) = parser.parse_args()
	global TIMEOUT
	TIMEOUT = options.programTimeout
	res = 0

	if not os.path.exists(options.executable) :
		print "\nCant find %s, please make sure this file exists in the working directory.\n" %options.executable
		parser.print_help()
		return 1
		
	flags = " ".join( options.flags.split(";") )
		
	if options.compiled_space:
		flags += " -spaceType COMPILED_SPACE"

	if options.preferences_file != None:
		changePreferencesFile(options.preferences_file)
		
	res = runTest( options.executable, flags )

	jobRunning = True
	while jobRunning:
		output = subprocess.check_output( 'tasklist /fi "Imagename eq %s"' %options.executable, stderr=subprocess.STDOUT, shell=True )
		if "INFO: No tasks are running which match the specified criteria." in output:
			jobRunning = False
		else:
			print "Waiting for %s..." % options.executable
			time.sleep(10)
				
	if options.preferences_file != None:
		changeBackPreferencesFile
		
	return res

if __name__ == "__main__":
	sys.exit( offlineTest() )
