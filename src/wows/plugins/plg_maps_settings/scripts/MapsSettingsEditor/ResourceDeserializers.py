# -*- coding: utf-8 -*-
# AOR: GT team
from xml.dom.minidom import parse, xml, Document, Node
import re

import Error
import Paths
from DataStructures import Space, Scenario, Level, Package
import xmltodict
from collections import OrderedDict


def xmlToDict(filePath):
	try:
		with open(filePath, 'r') as f:
			try:
				return xmltodict.parse(f.read())
			except xml.parsers.expat.ExpatError as ee:
				return Error.Error(Error.ERROR_IDS.INVALID_XML, Error.ERROR_TAGS.XML, filePath, ee.args)
	except FileNotFoundError:
		return Error.Error(Error.ERROR_IDS.FILE_NOT_FOUND, Error.ERROR_TAGS.XML, filePath, 'No file {}'.format(filePath))


def dictToXml(filePath, data, useComments=True):
	"""

	:type data: xmltodict.OrderedDictWithInsert
	"""
	oldElement = xml.dom.minidom.Element
	xml.dom.minidom.Element = PatchedElement
	if useComments:
		if len(data) != 1:
			assert False, 'The root could be only one'
		for key, value in data.items():
			# I don't want to 'pop' and modify dict and I don't know the name of this root key
			document = Document()

			__createXmlTree(document, key, value, document)

			with open(filePath, 'w') as resultFile:
				resultFile.write(document.toprettyxml())
	else:
		with open(filePath, 'w') as f:
			xmltodict.unparse(data, f, pretty=True)
	xml.dom.minidom.Element = oldElement


def __createXmlTree(document, name, data, parent):
	if isinstance(data, (list, tuple)):
		for item in data:
			__createXmlTree(document, name, item, parent)
	else:
		node = document.createElement(tagName=name)
		if isinstance(data, dict):
			for k, v in data.items():
				if k.startswith('@'):
					node.setAttribute(k[1:], v)
				elif k == '//comment':
					node.appendChild(document.createComment(v))
				else:
					__createXmlTree(document, k, v, node)
		elif isinstance(data, str):
			node.appendChild(document.createTextNode(data))
		parent.appendChild(node)


# For monkey patching
def _write_data(writer, data):
	"Writes datachars to writer."
	if data:
		data = data.replace("&", "&amp;").replace("<", "&lt;"). \
					replace("\"", "&quot;").replace(">", "&gt;")
		writer.write(data)


class PatchedElement(xml.dom.minidom.Element):
	def __init__(self, *args, **kwargs):
		super().__init__(*args, **kwargs)
		self._attrs = OrderedDict()
		self._attrsNS = OrderedDict()

	# For monkey patching
	def writexml(self, writer, indent="", addindent="", newl=""):
		# indent = current indentation
		# addindent = indentation to add to higher levels
		# newl = newline string
		"""

		:type self: xml.dom.minidom.Element
		"""
		writer.write(indent +"<" + self.tagName)

		attrs = self._get_attributes()
		# a_names = sorted(attrs.keys())
		# ^ because of this little shit I have to monkey patch xml.dom.minidom.Element class
		a_names = attrs.keys()


		for a_name in a_names:
			writer.write(" %s=\"" % a_name)
			_write_data(writer, attrs[a_name].value)
			writer.write("\"")
		if self.childNodes:
			writer.write(">")
			if (len(self.childNodes) == 1 and
				self.childNodes[0].nodeType == Node.TEXT_NODE):
				self.childNodes[0].writexml(writer, '', '', '')
			else:
				writer.write(newl)
				for node in self.childNodes:
					node.writexml(writer, indent+addindent, addindent, newl)
				writer.write(indent)
			writer.write("</%s>%s" % (self.tagName, newl))
		else:
			writer.write("/>%s"%(newl))


# TODO: Deprecated functions, will be removed later
def parseMapsSettingsXml(filePath):
	"""

	:type filePath: str
	:param filePath: Path to 'mapsSettings.xml' file
	:rtype : dict[str, Space]
	"""
	doc = parse(filePath)
	result = dict()
	mapsList = doc.getElementsByTagName('publicMapsList')[0]
	for child in mapsList.childNodes:
		if child.nodeType == child.ELEMENT_NODE:
			result[child.getAttribute('name')] = Space(child.getAttribute('name'))

	matchesList = doc.getElementsByTagName('matches')[0]
	for match in matchesList.childNodes:
		if match.nodeType == match.ELEMENT_NODE:
			space = result.get(match.getAttribute('map'), Space(mapName='$Undefined$', isEnabled=False))
			scenario = Scenario(match.getAttribute('logic'), match.getAttribute('scenario'), match.getAttribute('map'), 'pvp' if not match.hasAttribute('matchGroup') else match.getAttribute('matchGroup'))
			for levels in match.childNodes:
				if levels.nodeType == levels.ELEMENT_NODE:
					level = Level(levels.getAttribute('list'), levels.getAttribute('rate'))
					scenario.levels.append(level)
			space.scenarios.append(scenario)
	return result


def parseSpaceManager(filePath):
	"""
	This function takes valid path to SpaceManager.py and reads content of 'gGlobalMapsList' list.
	The problem is that MapsSettingsEditor uses Python 3.4 and SpaceManager.py is Python 2.7 and you cannot easily
	import one from another.

	:type filePath: str
	:rtype: list[str]
	"""
	result = []
	with open(filePath) as f:
		start = False
		for line in f:
			if start is True:
				m = re.search('"(.+)"', line)
				if m:
					result.append(m.group(1))
				if ']' in line:
					break
			if 'gGlobalMapsList' in line:
				start = True
	return result


def parseSpaceScenarios(filePath):
	"""

	:type filePath: str
	:rtype : list[Scenario]
	"""
	try:
		doc = parse(filePath)
	except xml.parsers.expat.ExpatError as ee:
		print(ee.args)
		return [Scenario('$invalid$', '$invalid$', '$invalid$', status=ee.args)]
	result = list()
	logicsNode = doc.getElementsByTagName('logics')[0]
	logicNode = logicsNode.getElementsByTagName('logic')[0]
	for scenarioNode in logicNode.childNodes:
		if scenarioNode.nodeType == scenarioNode.ELEMENT_NODE:
			scenario = Scenario(scenarioNode.getAttribute('name'), scenarioNode.getAttribute('gameMode'), filePath.split('/')[-2],
								'pvp' if not scenarioNode.hasAttribute('matchGroup') else scenarioNode.getAttribute(
									'matchGroup'))
			result.append(scenario)
	return result


def parseContentPackages(filePath):
	"""

	:type filePath: str
	:rtype : list[Package]
	"""
	doc = parse(filePath)
	result = list()
	""":type : list[Package]"""
	packagesNode = doc.getElementsByTagName('packages')[0]
	for packageNode in packagesNode.getElementsByTagName('package'):
		if packageNode.nodeType == packageNode.ELEMENT_NODE:
			result.append(Package(packageNode.getAttribute('name'), int(packageNode.getAttribute('downloadPriority')) if packagesNode.hasAttribute('downloadPriority') else None))
			spaces = packageNode.getElementsByTagName('spaces')
			if spaces:
				for spaceNode in spaces[0].getElementsByTagName('space'):
					result[-1].spaces.append(spaceNode.getAttribute('name'))
			gameObjects = packageNode.getElementsByTagName('gameObjects')
			if gameObjects:
				for gameObjectNode in gameObjects[0].getElementsByTagName('gameObject'):
					result[-1].gameObjects.append(gameObjectNode.getAttribute('name'))
			overheads = packageNode.getElementsByTagName('overheads')
			if overheads:
				for overheadNode in overheads[0].getElementsByTagName('overhead'):
					result[-1].overheads.append(overheadNode.getAttribute('name'))
	return result


def dumpDeserializedData(projectPath):
	import os
	from xml.dom.minidom import xml
	logFileName = '_'.join(projectPath.split('/'))+ '_log.txt'
	with open(logFileName, 'w') as f:
		def writeln(text):
			f.write(text + '\n')

		# 	MapsSettings
		header = '--MapsSettings.xml--'
		writeln(header)
		mapsSettingsXml = parseMapsSettingsXml(projectPath + Paths.MAPS_SETTINGS_XML)
		for spaceName, space in mapsSettingsXml.items():
			writeln('\n'.join(space.readableData()))
		writeln('-'*len(header))

		# 	SpaceManager
		header = '--SpaceManager.py--'
		writeln(header)
		spaceManagerPy = parseSpaceManager(projectPath + Paths.SPACE_MANAGER_PY)
		for space in spaceManagerPy:
			writeln(space)
		writeln('-'*len(header))

		# 	ContentPackages
		header = '--ContentPackages.xml--'
		writeln(header)
		contentPackagesXml = parseContentPackages(projectPath + Paths.CONTENT_PACKAGES_XML)
		for package in contentPackagesXml:
			writeln('\n'.join(package.readableData()))
		writeln('-'*len(header))

		# 	Scenarios
		header = '--Scenarios.xml--'
		for mapName in spaceManagerPy:
			scenarioPath = projectPath + Paths.SCENARIO_PATH.format(mapName=mapName.split('/')[-1])
			if os.path.exists(scenarioPath):
				localHeader = '--Scenarios.xml {}--'.format(mapName)
				writeln(localHeader)
				try:
					spaceScenarios = parseSpaceScenarios(scenarioPath)
				except xml.parsers.expat.ExpatError:
					writeln('Malformated data in file {}'.format(scenarioPath))
				else:
					for spaceScenario in spaceScenarios:
						writeln('\n'.join(spaceScenario.readableData()))
				writeln('-'*len(localHeader))
		writeln('-'*len(header))

		header = '--ContentPackages.xml--'
		writeln(header)
		contentPackagesXml = parseContentPackages(projectPath + Paths.CONTENT_PACKAGES_XML)
		for package in contentPackagesXml:
			writeln('\n'.join(package.readableData()))
		writeln('-'*len(header))