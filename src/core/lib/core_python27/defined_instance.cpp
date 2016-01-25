#include "pch.hpp"
#include "defined_instance.hpp"

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
{
	assert( false && "Always construct with a Python object" );
}


DefinedInstance::DefinedInstance(
	IComponentContext & context,
	const PyScript::ScriptObject & pythonObject,
	std::shared_ptr< IClassDefinition > & definition )
	: BaseGenericObject()
	, pythonObject_( pythonObject )
	, pDefinition_( definition )
	, context_( &context )
{
	setDefinition( pDefinition_.get() );
}


DefinedInstance::~DefinedInstance()
{
}


/* static */ ObjectHandle DefinedInstance::create( IComponentContext & context,
	const PyScript::ScriptObject & pythonObject )
{
	assert( pythonObject.exists() );

	// Get a definition that's the same for each ScriptObject instance
	auto pRegistry = context.queryInterface< IScriptObjectDefinitionRegistry >();
	assert( pRegistry != nullptr );
	auto & registry = (*pRegistry);

	auto pDefinition = registry.getDefinition( pythonObject );
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
		new DefinedInstance( context, pythonObject, pDefinition ) );
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


const PyScript::ScriptObject & DefinedInstance::pythonObject() const
{
	return pythonObject_;
}


IBasePropertyPtr DefinedInstance::addProperty( const char * name,
	const TypeId & typeId,
	const MetaBase * pMetaBase )
{
	// TODO NGT-1247 support adding new attributes
	// Only supporting existing ones at the moment
	// Adding a property here would add it to properties_ instead of the
	// Python object
	assert( false && "The method or operation is not implemented." );
	return nullptr;
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
