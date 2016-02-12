# Send print to logger
import scriptoutputwriter
# Access C++ module from Python
import reflectiontest

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

class OldCallableClassTest:
	def __call__( self, value ):
		return "Callable class test " + value

class NewCallableClassTest( object ):
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

class ChildObjectTest( object ):
	def __init__( self ):
		self.stringTest = "Child"

class OldClassTest:
	'''Test of old-style classes'''

	'''
	Properties exposed to GUI.
	In the format "attribute name" : "meta data name"
	'''
	_metaData = {
		"floatTest" : "MetaSlider",
	}

	#def __setattr__( self, name, value ):
	#	'''
	#	Hook for notifying the GUI
	#	'''
	#	self.__dict__[ name ] = value
	#	# TODO NGT-1561 notify GUI

	#def __delattr__( self, name ):
	#	'''
	#	Hook for notifying the GUI
	#	'''
	#	del object.name
	#	# TODO NGT-1561 notify GUI

	classIntTest = 1

	def __init__( self ):
		self.noneTest = None
		self.boolTest = True
		self.intTest = 1
		self.longTest = 1L
		self.floatTest = 1.0
		#self.complexTest = 1.0j
		self.stringTest = "Spam"
		self.unicodeTest = u"Spam"
		self.childTest = ChildObjectTest()
		self.tupleTest = (1, 2, 3, "Spam")
		self.listTest = [0, 1, 2, 3]
		self.dictTest = {'Bacon': 1, 'Ham': 0}
		self.functionTest1 = \
			lambda testString: "Function test " + testString
		self.functionTest2 = OldCallableClassTest()
		self.functionTest3 = NewCallableClassTest()
		#self.generatorTest = firstn

		# Old-style classes only
		self.typeTest1 = type( OldClassTest )
		self.typeTest2 = type( self.typeTest1 )
		self.classTest1 = OldClassTest
		self.classTest2 = self.__class__
		self.instanceTest = type( self )

	def methodTest( self, testString ):
		return "Method test " + testString

	@classmethod
	def classMethodTest( cls, testString ):
		return "Class method test " + testString
	
	@staticmethod
	def staticMethodTest( testString ):
		return "Static method test " + testString

class NewClassTest( object ):
	'''Test of new-style classes'''

	'''
	Properties exposed to GUI.
	In the format "attribute name" : "meta data name"
	'''
	_metaData = {
		"floatTest" : "MetaSlider",
		"readOnlyPropertyTest1" : "MetaReadOnly",
		"readOnlyPropertyTest2" : "MetaReadOnly",
	}


	#def __setattr__( self, name, value ):
	#	'''
	#	Hook for notifying the GUI
	#	Note: descriptors will not be caught by this hook.
	#	'''
	#	super( NewClassTest, self ).__setattr__( name, value )
	#	# TODO NGT-1561 notify GUI

	#def __delattr__( self, name ):
	#	'''
	#	Hook for notifying the GUI
	#	Note: descriptors will not be caught by this hook.
	#	'''
	#	del object.name
	#	# TODO NGT-1561 notify GUI

	classIntTest = 1

	def __init__( self ):
		self.noneTest = None
		self.boolTest = True
		self.intTest = 1
		self.longTest = 1L
		self.floatTest = 1.0
		#self.complexTest = 1.0j
		self.stringTest = "Spam"
		self.unicodeTest = u"Spam"
		self.childTest = ChildObjectTest()
		self.tupleTest = (1, 2, 3, "Spam")
		self.listTest = [0, 1, 2, 3]
		self.dictTest = {'Bacon': 1, 'Ham': 0}
		self.functionTest1 = \
			lambda testString: "Function test " + testString
		self.functionTest2 = OldCallableClassTest()
		self.functionTest3 = NewCallableClassTest()
		#self.generatorTest = firstn

		# New-style classes only
		self.typeTest1 = type( NewClassTest )
		self.typeTest2 = type( self.typeTest1 )
		self.classTest1 = NewClassTest
		self.classTest2 = self.__class__
		self.instanceTest = type( self )
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
	reflectiontest.oldStyleConversionTest( oldClassTest )

	newClassTest = NewClassTest()
	reflectiontest.newStyleConversionTest( object=newClassTest )
	print "~~ Passed"

	print "~~ End test"

