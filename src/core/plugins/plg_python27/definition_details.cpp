#include "pch.hpp"
#include "definition_details.hpp"
#include "core_reflection/interfaces/i_class_definition_modifier.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_reflection/property_accessor.hpp"

namespace
{

/**
 *	Get attributes from the Python object and add them to the definition.
 */
void extractAttributes( PyScript::ScriptObject& pythonObject,
	IClassDefinitionModifier & collection )
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

	// Add each attribute to the definition
	while (PyScript::ScriptObject key = iter.next())
	{
		// Get the name of the attribute
		PyScript::ScriptString str = key.str( errorHandler );
		const char * name = str.c_str();

		// Get the attribute
		PyScript::ScriptObject attribute = pythonObject.getAttribute( name,
			errorHandler );
		assert( attribute.exists() );
		if (!attribute.exists())
		{
			continue;
		}

		// Add to list of properties
		// TODO NGT-1255 do not add meta data
		collection.addProperty(
			new ReflectedPython::Property( name, attribute ),
			nullptr ); //&MetaNone() );
	}
}

} // namespace


namespace ReflectedPython
{


DefinitionDetails::DefinitionDetails( IDefinitionManager & definitionManager,
	PyScript::ScriptObject& pythonObject )
	: pythonObject_( pythonObject )
	, metaData_( &MetaNone() )
{
	// Extract name
	{
		PyScript::ScriptErrorPrint errorHandler;

		// Note: this will make a unique class definition name per instance,
		// not per type
		PyScript::ScriptString scriptString = pythonObject_.str( errorHandler );
		const char* classDefinitionName = scriptString.c_str();

		// Copy our own reference to the string
		name_ = classDefinitionName;
		assert( !name_.empty() );
	}
}

void DefinitionDetails::init( IClassDefinitionModifier & collection )
{
	// TODO get properties dynamically instead of building the list statically
	extractAttributes( pythonObject_, collection );
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
	return name_.c_str();
}

const char * DefinitionDetails::getParentName() const
{
	// TODO NGT-1225 inheritance not implemented
	// All new-style Python classes inherit from 'object'
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
	// Python definitions should be created based on a PyScript::PyObject
	// Do not create definitions which do not have an instance
	assert( false && "Do not use this function" );
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


} // namespace ReflectedPython

