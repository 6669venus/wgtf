#include "pch.hpp"

#include "definition_details.hpp"
#include "property.hpp"

#include "core_dependency_system/depends.hpp"
#include "core_reflection/interfaces/i_class_definition_modifier.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_reflection/property_accessor.hpp"
#include "wg_pyscript/py_script_object.hpp"

namespace
{


/**
 *	Get metadata from class.
 *	@param name of attribute.
 *	@param metaData metadata found on Python class.
 *		Can be null.
 *	@return metadata or null.
 *		Caller is responsible for deleting metadata.
 */
MetaBase * extractMetaData( const char * name,
	const PyScript::ScriptDict & metaData )
{
	if (!metaData.exists())
	{
		// Class has no metadata
		return nullptr;
	}

	auto metaItem = metaData.getItem( name, PyScript::ScriptErrorClear() );
	if (!metaItem.exists())
	{
		// Class has metadata, but none for this attribute
		// Mark it as hidden
		return &MetaHidden();
	}


	assert( name != nullptr );
	assert( strlen( name ) > 0 );
	if (name[0] == '_')
	{
		// Members that start with an underscore are private
		// Mark it as hidden
		return &MetaHidden();
	}


	// Metadata should always be of the format
	// { "attribute" : "string" }
	// TODO NGT-1559 use a class instead of strings
	// TODO NGT-1559 support all MetaBase types
	auto metaTypeString = PyScript::ScriptString::create( metaItem );
	assert( metaTypeString );

	// Convert Python metadata to C++ metadata
	if (strcmp( metaTypeString.c_str(), "MetaNone" ) == 0)
	{
		return &MetaNone();
	}
	else if (strcmp( metaTypeString.c_str(), "MetaNoNull" ) == 0)
	{
		return &MetaNoNull();
	}
	else if (strcmp( metaTypeString.c_str(), "MetaColor" ) == 0)
	{
		return &MetaColor();
	}
	else if (strcmp( metaTypeString.c_str(), "MetaSlider" ) == 0)
	{
		return &MetaSlider();
	}
	else if (strcmp( metaTypeString.c_str(), "MetaHidden" ) == 0)
	{
		return &MetaHidden();
	}
	else if (strcmp( metaTypeString.c_str(), "MetaReadOnly" ) == 0)
	{
		return &MetaReadOnly();
	}
	else if (strcmp( metaTypeString.c_str(), "MetaNoSerialization" ) == 0)
	{
		return &MetaNoSerialization();
	}
	else if (strcmp( metaTypeString.c_str(), "MetaOnStack" ) == 0)
	{
		return &MetaOnStack();
	}

	return nullptr;
}


/**
 *	Get attributes from the Python object and add them to the definition.
 */
void extractAttributes( IComponentContext & context,
		const PyScript::ScriptObject& pythonObject,
	IClassDefinitionModifier & collection )
{
	if (pythonObject.get() == nullptr)
	{
		return;
	}

	// Get a list of strings appropriate for object arguments
	PyScript::ScriptObject dir = pythonObject.getDir( PyScript::ScriptErrorPrint() );
	if (dir.get() == nullptr)
	{
		return;
	}
	PyScript::ScriptIter iter = dir.getIter( PyScript::ScriptErrorPrint() );
	if (iter.get() == nullptr)
	{
		return;
	}

	// Find metadata
	// Clear errors if it was not found
	const char * metaDataName = "_metaData";
	const auto metaDataAttribute = pythonObject.getAttribute( metaDataName,
		PyScript::ScriptErrorClear() );
	const auto metaData = PyScript::ScriptDict::create( metaDataAttribute );

	// Add each attribute to the definition
	while (PyScript::ScriptObject key = iter.next())
	{
		// Get the name of the attribute
		PyScript::ScriptString str = key.str( PyScript::ScriptErrorPrint() );
		const char * name = str.c_str();

		// Some properties from dir are not accessible as attributes
		// e.g. __abstractmethods__ is a descriptor
		if (!pythonObject.hasAttribute( name ))
		{
			continue;
		}

		MetaBase * pMetaBase = extractMetaData( name, metaData );

		// Add to list of properties
		collection.addProperty(
			new ReflectedPython::Property( context, name, pythonObject ),
			pMetaBase );
	}
}

} // namespace


namespace ReflectedPython
{


class DefinitionDetails::Implementation
{
public:
	Implementation( IComponentContext & context,
		const PyScript::ScriptObject & pythonObject );

	IComponentContext & context_;

	std::string name_;
	PyScript::ScriptObject pythonObject_;

	std::unique_ptr< const MetaBase > metaData_;
	mutable IClassDefinitionDetails::CastHelperCache castHelperCache_;
};


DefinitionDetails::Implementation::Implementation( IComponentContext & context,
	const PyScript::ScriptObject & pythonObject )
	: context_( context )
	, pythonObject_( pythonObject )
	, metaData_( &MetaNone() )
{
}


DefinitionDetails::DefinitionDetails( IComponentContext & context,
	const PyScript::ScriptObject & pythonObject )
	: impl_( new Implementation( context, pythonObject ) )
{
	impl_->name_ = generateName( pythonObject );
	assert( !impl_->name_.empty() );
}

void DefinitionDetails::init( IClassDefinitionModifier & collection )
{
	// TODO get properties dynamically instead of building the list statically
	extractAttributes( impl_->context_, impl_->pythonObject_, collection );
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
	return impl_->name_.c_str();
}

const char * DefinitionDetails::getParentName() const
{
	// TODO NGT-1225 inheritance not implemented
	// All new-style Python classes inherit from 'object'
	return nullptr;
}

const MetaBase * DefinitionDetails::getMetaData() const
{
	return impl_->metaData_.get();
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
	return &impl_->castHelperCache_;
}

void * DefinitionDetails::upCast( void * object ) const
{
	return nullptr;
}


std::string DefinitionDetails::generateName( const PyScript::ScriptObject & object )
{
	PyScript::ScriptErrorPrint errorHandler;
	std::string typeName;

	if (PyScript::ScriptType::check( object ))
	{
		// Type type
		// type.__module__ + type.__name__
		PyScript::ScriptType scriptType(
			reinterpret_cast<PyTypeObject*>( object.get() ), PyScript::ScriptObject::FROM_BORROWED_REFERENCE );

		scriptType.getAttribute( "__module__", typeName, errorHandler );
		typeName += '.';
		typeName += scriptType.name();
	}
	else
	{
		// Class or None type
		// __module__ + __name__
		typeName = object.str( errorHandler ).c_str();
	}

	return typeName;
}


} // namespace ReflectedPython
