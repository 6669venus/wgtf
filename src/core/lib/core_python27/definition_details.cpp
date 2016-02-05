#include "pch.hpp"

#include "definition_details.hpp"
#include "defined_instance.hpp"
#include "property.hpp"

#include "core_dependency_system/depends.hpp"
#include "core_reflection/interfaces/i_class_definition_modifier.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_reflection/property_accessor.hpp"
#include "wg_pyscript/py_script_object.hpp"
#include "core_reflection/property_storage.hpp"
#include "core_reflection/base_property_with_metadata.hpp"

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
MetaHandle extractMetaData( const char * name,
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
		return MetaHidden();
	}


	assert( name != nullptr );
	assert( strlen( name ) > 0 );
	if (name[0] == '_')
	{
		// Members that start with an underscore are private
		// Mark it as hidden
		return MetaHidden();
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
		NGT_WARNING_MSG( "MetaNone not supported, just leave entry blank.\n" );
		return nullptr;
	}
	else if (strcmp( metaTypeString.c_str(), "MetaNoNull" ) == 0)
	{
		return MetaNoNull();
	}
	else if (strcmp( metaTypeString.c_str(), "MetaColor" ) == 0)
	{
		return MetaColor();
	}
	else if (strcmp( metaTypeString.c_str(), "MetaSlider" ) == 0)
	{
		return MetaMinMax( 0.0f, 5.0f ) + MetaStepSize( 1.0f ) + MetaDecimals( 1 ) + MetaSlider();
	}
	else if (strcmp( metaTypeString.c_str(), "MetaHidden" ) == 0)
	{
		return MetaHidden();
	}
	else if (strcmp( metaTypeString.c_str(), "MetaReadOnly" ) == 0)
	{
		return MetaReadOnly();
	}
	else if (strcmp( metaTypeString.c_str(), "MetaNoSerialization" ) == 0)
	{
		return MetaNoSerialization();
	}
	else if (strcmp( metaTypeString.c_str(), "MetaOnStack" ) == 0)
	{
		return MetaOnStack();
	}

	return nullptr;
}

} // namespace


namespace ReflectedPython
{

class PropertyIterator : public PropertyIteratorImplBase
{
public:	
	PropertyIterator( IComponentContext & context, const PyScript::ScriptObject& pythonObject )
		: context_( context )
		, object_( pythonObject )
	{
		if (object_.get() == nullptr)
		{
			return;
		}

		const char * metaDataName = "_metaData";
		const auto metaDataAttribute = pythonObject.getAttribute( metaDataName,
			PyScript::ScriptErrorClear() );
		metaData_ = PyScript::ScriptDict::create( metaDataAttribute );

		// Get a list of strings appropriate for object arguments
		PyScript::ScriptObject dir = object_.getDir( PyScript::ScriptErrorPrint() );
		if (dir.get() != nullptr)
		{
			iterator_ = dir.getIter( PyScript::ScriptErrorPrint() );
		}
	}

	IBasePropertyPtr current() const
	{
		return current_;
	}

	bool next()
	{
		current_.reset();

		if (iterator_.get() == nullptr)
		{
			return false;
		}

		while (PyScript::ScriptObject key = iterator_.next())
		{
			PyScript::ScriptString str = key.str( PyScript::ScriptErrorPrint() );
			const char * name = str.c_str();

			// Some properties from dir are not accessible as attributes
			// e.g. __abstractmethods__ is a descriptor
			if (!object_.hasAttribute( name ))
			{
				continue;
			}

			auto meta = extractMetaData( name, metaData_ );
			IBasePropertyPtr property = std::make_shared< ReflectedPython::Property >( context_, name, object_ );

			current_ = meta != nullptr ?
				std::make_shared< BasePropertyWithMetaData >( property, meta ) : property;
			return true;
		}

		return false;
	}

private:
	IComponentContext &		context_;
	PyScript::ScriptObject	object_;
	PyScript::ScriptDict	metaData_;
	PyScript::ScriptIter	iterator_;
	IBasePropertyPtr		current_;
};

class DefinitionDetails::Implementation
{
public:
	Implementation( IComponentContext & context,
		const PyScript::ScriptObject & pythonObject );

	IComponentContext & context_;

	std::string name_;
	PyScript::ScriptObject pythonObject_;

	MetaHandle metaData_;
};


DefinitionDetails::Implementation::Implementation( IComponentContext & context,
	const PyScript::ScriptObject & pythonObject )
	: context_( context )
	, pythonObject_( pythonObject )
	, metaData_( MetaNone() )
{
}


DefinitionDetails::DefinitionDetails( IComponentContext & context,
	const PyScript::ScriptObject & pythonObject )
	: impl_( new Implementation( context, pythonObject ) )
{
	impl_->name_ = generateName( pythonObject );
	assert( !impl_->name_.empty() );
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

MetaHandle DefinitionDetails::getMetaData() const
{
	return impl_->metaData_;
}

ObjectHandle DefinitionDetails::create( const IClassDefinition & classDefinition ) const
{
	// Python definitions should be created based on a PyScript::PyObject

	// If this Python object is a type; create an instance of that type
	auto scriptType = PyScript::ScriptType::create( impl_->pythonObject_ );
	if (!scriptType.exists())
	{
		// If this Python object is an instance; clone the instance
		scriptType = PyScript::ScriptType::getType( impl_->pythonObject_ );
	}

	auto newPyObject = scriptType.genericAlloc( PyScript::ScriptErrorPrint() );
	if (newPyObject == nullptr)
	{
		return nullptr;
	}
	return DefinedInstance::create( impl_->context_,
		PyScript::ScriptObject( newPyObject,
			PyScript::ScriptObject::FROM_NEW_REFERENCE) );
}


void * DefinitionDetails::upCast( void * object ) const
{
	return nullptr;
}


PropertyIteratorImplPtr DefinitionDetails::getPropertyIterator() const
{
	return std::make_shared< PropertyIterator >( impl_->context_, impl_->pythonObject_ );
}


IClassDefinitionModifier * DefinitionDetails::getDefinitionModifier() const
{
	return const_cast< DefinitionDetails * >( this );
}


void DefinitionDetails::addProperty( const IBasePropertyPtr & reflectedProperty, MetaHandle metaData )
{
	// TODO NGT-1788
	assert( false && "Must use DefinedInstance::addProperty" );
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
