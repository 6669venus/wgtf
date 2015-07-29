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
class IComponentContext;

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

	bool Initialize(IComponentContext& contextManager, const GameBinaryLoader& gameLoader);
	void Term(IComponentContext & contextManager);

private: 

	std::vector<GameConponentInfo>	m_gameComponents;
};
