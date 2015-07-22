//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  plg_file_system.cpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


#include <memory>
#include "game_common.h"

class GameBinaryLoader; 
class IContextManager;

class IQtTypeConverter; 
class IComponentProvider; 
class DespairClassDefinition; 
class IClassDefinition; 

// used to manage wgs game dlls loading and lifetime
class GameTypeProvider
{
public: 

	GameTypeProvider();
	~GameTypeProvider();

	bool Initialize(IContextManager& contextManager, const GameBinaryLoader& gameLoader);
	void Term(IContextManager & contextManager);

private: 

	std::vector<GameConponentInfo>	m_gameComponents;
};
