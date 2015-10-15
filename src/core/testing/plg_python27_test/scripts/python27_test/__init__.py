# Send print to logger
import ScriptOutputWriter
# Access C++ module from Python
import reflection

'''
Type testing
https://docs.python.org/2/library/types.html

Types not tested yet:
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

class CallableClassTest:
	def __call__( self, value ):
		return "Callable class test " + value

def firstn(n):
	'''Generator test'''
	num = 0
	while num < n:
		yield num
		num += 1

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
		self.functionTest1 = \
			lambda testString: "Function test " + testString
		self.functionTest2 = CallableClassTest()
		#self.generatorTest = firstn

	def methodTest( self, testString ):
		return "Method test " + testString

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
		self.functionTest1 = \
			lambda testString: "Function test " + testString
		self.functionTest2 = CallableClassTest()
		#self.generatorTest = firstn
		self.propertyTest1_ = "Read-only Property"
		self.propertyTest2_ = "Read-only Property"

	def methodTest( self, testString ):
		return "Method test " + testString

	def getReadOnlyPropertyTest1( self ):
		'''Only works for new-style classes'''
		return self.propertyTest1_
	readOnlyPropertyTest1 = property( getReadOnlyPropertyTest1 )

	@property
	def readOnlyPropertyTest2( self ):
		'''Only works for new-style classes'''
		return self.propertyTest2_


class RevealAccess(object):
	"""Descriptor test.
	   A data descriptor that sets and returns values
	   normally and prints a message logging their access.
	"""

	def __init__(self, initval=None, name='var'):
		self.val = initval
		self.name = name

	def __get__(self, obj, objtype):
		print 'Retrieving', self.name
		return self.val

	def __set__(self, obj, val):
		print 'Updating', self.name
		self.val = val

def run():
	print "~~ Begin test"

	print "~~ Python to C++"
	oldClassTest = OldClassTest()
	reflection.oldStyleConversionTest( oldClassTest )
	print "~~ Passed"

	newClassTest = NewClassTest()
	reflection.newStyleConversionTest( object=newClassTest )

	print "~~ C++ to Python"
	scenario = reflection.create( "class Scenario" )
	print "Scenario", scenario

	# TODO
	#print scenario.name
	print "~~ Passed"

	print "~~ End test"

