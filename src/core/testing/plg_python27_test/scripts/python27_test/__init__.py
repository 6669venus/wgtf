# Send print to logger
import ScriptOutputWriter
# Access C++ module from Python
import reflection

'''
Type testing
https://docs.python.org/2/library/types.html

Types not tested yet:
types.FunctionType
types.LambdaType
types.GeneratorType
types.CodeType
types.ClassType
types.InstanceType
types.MethodType
types.UnboundMethodType
types.BuiltinFunctionType
types.BuiltinMethodType
types.ModuleType
types.FileType
types.XRangeType
types.SliceType
types.EllipsisType
types.TracebackType
types.FrameType
types.BufferType
types.DictProxyType
types.NotImplementedType
types.GetSetDescriptorType
types.MemberDescriptorType
types.StringTypes
'''
class OldClassTest:
	def __init__( self ):
		#self.noneTest = None
		#self.typeTest = type( NewClassTest )
		#self.boolTest = True
		self.intTest = 1
		self.longTest = 1L
		self.floatTest = 1.0
		#self.complexTest = 1.0j
		self.stringTest = "Spam"
		self.unicodeTest = u"Spam"
		#self.tupleTest = (1, 2, 3, "Spam")
		#self.listTest = [0, 1, 2, 3]
		#self.dictTest = {'Bacon': 1, 'Ham': 0}

	def methodTest( self ):
		return "Method test was run"

class NewClassTest( object ):
	def __init__( self ):
		#self.noneTest = None
		#self.typeTest = type( NewClassTest )
		#self.boolTest = True
		self.intTest = 1
		self.longTest = 1L
		self.floatTest = 1.0
		#self.complexTest = 1.0j
		self.stringTest = "Spam"
		self.unicodeTest = u"Spam"
		#self.tupleTest = (1, 2, 3, "Spam")
		#self.listTest = [0, 1, 2, 3]
		#self.dictTest = {'Bacon': 1, 'Ham': 0}

	def methodTest( self ):
		return "Method test was run"

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

