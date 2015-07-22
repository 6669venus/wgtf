//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  plg_file_system.cpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


#include <memory>
#include <vector>

class GameConponentInfo;	// game_common.h
class GameBinaryLoaderImpl;

class GameObject; 
class ObjectHandle; 
class Variant; 

typedef std::shared_ptr<GameObject> GameObjectPtr; 

// used to manage wgs game dlls loading and lifetime
class GameBinaryLoader
{
public: 

	GameBinaryLoader();
	~GameBinaryLoader(); 

	// singleton accessor to simplify access from random NGT objects.
	static GameBinaryLoader* GetInstance() { return m_Instance; }


	bool Initialize(); 
	void Term();

	bool CreateToolsApp(const char* project);
	bool InitializeGameComponents(std::vector<GameConponentInfo>& output) const;

	GameObjectPtr CreateObject(); 

	bool AddComponentToObject(const GameObjectPtr& obj, GameConponentInfo& info);
	bool RemoveComponentFromObject(const GameObjectPtr& obj, GameConponentInfo& info);

	Despair::IFileMgr& GetFileMgr() const;
	Despair::IGraphicsRenderer& GetRenderer() const;

	bool setValue(const ObjectHandle & handle, const Variant & value, const Despair::Reflection::ClassField& field); 
	Variant getValue(const ObjectHandle & handle, const Despair::Reflection::ClassField& field); 

private: 

	// using pimpl pattern to hide all the nasty include tricks from the rest of the codebase
	GameBinaryLoaderImpl*		m_Impl;

	static GameBinaryLoader*	m_Instance; 
};
