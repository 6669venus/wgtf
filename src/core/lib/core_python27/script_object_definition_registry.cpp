#include "pch.hpp"
#include "script_object_definition_registry.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "definition_details.hpp"
#include "definition_helper.hpp"


namespace ReflectedPython
{


struct ScriptObjectDefinitionDeleter
{
	ScriptObjectDefinitionDeleter( const PyScript::ScriptObject& object, ScriptObjectDefinitionRegistry& registry )
		: object_( object )
		, registry_( registry )
	{}


	void operator()( IClassDefinition* definition )
	{
		registry_.removeDefinition( object_, definition );
	}


	const PyScript::ScriptObject object_;
	ScriptObjectDefinitionRegistry& registry_;
};


template< typename PAIR_T >
class PairMatch
{
public:
	PairMatch( const PyScript::ScriptObject & match );
	bool operator()( const PAIR_T & entry ) const;
private:
	const PyScript::ScriptObject & match_;
};


template< typename PAIR_T >
PairMatch< PAIR_T >::PairMatch( const PyScript::ScriptObject & match )
	: match_( match )
{
}


template< typename PAIR_T >
bool PairMatch< PAIR_T >::operator()( const PAIR_T & entry ) const
{
	const auto & a = entry.first;
	const auto & b = match_;
	const auto result = a.compareTo( b, PyScript::ScriptErrorRetain() ) == 0;
	if (PyScript::Script::hasError())
	{
		// Error comparing objects, just compare pointer addresses
		PyScript::Script::clearError();
		return (a.get() == b.get());
	}
	return result;
}


ScriptObjectDefinitionRegistry::ScriptObjectDefinitionRegistry( IComponentContext& context )
	: context_( context )
	, definitionManager_( context )
	, hookListener_( new HookListener() )
{
	g_pHookContext = &context_;
	g_pHookLookup_ = &hookLookup_;
	g_listener_ = hookListener_;
}


ScriptObjectDefinitionRegistry::~ScriptObjectDefinitionRegistry()
{
	// All reflected Python objects should have been removed by this point
	assert( hookLookup_.empty() );
	cleanupListenerHooks( hookLookup_ );
	g_listener_.reset();
	g_pHookLookup_ = nullptr;
	g_pHookContext = nullptr;
}


void ScriptObjectDefinitionRegistry::init()
{
	assert( definitionManager_ != nullptr );

	definitionHelper_.reset( new ReflectedPython::DefinitionHelper );
	definitionManager_->registerDefinitionHelper( *definitionHelper_ );

	definitionManager_->registerPropertyAccessorListener(
		std::static_pointer_cast< PropertyAccessorListener >( hookListener_ ) );
}


void ScriptObjectDefinitionRegistry::fini()
{
	assert( definitionManager_ != nullptr );

	definitionManager_->deregisterPropertyAccessorListener(
		std::static_pointer_cast< PropertyAccessorListener >( hookListener_ ) );

	definitionManager_->deregisterDefinitionHelper( *definitionHelper_ );
	definitionHelper_.reset();
}


std::shared_ptr< IClassDefinition > ScriptObjectDefinitionRegistry::findOrCreateDefinition(
	const PyScript::ScriptObject & object )
{
	assert( object.exists() );
	assert( definitionManager_ != nullptr );

	std::lock_guard<std::mutex> lock( definitionsMutex_ );
	// Find uses a ScriptObject comparator which may raise script errors
	auto itr = std::find_if( definitions_.cbegin(),
		definitions_.cend(),
		PairMatch< DefinitionPair >( object ) );

	if (itr != definitions_.end())
	{
		std::shared_ptr<IClassDefinition> pointer = itr->second.lock();

		if (pointer != nullptr)
		{
			return pointer;
		}

		std::string definitionName = ReflectedPython::DefinitionDetails::generateName( object );
		assert( !definitionName.empty() );

		auto definition = definitionManager_->getDefinition( definitionName.c_str() );
		assert( definition != nullptr );
		definitionManager_->deregisterDefinition( definition );
	}

	auto definition = definitionManager_->registerDefinition(
		new ReflectedPython::DefinitionDetails( context_, object, hookLookup_ ) );
	assert( definition != nullptr );

	std::shared_ptr<IClassDefinition> pointer( definition, ScriptObjectDefinitionDeleter( object, *this ) );
	definitions_.emplace_back( DefinitionPair( object, pointer ) );
	ids_.emplace_back( IdPair( object, RefObjectId::generate() ) );

	return pointer;
}


std::shared_ptr< IClassDefinition > ScriptObjectDefinitionRegistry::findDefinition(
	const PyScript::ScriptObject & object )
{
	assert( object.exists() );

	std::lock_guard< std::mutex > lock( definitionsMutex_ );
	// Find uses a ScriptObject comparator which may raise script errors
	auto itr = std::find_if( definitions_.cbegin(),
		definitions_.cend(),
		PairMatch< DefinitionPair >( object ) );

	if (itr != definitions_.cend())
	{
		return itr->second.lock();
	}

	return nullptr;
}


void ScriptObjectDefinitionRegistry::removeDefinition(
	const PyScript::ScriptObject& object, const IClassDefinition* definition )
{
	std::lock_guard<std::mutex> lock( definitionsMutex_ );
	assert( definition != nullptr );

	auto itr = std::find_if( definitions_.cbegin(),
		definitions_.cend(),
		PairMatch< DefinitionPair >( object ) );

	if (itr == definitions_.cend() || itr->second.use_count() > 0)
	{
		return;
	}

	definitions_.erase( itr );
	auto idItr = std::find_if( ids_.cbegin(),
		ids_.cend(),
		PairMatch< IdPair >( object ) );
	assert( idItr != ids_.cend() );
	ids_.erase( idItr );

	assert( definitionManager_ != nullptr );
	const bool success = definitionManager_->deregisterDefinition( definition );
	assert( success );
}


const RefObjectId & ScriptObjectDefinitionRegistry::getID(
	const PyScript::ScriptObject & object ) /* override */
{
	assert( object.exists() );

	std::lock_guard< std::mutex > lock( definitionsMutex_ );

	auto itr = std::find_if( ids_.cbegin(),
		ids_.cend(),
		PairMatch< IdPair >( object ) );

	// Object must have been registered with getDefinition()
	assert( itr != ids_.cend() );

	return itr->second;
}


} // namespace ReflectedPython
