#include "pch.hpp"
#include "defined_instance.hpp"
#include "definition_details.hpp"

#include "core_reflection/interfaces/i_class_definition.hpp"
#include "metadata/defined_instance.mpp"


namespace ReflectedPython
{


DefinedInstance::DefinedInstance()
	: BaseGenericObject()
	, pythonObject_( nullptr )
	, pDefinition_( nullptr )
{
	assert( false && "Always construct with a Python object" );
}


DefinedInstance::DefinedInstance( IDefinitionManager& definitionManager,
	PyScript::ScriptObject& pythonObject,
	const PythonTypeConverters & typeConverters )
	: BaseGenericObject()
	, pythonObject_( pythonObject )
	, pDefinition_( nullptr )
{
	pDefinition_ = definitionManager.registerDefinition(
		new DefinitionDetails( definitionManager,
			pythonObject,
			typeConverters ) );
	assert( pDefinition_ != nullptr );
}


DefinedInstance::~DefinedInstance()
{
	if (pDefinition_ != nullptr)
	{
		IDefinitionManager * pDefinitionManager =
			pDefinition_->getDefinitionManager();
		assert( pDefinitionManager != nullptr );
		pDefinitionManager->deregisterDefinition( pDefinition_ );
	}
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
