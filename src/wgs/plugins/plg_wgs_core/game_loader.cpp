//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  gameloader.cpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "game_common.h"
#include "game_loader.h"

#include <dsUtility\ScopedLibraryHandle.h>
#include <dsReflection/VariableHelpers.h>

#include "reflection/object_handle.hpp"
#include "variant/variant.hpp"

#if _DEBUG
#define WGS_GAME_BINARY L"gamecentralutils_w64d.dll"
#else
#define WGS_GAME_BINARY L"gamecentralutils_w64r.dll"
#endif

using namespace Despair::GameCentralUtils; 

GameObject::~GameObject()
{
}

typedef std::unique_ptr<HINSTANCE, Despair::LibraryHandleDeleter> ScopedLibraryHandle;


inline IToolsFactory* CreateFactory(HMODULE dll)
{
	typedef IToolsFactory* (CreateFactoryFunc)();
	CreateFactoryFunc* createFactoryFunc = reinterpret_cast<CreateFactoryFunc*>(::GetProcAddress(dll, "CreateToolsFactory"));
	if (!createFactoryFunc)
	{
		return nullptr;
	}
	return (*createFactoryFunc)();
}

class GameBinaryLoaderImpl
{
public:
	GameBinaryLoaderImpl()
		: m_App(nullptr)
		, m_Factory(nullptr)
	{}

public: 
	
	bool Initialize()
	{
		if (!m_gameCentralUtilsDLL)
		{
			m_gameCentralUtilsDLL.reset(LoadLibrary(WGS_GAME_BINARY));
			if (!m_gameCentralUtilsDLL)
			{
				return false;
			}
		}

		return true;
	}

	bool CreateToolsApp(const char* project)
	{
		if (!m_gameCentralUtilsDLL)
			return false; 

		m_Factory = CreateFactory(m_gameCentralUtilsDLL.get());
		if (m_Factory == nullptr)
		{
			return false;
		}

		m_App = m_Factory->CreateToolsApp(project, "", "");
		if (m_App == nullptr)
		{
			return false;
		}

		return true;
	}

	void Term()
	{
		if (m_App != nullptr)
		{
			m_App->Release(); 
			m_App = nullptr;
		}

		if (m_Factory != nullptr)
		{
			m_Factory->Release();
			m_Factory = nullptr;
		}

		// force-nuke the dll instead of waiting for the process to shut-down
		// crash warning - when running a x64|Debug gamecentralutils.dll compiled with VS2013, there is a crash in std::converter<>.  
		// more details can be found here: https://connect.microsoft.com/VisualStudio/feedback/details/796566/vc-stl-alloc-dealloc-mismatch-in-locale
		m_gameCentralUtilsDLL.swap(ScopedLibraryHandle(nullptr));
	}

	IToolsFactory*	GetFactory() { return m_Factory; }
	IToolsApp*		GetApp() { return m_App; }

private: 
	
	ScopedLibraryHandle					m_gameCentralUtilsDLL;
	IToolsFactory*						m_Factory;
	IToolsApp*							m_App;
};

GameBinaryLoader* GameBinaryLoader::m_Instance = nullptr; 

GameBinaryLoader::GameBinaryLoader()
	: m_Impl(new GameBinaryLoaderImpl())
{
	assert(GameBinaryLoader::m_Instance == nullptr);
	GameBinaryLoader::m_Instance = this; 
}
GameBinaryLoader::~GameBinaryLoader()
{
	GameBinaryLoader::m_Instance = nullptr; 
	delete m_Impl;
}

bool GameBinaryLoader::Initialize()
{
	if( !m_Impl->Initialize() )
		return false; 

	return true;
}

bool GameBinaryLoader::CreateToolsApp(const char* project)
{
	// load gamecentralutils.dll so that we can access the rest of the game API
	if (!Initialize())
	{
		return false;
	}

	return m_Impl->CreateToolsApp(project);
}

void GameBinaryLoader::Term()
{
	m_Impl->Term();
}

bool GameBinaryLoader::InitializeGameComponents(std::vector<GameConponentInfo>& output) const
{
	IToolsApp* app = m_Impl->GetApp();
	if( app == nullptr )
		return false;

	app->EnumerateComponentTypes([&](Despair::Reflection::ClassId id, const char* name, const char* type) {

		GameConponentInfo info;
		info.id = id;
		info.name = name;
		info.type = type;

		output.emplace_back(info);
	});

	return true; 
}

GameObjectPtr GameBinaryLoader::CreateObject()
{
	IToolsApp* app = m_Impl->GetApp();
	if (app == nullptr)
		return nullptr;

	GameObjectPtr o = std::make_shared<GameObject>();
	//o->impl_ = app->CreateObject();
	return o;
}

bool GameBinaryLoader::AddComponentToObject(const GameObjectPtr& obj, GameConponentInfo& info)
{
	IToolsApp* app = m_Impl->GetApp();
	if( app == nullptr )
		return false;
	
	return false; 
}

bool GameBinaryLoader::RemoveComponentFromObject(const GameObjectPtr& obj, GameConponentInfo& info)
{
	return false; 
}

Despair::IFileMgr& GameBinaryLoader::GetFileMgr() const
{
	assert(m_Impl);
	assert(m_Impl->GetApp()); 

	return m_Impl->GetApp()->GetFileMgr();
}

Despair::IGraphicsRenderer& GameBinaryLoader::GetRenderer() const
{
	assert(m_Impl);
	assert(m_Impl->GetApp());

	return m_Impl->GetApp()->GetRenderer();
}

bool GameBinaryLoader::setValue(const ObjectHandle & handle, const Variant & value, const Despair::Reflection::ClassField& field)
{
	auto parentObject = handle.getBase<GameObject>();

	switch (field.GetType())
	{
		case Despair::Reflection::ClassField::FIELDTYPE_INT:
		case Despair::Reflection::ClassField::FIELDTYPE_INT32:
		{
			parentObject->impl_->SetValue(field, value.cast< int >());
		}
		case Despair::Reflection::ClassField::FIELDTYPE_UINT:
		case Despair::Reflection::ClassField::FIELDTYPE_UINT32:
		{
			parentObject->impl_->SetValue(field, value.cast< unsigned int >());
		}
		case Despair::Reflection::ClassField::FIELDTYPE_BOOL:
		{
			parentObject->impl_->SetValue(field, value.cast< bool >());
		}
		case Despair::Reflection::ClassField::FIELDTYPE_FLOAT:
		{
			parentObject->impl_->SetValue(field, value.cast< float >());
		}
		case Despair::Reflection::ClassField::FIELDTYPE_DOUBLE:
		{
			parentObject->impl_->SetValue(field, value.cast< double >());
		}
		case Despair::Reflection::ClassField::FIELDTYPE_STRING:
		{
			parentObject->impl_->SetValue(field, value.cast< Despair::string >());
		}
		case Despair::Reflection::ClassField::FIELDTYPE_VECTOR3D:
		{
			parentObject->impl_->SetValue(field, value.cast< Despair::Vector3d >());
		}
	}

	return false;
}

Variant GameBinaryLoader::getValue(const ObjectHandle & handle, const Despair::Reflection::ClassField& field)
{
	auto parentObject = handle.getBase<GameObject>();

	switch (field.GetType())
	{
		case Despair::Reflection::ClassField::FIELDTYPE_INT:
		case Despair::Reflection::ClassField::FIELDTYPE_INT32:
		{
			return Variant(static_cast<int>(parentObject->impl_->GetInt(field)));
		}
		case Despair::Reflection::ClassField::FIELDTYPE_UINT:
		case Despair::Reflection::ClassField::FIELDTYPE_UINT32:
		{
			return Variant(static_cast<unsigned int>(parentObject->impl_->GetUnsignedInt(field)));
		}
		case Despair::Reflection::ClassField::FIELDTYPE_BOOL:
		{
			return Variant(parentObject->impl_->GetInt(field));
		}
		case Despair::Reflection::ClassField::FIELDTYPE_FLOAT:
		{
			return Variant(static_cast<float>(parentObject->impl_->GetFloat(field)));
		}
		case Despair::Reflection::ClassField::FIELDTYPE_DOUBLE:
		{
			return Variant(static_cast<double>(parentObject->impl_->GetDouble(field)));
		}
		case Despair::Reflection::ClassField::FIELDTYPE_STRING:
		{
			return Variant(parentObject->impl_->GetString(field).c_str());
		}
		case Despair::Reflection::ClassField::FIELDTYPE_VECTOR3D:
		{
			return Variant(parentObject->impl_->GetVector3D(field));
		}
		default:
		{
			throw std::logic_error("Attempting to set a value that has not been implemented by the game code yet.");
		}
	}

	return Variant();
}
