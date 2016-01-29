#########################################################################################

######### This Test shoud be run from the same location as profile_analyzer.exe #########

#########################################################################################

import optparse
import os
import subprocess
import sys
import time
import threading
import shutil
sys.path.append(os.path.join(os.path.dirname(os.path.realpath(__file__)), ".."))
import stat
from constants import *

try:
	import database
	from database import WotPerformanceTestResult
except:
	pass

TIMEOUT = 600


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

		thread.join(timeout)
		if thread.is_alive():
			subprocess.check_output("taskkill /im " + program + " /f")
			thread.join()
			end_time = time.time()
			timeToRun = end_time - start_time
			self.subprocess_output =  "TIMEOUT ERROR. Total Time: " + str(timeToRun)
			self.returncode = 1
			
		return self.subprocess_output
	

def addToDB( dbType, changelist, executable, replayName, peakAllocatedMB, loadTimeSec, frameTimeMs, fps, successState, branchName ):
	print "---------- Summary ----------"
	print "Average frame time " + str(frameTimeMs) + " ms"
	print "Average fps " + str(fps)
	print "Average memory peak " + str(peakAllocatedMB) + " MB"
	print "Average load time " + str(loadTimeSec) + " sec"
	print "-----------------------------"
	
	if dbType == DB_PRODUCTION:
		dbSession = database.createSession( dbType )
		# Add to MYSQL server
		WotPerformanceTestResult.addResult( dbSession, changelist, executable, replayName, peakAllocatedMB, 0, loadTimeSec, frameTimeMs, fps, successState, branchName )
		
def analyzeResults(uplFile, outputFileName, numberRun, exeType, replayName, changelist, database_type, branchName):

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
	
	def analyzeBl(l):
		#loadTime 
		avg = 0
		if l.startswith("beginSpaceLoad"):
			avg -= ((float((l.split(":")[-1]).split("ms")[0])) / 1000 ) / numberRun
		elif l.startswith("endSpaceLoad"):
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
		
	res = 0
	
	os.environ['UNIPROF_ENABLE'] = 't'
	os.environ['UNIPROF_AUTOSTART'] = 't'
	# cmd = "..\\game64\\profile_analyzer.exe -c=..\\game64\\analyzer_config.json"
	cmd = "profile_analyzer.exe -c=analyzer_config.json"
	bfOut = outputFileName + ".bf.txt"
	print cmd + " -bf " + uplFile + " > " + bfOut
	command = Command(cmd + " -bf " + uplFile + " > " + bfOut )
	print command.run(timeout=TIMEOUT, program="profile_analyzer.exe" )
	res += command.returncode
	
	bmOut = outputFileName + ".bm.txt"
	command = Command(cmd + " -bm " + uplFile + " > " + bmOut )
	print command.run(timeout=TIMEOUT, program="profile_analyzer.exe" )
	res += command.returncode
	
	# blOut = outputFileName + ".bl.txt"
	# command = Command(cmd + " -bl " + uplFile + " > " + blOut )
	# print command.run(timeout=TIMEOUT, program="profile_analyzer.exe" )
	# res += command.returncode
	
	if res == 0:
		status = OK_STATE
	else:
		status = FAIL_STAT
		
	addToDB( database_type, changelist, exeType, replayName, analyzeFile(bmOut, analyzeBm), \
			0, analyzeFile(bfOut, analyzeBf), analyzeFile(bfOut, analyzeBfps), status, branchName )

	
	return res

def analyzeUpl():

	usage = "usage: %prog --upl <upl_name> [options]"
	
	parser = optparse.OptionParser( usage )
	
	parser.add_option( "--upl",
					   dest = "upl", default=None,
					   help = "upl name\n"  )
					   
	parser.add_option( "--number",
					   dest = "uplNumber", type="int", default=1,
					   help = "number of upl files\n"  )
					   
	parser.add_option( "-d", "--database",
					dest = "database_type", default=DB_MEMORY,
					help = "Database engine: " + "|".join( DB_TYPES ) \
							+ " (default="+DB_MEMORY+")" )
							
	parser.add_option( "-o", "--output",
					   dest = "output", default="",
					   help = "Output directory" )
					   
	parser.add_option( "-e", "--exeType",
					   dest = "exeType", default=EXE_RELEASE,
					   help = "Select client exe type  " + ("|").join(EXE_TYPES))	
	
	parser.add_option( "--changelist",
					   dest = "changelist", default=0,
					   help = "Specify perforce changelist number")
					   
	parser.add_option( "-b", "--branch_name",
					   dest = "branchName", default="Internal",
					   help = "Specify the branch name")
			
	(options, args) = parser.parse_args()
	

	if options.upl == None:
		print "upl name is missing"
		return 1
	
	
	NSETVar = ""

	for i in range(1,options.uplNumber+1):
		logName = options.upl + "_%s.upl" % (i)
		NSETVar = NSETVar + " " + logName

	
	# analyzeResults(uplFile, outputFileName, numberRun, exeType, replayName, changelist, database_type, branchName):
	res = analyzeResults( NSETVar, os.path.join(options.output , options.upl), \
					options.uplNumber, options.exeType, options.upl , \
					options.changelist, options.database_type, options.branchName  )		


	return res

if __name__ == "__main__":
	sys.exit( analyzeUpl() )
