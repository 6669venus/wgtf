# -*- coding: utf-8 -*-
# AOR: GT team
from xml.dom.minidom import parse
import re

import Paths
from DataStructures import Space, Scenario, Level, Package


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
		if child.nodeType == child.TEXT_NODE:
			continue
		if child.nodeType == child.COMMENT_NODE:
			for match in re.finditer('map name="(.+)"', child.data):
				result[match.group(1)] = Space(match.group(1), False)
		if child.nodeType == child.ELEMENT_NODE:
			result[child.getAttribute('name')] = Space(child.getAttribute('name'))

	matchesList = doc.getElementsByTagName('matches')[0]
	for match in matchesList.childNodes:
		if match.nodeType == match.ELEMENT_NODE:
			space = result[match.getAttribute('map')]
			scenario = Scenario(match.getAttribute('logic'), match.getAttribute('scenario'), 'pvp' if not match.hasAttribute('matchGroup') else match.getAttribute('matchGroup'))
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
	doc = parse(filePath)
	result = list()
	logicsNode = doc.getElementsByTagName('logics')[0]
	logicNode = logicsNode.getElementsByTagName('logic')[0]
	for scenarioNode in logicNode.childNodes:
		if scenarioNode.nodeType == scenarioNode.ELEMENT_NODE:
			scenario = Scenario(scenarioNode.getAttribute('name'), scenarioNode.getAttribute('gameMode'),
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
			scenarioPath = projectPath + Paths.SCENARIO_PATH.format(mapName=mapName)
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