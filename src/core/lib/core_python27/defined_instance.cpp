#include "pch.hpp"
#include "defined_instance.hpp"

#include "core_generic_plugin/interfaces/i_component_context.hpp"
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


DefinedInstance::DefinedInstance( IComponentContext & context,
	const PyScript::ScriptObject & pythonObject )
	: BaseGenericObject()
	, pythonObject_( pythonObject )
	, pDefinition_( nullptr )
	, context_( &context )
{
	auto registry = context_->queryInterface<IScriptObjectDefinitionRegistry>();
	assert( registry != nullptr );
	pDefinition_ = registry->registerObject( pythonObject );
}


DefinedInstance::~DefinedInstance()
{
}


const IClassDefinition & DefinedInstance::getDefinition() const
{
	assert( pDefinition_ != nullptr );
	return (*pDefinition_);
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
	return ObjectHandleT< DefinedInstance >( this,
		&this->getDefinition() );
}


ObjectHandle DefinedInstance::getDerivedType()
{
	// MUST pass this as a pointer and NOT (*this) as a reference or
	// ObjectHandleT will make a copy
	return ObjectHandleT< DefinedInstance >( this,
		&this->getDefinition() );
}


} // namespace ReflectedPython
