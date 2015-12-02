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

	// Find metadata
	// Clear errors if it was not found
	const char * metaDataName = "_metaData";
	const auto metaDataAttribute = pythonObject.getAttribute( metaDataName,
		PyScript::ScriptErrorClear() );
	const auto metaData = PyScript::ScriptDict::create( metaDataAttribute );
	// TODO NGT-1255 do not add meta data for function types
	MetaBase * pMetaBase = nullptr; //&MetaNone();

	// Add each attribute to the definition
	while (PyScript::ScriptObject key = iter.next())
	{
		// Get the name of the attribute
		PyScript::ScriptString str = key.str( errorHandler );
		const char * name = str.c_str();
		
		// Only filter attributes for objects that have metadata.
		// So that it's backward-compatible with objects that haven't had
		// metadata added yet.
		if (metaData.exists())
		{
			auto metaItem = metaData.getItem( name, errorHandler );
			if (!metaItem.exists())
			{
				// Attribute is hidden
				// TODO NGT-1255 do not add meta data
				//pMetaBase = &MetaHidden();
			}
			else
			{
				auto metaTypeString = PyScript::ScriptString::create( metaItem );

				// Metadata should always be of the format
				// { "attribute" : "string" }
				// TODO use an enum instead of strings
				assert( metaTypeString );

				// Convert Python metadata to C++ metadata
				// TODO support all MetaBase types
				if (strcmp( metaTypeString.c_str(), "MetaSlider" ) == 0)
				{
					// TODO NGT-1255 do not add meta data
					//pMetaBase = &MetaSlider();
				}
			}
		}

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
