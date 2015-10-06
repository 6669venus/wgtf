#include "pch.hpp"
#include "definer.hpp"

#include "core_reflection/interfaces/i_class_definition.hpp"

Definer::Definer( IDefinitionManager & definitionManager,
	PyScript::ScriptObject& pythonObject )
	: pythonObject_( pythonObject )
	, pDefinition_( nullptr )
{
	PyScript::ScriptErrorPrint errorHandler;
	PyScript::ScriptString scriptString = pythonObject.str( errorHandler );
	const char* classDefinitionName = scriptString.c_str();

	auto definitionDetails = 
		definitionManager.createGenericDefinition( classDefinitionName );
	pDefinition_ = definitionManager.registerDefinition( definitionDetails );

	// TODO NGT-1160
	// Add attributes to definition
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
