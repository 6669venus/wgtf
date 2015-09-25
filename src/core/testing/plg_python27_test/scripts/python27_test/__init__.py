# Send print to logger
import ScriptOutputWriter
# Access C++ module from Python
import Reflection

def run():
	print "~~ Begin test"

	scenario = Reflection.create( "class Scenario" )
	print "Scenario", scenario

	# TODO
	#print scenario.name

	print "~~ End test"

