# -*- coding: utf-8 -*-
import MapsSettings
import random
try:
	from debug_utils import LOG_WARNING
except Exception, e: # hack for python exe
	print e

UNSPECIFIED_MAP_ID 	= 0xFFFF
UNSPECIFIED_LVL  	= 0xFF
UNSPECIFIED_MODE 	= ''


def getClientMapsSettings():
	return MapsSettings.packedMapsListAndSettings

def getRandomMatch(lvl, matchGroup = MapsSettings.defaultMatchGroup, logic=None):
	choiceArray = MapsSettings.matchSelector.matchGroup[matchGroup].level[lvl].choiceArray()
	assert len(choiceArray) > 0, ("SpaceManager.getRandomMatch: matches for matchGroup = " + matchGroup + " and lvl = " + str(lvl) + " is not available.")
	match = random.choice(choiceArray)
	return lvl, match.logic, match.scenario, match.mapId
	pass


# editable.
gPublicMapList = []

gDefaultMap = 'spaces/10_NE_big_race'


# never, never delete items from this list. NEVER!!!!
# increments only allowed.
#======== N E V E R   D E L E T E   I T E M S   F R O M    T H I S   L I S T
gGlobalMapsList = [
	"spaces/00_pure_gameplay",        #  1
	"spaces/15_NE_north",             #  2
	"spaces/16_OC_bees_to_honey",     #  3
	"spaces/17_NA_fault_line",        #  4
	"spaces/18_NE_ice_islands",       #  5
	"spaces/19_OC_prey",              #  6
	"spaces/20_BD_volley_fire",       #  7
	"spaces/test_gd",                 #  8
	"spaces/ocean",                   #  9
	"spaces/20_NE_two_brothers",      # 10
	"spaces/21_AS_flower",            # 11
	"spaces/22_OC_wind_rose",         # 12
	"spaces/24_AS_attack",            # 13
	"spaces/25_NE_morning_breeze",    # 14
	"spaces/26_OC_on_two_front",      # 15
	
#==============================================================================================================
# Ниже идет 5 постоянных слотов временного положения карт для плей-тестов. Ни одна из них не может быть в релизе.
#==============================================================================================================

	"spaces/SLOT_FOR_GD1",            # 16
	"spaces/12_OC_sea_bases",         # 17
	"spaces/11_NA_seadog",            # 18
	"spaces/02_new_map",              # 19
	"spaces/07_SA_tierra_del_fuego",  # 20


	"spaces/robo-battle",             # 21
	"spaces/01_solomon_islands",      # 22
	"spaces/10_NE_big_race",          # 23
	"spaces/31_NE_archipelago",       # 24
	"spaces/34_OC_islands",           # 25
	"spaces/16_OC_bees_to_honey_01",  # 26
	"spaces/32_AS_artful_rocks",      # 27
	"spaces/01_NA_escort",            # 28
	"spaces/01_NA_escort_markers",    # 29
	"spaces/02_NE_fiords",            # 30
	"spaces/13_OC_new_dawn",          # 31
	"spaces/00_domination_test",      # 32
	"spaces/intro_start",             # 33
	"spaces/00_CO_ocean",             # 34
	"spaces/99_deep_space",           # 35
	"spaces/i01_tutorial",            # 36
	"spaces/02_co_pacific_ocean",     # 37
	"spaces/08_NE_passage",            # 38
	"spaces/23_OC_islands_rangunutu",      # 39
	"spaces/35_NE_north_winter",	  # 40
	"spaces/22_tierra_del_fuego",	  # 40

]
#=================  T H E   E N D  ===========================================

gDisabledMapList = []

# a little bit hardcode here =(
# TO DO: remove hardcode
_clientList = None
_clientIdToGlobalMap = None
_clientNameIdMap = None

#
def _initDicts():
	global _clientList
	global _clientNameIdMap
	global _clientIdToGlobalMap
	
	_clientList = MapsSettings.mapsList[:]
	_clientNameIdMap = dict( [(map, cMapId) for cMapId, map in enumerate(_clientList)] )
	_clientIdToGlobalMap = dict( [(cMapId,  gGlobalMapsList.index(map)) for cMapId, map in enumerate(_clientList)] )

#
def initLists():
	# for dirname, dirnames, filenames in os.walk('../../../wob/res/spaces'):
		# for subdirname in dirnames:
			# if not subdirname.startswith('.'):
				# mapName = (os.path.join(dirname, subdirname)).split('/wob/res/')[1]
				# if not mapName.startswith('.'):
					# print dirname, subdirname, mapName
					#assert mapName in gGlobalMapsList, "please, add %s to SpaceManager.gGlobalMapsList"%mapName
	
	
	testSet = set()
	for map in gPublicMapList:
		if map not in testSet:
			testSet.add(map)
		else:
			assert False, "map %s defined a twice"%map
	
	MapsSettings.init(gPublicMapList, gDisabledMapList)
	_initDicts()
	
initLists()


def updateDisabledMapList(disableList):
	global gDisabledMapList
	
	gDisabledMapList = []
	
	for mapId in disableList:
		mapName = gGlobalMapsList[mapId]
		if mapName not in gPublicMapList:
			continue
		mapIdx = gPublicMapList.index(mapName)
		
		LOG_WARNING("Map %s/%d disabled by admin" % (mapName, mapIdx))
		
		gDisabledMapList.append(mapIdx)
	
	MapsSettings.init(gPublicMapList, gDisabledMapList)
	_initDicts()
	return

def getDisabledMapList():
	return gDisabledMapList

#---------------------------------
def getClientList():
	return _clientList


#---------------------------------
def getMapByClientId(mapId):
	if mapId < len(_clientList) and mapId >= 0:
		return _clientList[mapId]
	return gDefaultMap
	# return gClientMaps.get(mapId, gDefaultMap)
	pass

def getGlobalIdByClientId(cMapId):
	return _clientIdToGlobalMap[cMapId]
	pass

def getClientNameByMapId(mapId):
	return _clientNameIdMap[mapId]

#---------------------------------
def getMapByGlobalId(mapId):
	return gGlobalMapsList[mapId] if len(gGlobalMapsList) > mapId and mapId >= 0 else gDefaultMap



def test():
	print "----------------------------------"
	# for n in xrange(99):
		# print getRandomLvl()

	# for n in xrange(30):
		# print "lvl:", n
		# for i in xrange(20):
			# print "\t",getRandomMod(n)
	# print "----------------------------------"
	# for n in xrange(20):
		# lvl, mod, map = getRandomMap(n)
		# print lvl, mod, _clientList[map] if map >= 0 and map < len(_clientList) else None
	# print "----------------------------------"
	MapsSettings.test(_clientList, gDisabledMapList)

if __name__ == "__main__":
	test()
