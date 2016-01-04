# -*- coding: utf-8 -*-
# AOR: GT team
from collections import OrderedDict, defaultdict

class _ValidationParams:
	def __init__(self, name, valType, **kwargs):
		self.name = name
		self.valType = valType
		self.extraParams = kwargs

def validation(valType, **kwargs):
	def wrap(func):
		def wrapped_func(*func_args):
			return func(*func_args)
		wrapped_func.validationParams = _ValidationParams(func.__name__, valType, **kwargs)
		return property(wrapped_func)
	return wrap


# a_pirogov: this class is my addition. I believe that I'll need that 'insert' functionality a lot
class OrderedDictWithInsert(OrderedDict):
	def insert(self, index, key, value):
		if key in self:
			assert(index <= len(self) - 1)
			del self[key]
		else:
			assert(index <= len(self))

		numberToRemove = len(self) - index
		stack = OrderedDict()
		while numberToRemove:
			k, v = self.popitem()
			stack[k] = v
			numberToRemove -= 1
		self[key] = value
		while stack:
			k, v = stack.popitem()
			self[k] = v


class Map(object):
	def __init__(self, name='', status='valid'):
		self.status = status
		self.__name = name
		self.enabled = False
		self.scenarios = defaultdict(list)

	@validation(str, regex = "spaces/[a-zA-Z0-9_]+")
	def name(self):
		return self.__name

	def fullName(self):
		return str(self.__name)

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
	def __init__(self, logic, scenario, mapName, matchGroup='pvp', status='ok', gameMode=None):
		"""
		:type matchGroup: str
		:type scenario: str
		:type logic: str
		"""
		self.gameMode = gameMode
		self.enabled = False
		self.mapName = mapName
		self.__matchGroup = matchGroup
		self.__logic = logic
		self.scenario = scenario
		self.levels = list()
		self.status = status
		""":type :list[Level]"""
		self.extraData = dict()

	def readableData(self, indent=''):
		return [indent + self.__class__.__name__ + ':: ' + "matchGroup: {}, logic: {}, scenario: {}".format(self.matchGroup, self.logic, self.scenario)] + [item for level in self.levels for item in level.readableData(indent + '\t')]

	@validation(str, validValues = ["pvp", "pve", "ranked", "tutorial"])
	def matchGroup(self):
		return self.__matchGroup

	@validation(str, validValues = ["Default", "Domination", "Offline", "DefaultTest"])
	def logic(self):
		return self.__logic

	def fullName(self):
		return str(self.mapName) + "." + str(self.scenario)


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
