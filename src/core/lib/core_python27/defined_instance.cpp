#include "pch.hpp"

#include "defined_instance.hpp"
#include "definition_details.hpp"
#include "property.hpp"

#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_reflection/interfaces/i_class_definition.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"
#include "interfaces/core_python_script/i_scripting_engine.hpp"
#include "i_script_object_definition_registry.hpp"


namespace ReflectedPython
{


DefinedInstance::DefinedInstance()
	: BaseGenericObject()
	, pythonObject_( nullptr )
	, pDefinition_( nullptr )
	, context_( nullptr )
	, pParent_( nullptr )
{
	assert( false && "Always construct with a Python object" );
}


DefinedInstance::DefinedInstance(
	IComponentContext & context,
	const PyScript::ScriptObject & pythonObject,
	std::shared_ptr< IClassDefinition > & definition,
	const DefinedInstance * parent,
	const std::string & childPath )
	: BaseGenericObject()
	, pythonObject_( pythonObject )
	, pDefinition_( definition )
	, context_( &context )
	, pParent_( parent )
{
	if (parent != nullptr)
	{
		fullPath_ = parent->fullPath();
		if (!fullPath_.empty())
		{
			fullPath_ += DOT_OPERATOR;
		}
	}
	fullPath_ += childPath;

	setDefinition( pDefinition_.get() );
	const auto & details = definition->getDetails();
	const auto & definitionDetails = static_cast< const DefinitionDetails & >( details );
	definitionDetails.instance( *this );
}


DefinedInstance::~DefinedInstance()
{
}


/* static */ ObjectHandle DefinedInstance::findOrCreate( IComponentContext & context,
	const PyScript::ScriptObject & pythonObject )
{
	const DefinedInstance * pParent = nullptr;
	const std::string childPath;
	return findOrCreateInternal( context, pythonObject, pParent, childPath );
}


/* static */ ObjectHandle DefinedInstance::findOrCreate( IComponentContext & context,
	const PyScript::ScriptObject & pythonObject,
	const DefinedInstance & parent,
	const std::string & childPath )
{
	return findOrCreateInternal( context, pythonObject, &parent, childPath );
}


/* static */ ObjectHandle DefinedInstance::findOrCreateInternal( IComponentContext & context,
	const PyScript::ScriptObject & pythonObject,
	const DefinedInstance * pParent,
	const std::string & childPath )
{
	assert( pythonObject.exists() );

	// Get a definition that's the same for each ScriptObject instance
	auto pRegistry = context.queryInterface< IScriptObjectDefinitionRegistry >();
	assert( pRegistry != nullptr );
	auto & registry = (*pRegistry);

	auto pDefinition = registry.findOrCreateDefinition( pythonObject );
	assert( pDefinition != nullptr );
	auto & definition = (*pDefinition);

	const auto & id = registry.getID( pythonObject );

	// Search for an existing object handle that's using that definition
	auto pObjectManager = context.queryInterface< IObjectManager >();
	assert( pObjectManager != nullptr );
	auto & objectManager = (*pObjectManager);
	auto handle = objectManager.getObject( id );
	if (handle.isValid())
	{
		return handle;
	}

	// Existing object handle not found, construct a new instance
	auto pInstance = std::unique_ptr< ReflectedPython::DefinedInstance >(
		new DefinedInstance( context, pythonObject, pDefinition, pParent, childPath ) );
	ObjectHandleT< ReflectedPython::DefinedInstance > handleT(
		std::move( pInstance ),
		&definition );
	handle = handleT;

	// Register with IObjectManager to generate an ID
	// IObjectManager should take a weak reference
	handle = objectManager.registerObject( handle, id );
	assert( handle.isValid() );

	// Registered reference
	return handle;
}


/* static */ ObjectHandle DefinedInstance::find( IComponentContext & context,
	const PyScript::ScriptObject & pythonObject )
{
	assert( pythonObject.exists() );

	// Get a definition that's the same for each ScriptObject instance
	auto pRegistry = context.queryInterface< IScriptObjectDefinitionRegistry >();
	assert( pRegistry != nullptr );
	auto & registry = (*pRegistry);

	auto pDefinition = registry.findDefinition( pythonObject );
	if (pDefinition == nullptr)
	{
		return nullptr;
	}
	auto & definition = (*pDefinition);

	const auto & id = registry.getID( pythonObject );

	// Search for an existing object handle that's using that definition
	auto pObjectManager = context.queryInterface< IObjectManager >();
	assert( pObjectManager != nullptr );
	auto & objectManager = (*pObjectManager);
	return objectManager.getObject( id );
}


const PyScript::ScriptObject & DefinedInstance::pythonObject() const
{
	return pythonObject_;
}


const DefinedInstance & DefinedInstance::root() const
{
	const DefinedInstance * pParent = this;
	while (pParent->pParent_ != nullptr)
	{
		pParent = pParent->pParent_;
	}
	assert( pParent != nullptr );
	return (*pParent);
}


const std::string & DefinedInstance::fullPath() const
{
	return fullPath_;
}


ObjectHandle DefinedInstance::getDerivedType() const
{
	auto pRegistry = context_->queryInterface< IScriptObjectDefinitionRegistry >();
	assert( pRegistry != nullptr );
	auto & registry = (*pRegistry);

	auto pObjectManager = context_->queryInterface< IObjectManager >();
	assert( pObjectManager != nullptr );
	auto & objectManager = (*pObjectManager);

	const auto & id = registry.getID( pythonObject_ );
	return objectManager.getObject( id );
}


ObjectHandle DefinedInstance::getDerivedType()
{
	auto pRegistry = context_->queryInterface< IScriptObjectDefinitionRegistry >();
	assert( pRegistry != nullptr );
	auto & registry = (*pRegistry);

	auto pObjectManager = context_->queryInterface< IObjectManager >();
	assert( pObjectManager != nullptr );
	auto & objectManager = (*pObjectManager);

	const auto & id = registry.getID( pythonObject_ );
	return objectManager.getObject( id );
}


} // namespace ReflectedPython
