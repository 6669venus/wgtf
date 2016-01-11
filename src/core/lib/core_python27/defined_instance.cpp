#include "pch.hpp"
#include "defined_instance.hpp"

#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "i_object_manager.hpp"
#include "core_reflection/interfaces/i_class_definition.hpp"
#include "metadata/defined_instance.mpp"
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
}


DefinedInstance::~DefinedInstance()
{
	auto pObjectManager = context_->queryInterface< IPythonObjectManager >();
	if (pObjectManager != nullptr)
	{
		pObjectManager->deregisterUnmanagedObject( this->getDefinition() );
	}
}


/* static */ ObjectHandle DefinedInstance::create( IComponentContext & context,
	const PyScript::ScriptObject & pythonObject )
{
	assert( pythonObject.exists() );

	// Get a definition that's the same for each ScriptObject instance
	auto pRegistry = context.queryInterface< IScriptObjectDefinitionRegistry >();
	assert( pRegistry != nullptr );
	auto pDefinition = pRegistry->getDefinition( pythonObject );
	assert( pDefinition != nullptr );
	auto & definition = (*pDefinition);

	// Search for an existing object handle that's using that definition

	auto pObjectManager = context.queryInterface< IPythonObjectManager >();
	assert( pObjectManager != nullptr );
	auto & objectManager = (*pObjectManager);
	auto handle = objectManager.getUnmanagedObject( definition );
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
	handle = objectManager.registerUnmanagedObject( definition, handle );
	assert( handle.isValid() );

	// Registered reference
	return handle;
}


const IClassDefinition & DefinedInstance::getDefinition() const
{
	assert( pDefinition_ != nullptr );
	return (*pDefinition_);
}


const PyScript::ScriptObject & DefinedInstance::pythonObject() const
{
	return pythonObject_;
}


IBaseProperty * DefinedInstance::addProperty( const char * name,
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
	// MUST pass this as a pointer and NOT (*this) as a reference or
	// ObjectHandleT will make a copy
	//return ObjectHandleT< DefinedInstance >( this,
	//	&this->getDefinition() );

	auto pObjectManager = context_->queryInterface< IPythonObjectManager >();
	assert( pObjectManager != nullptr );
	auto & objectManager = (*pObjectManager);
	return objectManager.getUnmanagedObject( this->getDefinition() );
}


ObjectHandle DefinedInstance::getDerivedType()
{
	// MUST pass this as a pointer and NOT (*this) as a reference or
	// ObjectHandleT will make a copy
	//return ObjectHandleT< DefinedInstance >( this,
	//	&this->getDefinition() );

	auto pObjectManager = context_->queryInterface< IPythonObjectManager >();
	assert( pObjectManager != nullptr );
	auto & objectManager = (*pObjectManager);
	return objectManager.getUnmanagedObject( this->getDefinition() );
}


} // namespace ReflectedPython
