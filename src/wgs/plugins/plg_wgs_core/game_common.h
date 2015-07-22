//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  game_common.h
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef GAME_COMMON_H
#define GAME_COMMON_H

#define DS_PLATFORM_WINDOWS
#define DS_PLATFORM_64
#define WG_PLATFORM_NGT
#define TELEMETRY_ENABLED 0
#define DS_NO_INLINE_NEW_DELETE

#include <Windows.h>
#include <memory>
#include <assert.h>

#include <dsReflection.h>
#include <dsIO.h>
#include <dsIO/IOCommon.h>
#include <GameCentralUtils\IToolsFactory.h>

typedef Despair::GameCentralUtils::IReflectionObject IReflectionObject;

class GameConponentInfo 
{
public: 

	Despair::Reflection::ClassId id;
	std::string name; 
	std::string type;
};

class GameObject
{
public:
	~GameObject(); 

	IReflectionObject* impl_;
};

typedef std::shared_ptr<GameObject> GameObjectPtr; 

#endif // GAME_COMMON_H
