import scriptoutputwriter

class OldClass:
	def __init__( self ):
		self.boolean = False
		self.integer = 0
		self.longinteger = 0L
		self.asciistring = ""
		self.updateValues()

	def updateValues( self ):
		self.boolean = not self.boolean
		self.integer = self.integer + 56
		self.longinteger = self.integer * 123456
		self.asciistring = "ascii " + repr( self.integer )

class NewClass( object ):
	def __init__( self ):
		self.boolean = False
		self.integer = 0
		self.longinteger = 0L
		self.asciistring = ""
		self.updateValues()

	def updateValues( self ):
		self.boolean = not self.boolean
		self.integer = self.integer + 56
		self.longinteger = self.integer * 123456
		self.asciistring = "ascii " + repr( self.integer )

oldStyleObject = OldClass()
newStyleObject = NewClass()
