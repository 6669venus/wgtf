#include "pch.hpp"
#include "definer.hpp"

#include "core_reflection/interfaces/i_class_definition.hpp"

Definer::Definer( IDefinitionManager & definitionManager,
	PyScript::ScriptObject& pythonObject )
	: definitionManager_( definitionManager )
	, pythonObject_( pythonObject )
	, pDefinition_( nullptr )
{
	pDefinition_ = definitionManager_.registerDefinition(
		new DefinitionDetails( definitionManager, pythonObject ) );
	assert( pDefinition_ != nullptr );
}


Definer::~Definer()
{
	definitionManager_.deregisterDefinition( pDefinition_ );
}


const IClassDefinition & Definer::getDefinition() const
{
	assert( pDefinition_ != nullptr );
	return (*pDefinition_);
}


bool Definer::set( const char * name, Variant & value )
{
	return this->setProperty( name,
		value.type()->typeId(),
		value );
}


Variant Definer::getProperty( const char * name ) const
{
	// TODO NGT-1161
	return Variant();
}


bool Definer::setProperty( const char * name,
	const TypeId & typeId,
	Variant & value ) const
{
	// TODO NGT-1162
	return false;
}
