import scriptoutputwriter

class OldClass:
	'''Test of old-style classes'''

	'''
	Properties exposed to GUI.
	In the format "attribute name" : "meta data name"
	'''
	_metaData = {
		"boolean" : "MetaNone",
		"integer" : "MetaNone",
		"floatslider" : "MetaSlider",
		"longinteger" : "MetaNone",
		"asciistring" : "MetaNone",
		"updateValues" : "MetaNone",
	}

	def __setattr__( self, name, value ):
		'''
		Hook for notifying the GUI
		'''
		self.__dict__[ name ] = value
		# Notify GUI

	def __init__( self ):
		self.boolean = False
		self.integer = 0
		self.floatslider = 1.0
		self.longinteger = 0L
		self.asciistring = ""
		self.updateValues()

	def updateValues( self ):
		self.boolean = not self.boolean
		self.integer = self.integer + 56
		self.floatslider = self.floatslider + 1.0
		self.longinteger = self.integer * 123456
		self.asciistring = "ascii " + repr( self.integer )

class NewClass( object ):
	'''Test of new-style classes'''

	'''
	Properties exposed to GUI.
	In the format "attribute name" : "meta data name"
	'''
	_metaData = {
		"boolean" : "MetaNone",
		"integer" : "MetaNone",
		"floatslider" : "MetaSlider",
		"longinteger" : "MetaNone",
		"asciistring" : "MetaNone",
		"updateValues" : "MetaNone",
	}

	def __setattr__( self, name, value ):
		'''
		Hook for notifying the GUI
		'''
		self.__dict__[ name ] = value
		# Notify GUI

	def __init__( self ):
		self.boolean = False
		self.integer = 0
		self.floatslider = 1.0
		self.longinteger = 0L
		self.asciistring = ""
		self.updateValues()

	def updateValues( self ):
		self.boolean = not self.boolean
		self.floatslider = self.floatslider + 1.0
		self.integer = self.integer + 56
		self.longinteger = self.integer * 123456
		self.asciistring = "ascii " + repr( self.integer )

oldStyleObject = OldClass()
newStyleObject = NewClass()
