#include "generic_plugin/generic_plugin.hpp"
#include "generic_plugin/interfaces/i_plugin_context_creator.hpp"

#include "reflection_component_provider.hpp"
#include "reflection/definition_manager.hpp"
#include "reflection/variant_handler.hpp"
#include "reflection/object_manager.hpp"
#include "reflection/reflected_types.hpp"
#include "reflection/metadata/meta_types.hpp"
#include "context_definition_manager.hpp"
#include "serialization/serializer/i_serialization_manager.hpp"
#include "reflection_utils/serializer/reflection_serializer.hpp"
#include "variant/variant.hpp"
#include "ui_framework/i_ui_framework.hpp"


//==============================================================================
class ReflectionSystemContextManager
	: public Implements< IPluginContextCreator >
{
public:
	//==========================================================================
	const char * getType() const
	{
		return typeid( ContextDefinitionManager ).name();
	}

	IInterface * createContext( const wchar_t * contextId );
};


//==============================================================================
class ReflectionSystemHolder
{
public:
	//==========================================================================
	ReflectionSystemHolder()
		: objectManager_( new ObjectManager() )
		, definitionManager_( new DefinitionManager( *objectManager_ ) )
		, contextManager_( new ReflectionSystemContextManager )
	{
		objectManager_->init( definitionManager_.get() );
		s_definitionManager_ = definitionManager_.get();
	}


	//==========================================================================
	~ReflectionSystemHolder()
	{
		objectManager_.reset(); 
		s_definitionManager_ = nullptr;
		definitionManager_.reset();
		contextManager_.reset();
	}


	//==========================================================================
	static IDefinitionManager * getGlobalDefinitionManager()
	{
		return s_definitionManager_;
	}


	//==========================================================================
	DefinitionManager * getDefinitionManager()
	{
		return definitionManager_.get();
	}


	//==========================================================================
	ObjectManager * getObjectManager()
	{
		return objectManager_.get();
	}

	//==========================================================================
	ReflectionSystemContextManager * getContextManager()
	{
		return contextManager_.get();
	}

private:
	static IDefinitionManager *							s_definitionManager_;
	std::unique_ptr< ObjectManager >					objectManager_;
	std::unique_ptr< DefinitionManager >				definitionManager_;
	std::unique_ptr< ReflectionSystemContextManager >	contextManager_;
};

IDefinitionManager * ReflectionSystemHolder::s_definitionManager_ = nullptr;

//==========================================================================
IInterface * ReflectionSystemContextManager::createContext(
	const wchar_t * contextId )
{
	auto contextManager =
		new ContextDefinitionManager( contextId );
	contextManager->init(
		ReflectionSystemHolder::getGlobalDefinitionManager() );
	return new InterfaceHolder< ContextDefinitionManager >( contextManager, true );
}


//==============================================================================
class ReflectionPlugin
	: public PluginMain
{
private:
	std::unique_ptr< ReflectionSystemHolder >	reflectionSystemHolder_;
	std::vector< IInterface * >					types_;
	std::unique_ptr< ReflectionSerializer > reflectionSerializer_;
	std::unique_ptr< MetaTypeImpl< ObjectHandle > >			baseProviderMetaType_;
	std::unique_ptr< IStorageLookupHandler >		variantStorageLookupHandler_;
	std::unique_ptr< ReflectionComponentProvider > reflectionComponentProvider_;
public:
	//==========================================================================
	ReflectionPlugin( IContextManager & contextManager )
		: reflectionSystemHolder_( new ReflectionSystemHolder )
		, baseProviderMetaType_( new MetaTypeImpl<ObjectHandle>() )
		, variantStorageLookupHandler_(
			new ReflectionStorageLookupHandler(
				reflectionSystemHolder_->getDefinitionManager(),
				baseProviderMetaType_.get() ))
	{ 
		types_.push_back(
			contextManager.registerInterface( reflectionSystemHolder_->getObjectManager(), false ) );
		types_.push_back(
			contextManager.registerInterface( reflectionSystemHolder_->getDefinitionManager(), false ) );
		types_.push_back(
			contextManager.registerInterface( reflectionSystemHolder_->getContextManager(), false ) );
		Reflection::initReflectedTypes( *reflectionSystemHolder_->getDefinitionManager() );
	}

	//==========================================================================
	void Initialise( IContextManager & contextManager ) override
	{
		auto metaTypeMgr = contextManager.queryInterface<IMetaTypeManager>();
		if(metaTypeMgr)
		{
			Variant::setMetaTypeManager( metaTypeMgr );
			metaTypeMgr->registerType( baseProviderMetaType_.get() );
			metaTypeMgr->registerDynamicStorageHandler( *variantStorageLookupHandler_ );
		}
		auto serializationMgr = contextManager.queryInterface<ISerializationManager>();
		if (serializationMgr)
		{
			reflectionSerializer_.reset( 
				new ReflectionSerializer( 
					*serializationMgr, *metaTypeMgr, *(reflectionSystemHolder_->getObjectManager()) ) );
			ObjectManager* objManager = 
				static_cast<ObjectManager*>(reflectionSystemHolder_->getObjectManager());
			objManager->setSerializationManager( serializationMgr );
			for(auto type : reflectionSerializer_->getSupportedType())
			{
				serializationMgr->registerSerializer( 
					type.getName(), reflectionSerializer_.get() );
			}
		}
		auto uiFramework = contextManager.queryInterface<IUIFramework>();
		if (uiFramework)
		{
			reflectionComponentProvider_.reset( 
				new ReflectionComponentProvider( 
					*reflectionSystemHolder_->getDefinitionManager() ) );
			uiFramework->registerComponentProvider( *reflectionComponentProvider_ );
		}
	}

	//==========================================================================
	bool Finalise( IContextManager & contextManager ) override
	{
		auto serializationMgr = contextManager.queryInterface<ISerializationManager>();
		if (serializationMgr)
		{
			for(auto type : reflectionSerializer_->getSupportedType())
			{
				serializationMgr->deregisterSerializer( type.getName() );
			}
			reflectionSerializer_ = nullptr;
		}
		auto metaTypeMgr = contextManager.queryInterface<IMetaTypeManager>();
		if(metaTypeMgr)
		{
			metaTypeMgr->deregisterDynamicStorageHandler(
				*variantStorageLookupHandler_ );
		}
		return true;
	}

	//==========================================================================
	void Unload( IContextManager & contextManager ) override
	{
		for( auto type : types_)
		{
			contextManager.deregisterInterface( type );
		}
	}
};

PLG_CALLBACK_FUNC( ReflectionPlugin )