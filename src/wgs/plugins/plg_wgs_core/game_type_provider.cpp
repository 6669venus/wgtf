//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  gameloader.cpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include <string>
#include <functional>

#include "serialization/interfaces/i_file_utilities.hpp"
#include "serialization/serializer/i_serialization_manager.hpp"
#include "variant/interfaces/i_meta_type_manager.hpp"
#include "generic_plugin/interfaces/i_context_manager.hpp"
#include "reflection/i_definition_manager.hpp"

#include "ds_stubs.h"
#include "game_type_provider.h"
#include "game_loader.h"
#include "despair_class.h"

namespace Despair
{

}

GameTypeProvider::GameTypeProvider()
{
}

GameTypeProvider::~GameTypeProvider()
{

}

bool GameTypeProvider::Initialize(IContextManager & contextManager, const GameBinaryLoader& gameLoader)
{

	IDefinitionManager& definitionManager = *contextManager.queryInterface<IDefinitionManager>();
	
	auto metaTypeMgr = contextManager.queryInterface<IMetaTypeManager>();
	if (metaTypeMgr)
	{
		m_gameComponents.clear(); 
		gameLoader.InitializeGameComponents(m_gameComponents); 

		Variant::setMetaTypeManager(metaTypeMgr);
		
		for (auto itr = m_gameComponents.begin(); itr != m_gameComponents.end(); ++itr)
		{
			definitionManager.registerDefinition(new DespairClassDefinition((*itr).id));
		}

	}

	return false; 
}

void GameTypeProvider::Term(IContextManager & contextManager)
{
	IDefinitionManager& definitionManager = *contextManager.queryInterface<IDefinitionManager>();
	for (auto itr = m_gameComponents.begin(); itr != m_gameComponents.end(); ++itr)
	{
		definitionManager.deregisterDefinition(definitionManager.getDefinition((*itr).id->GetName()));
	}
}




