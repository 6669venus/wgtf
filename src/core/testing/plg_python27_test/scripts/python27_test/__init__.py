# Send print to logger
import ScriptOutputWriter
# Access C++ module from Python
import reflection

class OldClassTest:
	def __init__( self ):
		self.name = "Old Python Object"
		self.testNumber = 1

class NewClassTest( object ):
	def __init__( self ):
		self.name = "New Python Object"
		self.testNumber = 2

def run():
	print "~~ Begin test"

	print "~~ Python to C++"
	oldClassTest = OldClassTest()
	reflection.conversionTest( oldClassTest )
	print "~~ Passed"

	newClassTest = NewClassTest()
	reflection.conversionTest( object=newClassTest )

	print "~~ C++ to Python"
	scenario = reflection.create( "class Scenario" )
	print "Scenario", scenario

	# TODO
	#print scenario.name
	print "~~ Passed"

	print "~~ End test"

