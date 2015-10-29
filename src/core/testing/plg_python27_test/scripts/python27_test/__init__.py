# Send print to logger
import ScriptOutputWriter
# Access C++ module from Python
import reflection

'''
Type testing
https://docs.python.org/2/library/types.html

Types not tested yet:
types.CodeType
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

class DescriptorTest( object ):
	def __init__( self, value ):
		self.value = value

	def __get__( self, obj, objtype ):
		return self.value

	def __set__( self, obj, value ):
		self.value = value

def firstn(n):
	'''Generator test'''
	num = 0
	while num < n:
		yield num
		num += 1

class OldClassTest:
	def __init__( self ):
		self.noneTest = None
		self.typeTest1 = type( OldClassTest )
		self.typeTest2 = type( self.typeTest1 )
		self.classTest1 = OldClassTest
		self.classTest2 = self.__class__
		self.instanceTest = type( self )
		self.boolTest = True
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

	@classmethod
	def classMethodTest( cls, testString ):
		return "Class method test " + testString
	
	@staticmethod
	def staticMethodTest( testString ):
		return "Static method test " + testString

class NewClassTest( object ):
	def __init__( self ):
		self.noneTest = None
		self.typeTest1 = type( NewClassTest )
		self.typeTest2 = type( self.typeTest1 )
		self.classTest1 = NewClassTest
		self.classTest2 = self.__class__
		self.instancetest = type( self )
		self.boolTest = True
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
		self.descriptorTest = DescriptorTest( "Descriptor property" )

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

	@classmethod
	def classMethodTest( cls, testString ):
		return "Class method test " + testString
	
	@staticmethod
	def staticMethodTest( testString ):
		return "Static method test " + testString

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

