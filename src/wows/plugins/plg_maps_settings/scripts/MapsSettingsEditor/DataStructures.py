# -*- coding: utf-8 -*-
# AOR: GT team


class Map(object):
	def __init__(self, name=''):
		self.name = name
		self.enabled = False
		self.scenarios = []


class Level(object):
	def __init__(self, list, rate):
		"""
		:type rate: str
		:type list: str
		"""
		self.rate = rate
		self.list = list

	def readableData(self, indent=''):
		return [indent + self.__class__.__name__ + ':: ' + "rate: {}, list: {}".format(self.rate, self.list)]


class Scenario(object):
	def __init__(self, logic, scenario, mapName, matchGroup='pvp', status='ok'):
		"""
		:type matchGroup: str
		:type scenario: str
		:type logic: str
		"""
		self.enabled = False
		self.mapName = mapName
		self.matchGroup = matchGroup
		self.logic = logic
		self.scenario = scenario
		self.levels = list()
		self.status = status
		""":type :list[Level]"""
		self.extraData = dict()

	def readableData(self, indent=''):
		return [indent + self.__class__.__name__ + ':: ' + "matchGroup: {}, logic: {}, scenario: {}".format(self.matchGroup, self.logic, self.scenario)] + [item for level in self.levels for item in level.readableData(indent + '\t')]


class Space(object):
	def __init__(self, mapName='', isEnabled=True):
		"""
		:type mapName: str
		:type isEnabled: bool
		"""
		self.isEnabled = isEnabled
		self.mapName = mapName
		self.scenarios = list()
		""":type :list[Scenario]"""

	def readableData(self, indent=''):
		return [indent + self.__class__.__name__ + ':: ' + "isEnabled: {}, mapName: {}".format(self.isEnabled, self.mapName)] + [item for scenario in self.scenarios for item in scenario.readableData(indent + '\t')]


class Package(object):
	def __init__(self, name, downloadPriority=None):
		"""

		:type downloadPriority: int
		:type name: str
		"""
		self.downloadPriority = downloadPriority
		self.name = name
		self.spaces = list()
		""":type : list[str]"""
		self.gameObjects = list()
		""":type : list[str]"""
		self.overheads = list()
		""":type : list[str]"""

	def readableData(self):
		result = [self.__class__.__name__ + ':: ' + 'downloadPriority: {}, name: {}'.format(self.downloadPriority, self.name)]
		result.extend(['\tspace: ' + space for space in self.spaces])
		result.extend(['\tgameObject: ' + gameObject for gameObject in self.gameObjects])
		result.extend(['\toverhead: ' + overhead for overhead in self.overheads])
		return result
