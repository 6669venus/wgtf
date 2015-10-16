#include "pch.hpp"
#include "defined_instance.hpp"

#include "core_reflection/interfaces/i_class_definition.hpp"
#include "core_reflection/property_accessor.hpp"
#include "metadata/defined_instance.mpp"


namespace ReflectedPython
{


DefinedInstance::DefinedInstance()
	: DefinitionProvider()
	, pythonObject_( nullptr )
	, pDefinition_( nullptr )
{
	assert( false && "Always construct with a Python object" );
}


DefinedInstance::DefinedInstance( IDefinitionManager& definitionManager,
	PyScript::ScriptObject& pythonObject )
	: DefinitionProvider()
	, pythonObject_( pythonObject )
	, pDefinition_( nullptr )
{
	pDefinition_ = definitionManager.registerDefinition(
		new DefinitionDetails( definitionManager, pythonObject ) );
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


bool DefinedInstance::set( const char * name, Variant & value )
{
	return this->setProperty( name,
		value.type()->typeId(),
		value );
}


Variant DefinedInstance::getProperty( const char * name ) const
{
	const IClassDefinition & definition = this->getDefinition();
	ObjectHandle provider( this, &definition );
	PropertyAccessor accessor = definition.bindProperty( name, provider );
	if (!accessor.isValid())
	{
		// TODO NGT-1161
		// Once this is working, I can combine the common part of
		// DefinedInstance and GenericObject
		//assert( false && "Property could not be found" );
		return Variant();
	}
	Property * property =
		static_cast< Property * >( accessor.getProperty() );
	auto pDefinitionManager = definition.getDefinitionManager();
	assert( pDefinitionManager != nullptr );
	return property->get( provider, (*pDefinitionManager) );
}


bool DefinedInstance::setProperty( const char * name,
	const TypeId & typeId,
	Variant & value ) const
{
	// TODO NGT-1162
	return false;
}


} // namespace ReflectedPython
