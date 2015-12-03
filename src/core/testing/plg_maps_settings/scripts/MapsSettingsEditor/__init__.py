# NGT
import scriptoutputwriter

# MapsSettingsEditor
from ResourceDeserializers import parseMapsSettingsXml, parseSpaceManager, dumpDeserializedData
import Paths

class MapsSettingsLoader:

	DEFAULT_PATH = r"C:\SVN\GT_Pack_29"

	_metaData = {
		"currentWorkingDirectory" : "MetaNone",
		"mapsSettingsXMLData" : "MetaNone",
		"spaceManagerMapsIds" : "MetaNone"
	}

	def __init__( self ):
		self.showProjectScreen( MapsSettingsLoader.DEFAULT_PATH )

	def showProjectScreen( self, projectPath ):
		self.currentWorkingDirectory = projectPath
		self.mapsSettingsXMLData = parseMapsSettingsXml(
			projectPath + Paths.MAPS_SETTINGS_XML )
		self.spaceManagerMapsIds = parseSpaceManager(
			projectPath + Paths.SPACE_MANAGER_PY )

rootPythonObject = MapsSettingsLoader()

