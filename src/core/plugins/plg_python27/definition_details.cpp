#include "pch.hpp"
#include "definition_details.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_reflection/property_accessor.hpp"

namespace
{

/**
 *	Get attributes from the Python object and add them to the definition.
 */
void extractAttributes( PyScript::ScriptObject& pythonObject,
	std::vector< Property >& properties )
{
	if (pythonObject.get() == nullptr)
	{
		return;
	}

	// Get a list of strings appropriate for object arguments
	PyScript::ScriptErrorPrint errorHandler;
	PyScript::ScriptObject dir = pythonObject.getDir( errorHandler );
	if (dir.get() == nullptr)
	{
		return;
	}
	PyScript::ScriptIter iter = dir.getIter( errorHandler );
	if (iter.get() == nullptr)
	{
		return;
	}

	while (PyScript::ScriptObject item = iter.next())
	{
		// Add property to definition
		// TODO NGT-1051 only adding name for now
		PyScript::ScriptString str = item.str( errorHandler );
		const char* name = str.c_str();

		properties.emplace_back( Property( name ) );
	}
}

} // namespace

DefinitionDetails::DefinitionDetails( IDefinitionManager & definitionManager,
	PyScript::ScriptObject& pythonObject )
	: pythonObject_( pythonObject )
	, metaData_( &MetaNone() )
{
	extractAttributes( pythonObject, attributes_ );
}

void DefinitionDetails::init( IClassDefinitionModifier & collection )
{
}

bool DefinitionDetails::isAbstract() const
{
	return false;
}

bool DefinitionDetails::isGeneric() const
{
	return true;
}

const char * DefinitionDetails::getName() const
{
	PyScript::ScriptErrorPrint errorHandler;

	// Note: this will make a unique class definition name per instance,
	// not per type
	PyScript::ScriptString scriptString = pythonObject_.str( errorHandler );
	const char* classDefinitionName = scriptString.c_str();

	return classDefinitionName;
}

const char * DefinitionDetails::getParentName() const
{
	assert( false && "The method or operation is not implemented." );
	return nullptr;
}

const MetaBase * DefinitionDetails::getMetaData() const
{
	return metaData_.get();
}

ObjectHandle DefinitionDetails::createBaseProvider( const ReflectedPolyStruct & ) const
{
	assert( false && "The method or operation is not implemented." );
	return ObjectHandle( nullptr );
}

ObjectHandle DefinitionDetails::createBaseProvider(
	const IClassDefinition & classDefinition,
	const void * pThis ) const
{
	assert( false && "The method or operation is not implemented." );
	return ObjectHandle( nullptr );
}

ObjectHandle DefinitionDetails::create( const IClassDefinition & classDefinition ) const
{
	assert( false && "The method or operation is not implemented." );
	return ObjectHandle( nullptr );
}

IClassDefinitionDetails::CastHelperCache *
DefinitionDetails::getCastHelperCache() const
{
	return &castHelperCache_;
}

void * DefinitionDetails::upCast( void * object ) const
{
	return nullptr;
}

