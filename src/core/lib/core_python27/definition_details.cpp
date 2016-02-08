#include "pch.hpp"

#include "definition_details.hpp"
#include "defined_instance.hpp"
#include "listener_hooks.hpp"
#include "property.hpp"

#include "core_dependency_system/depends.hpp"
#include "core_reflection/interfaces/i_class_definition_modifier.hpp"
#include "core_reflection/property_accessor.hpp"
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
		return nullptr;
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


const char * SETATTR_NAME = "__setattr__";

struct HookInfo
{
	size_t hookCount;
	setattrofunc oldHook;
};

/**
 *	Key compare functor.
 *	Need to do a deep compare on PyScript::ScriptObject to prevent getting
 *	copies of the same object added to the map.
 */
class ScriptObjectCompare
{
public:
	bool operator()( const PyScript::ScriptObject & a,
		const PyScript::ScriptObject & b ) const
	{
		return a.compareTo( b, PyScript::ScriptErrorPrint() ) < 0;
	}
};
typedef std::map< PyScript::ScriptType, HookInfo, ScriptObjectCompare > HookLookup;
static HookLookup g_hooks;


// Add hooks for listening to setattr and delattr
// TODO delattr
void attachListenerHooks( PyScript::ScriptObject & pythonObject )
{
	// PyErr_PrintEx may want to set another attribute
	// Prevent recursion
	PyScript::ScriptErrorClear errorClear;

	// __setattr__ hooks are added to the Python *type*, not the *instance*
	// So we must count how many reflected Python objects are using the type
	// Add an attribute to the object to track the number of reflected Python objects
	// Ignore errors if the attribute does not exist - first time hook is added
	auto typeObject = PyScript::ScriptType::getType( pythonObject );
	auto pyType = pythonObject.get()->ob_type;
	assert( pyType != nullptr );
	{
		auto foundIt = g_hooks.find( typeObject );
		if (foundIt != g_hooks.end())
		{
			// Already hooked
			return;
		}

		// Add counter
		// Save old setattr
		HookInfo hookInfo;
		hookInfo.hookCount = 1;
		hookInfo.oldHook = pyType->tp_setattro;
		g_hooks[ typeObject ] = hookInfo;
	}

	// Choose hook
	setattrofunc hook = nullptr;
	if (PyScript::ScriptInstance::check( pythonObject ))
	{
		hook = reinterpret_cast< setattrofunc >( py_instance_setattr_hook );
	}
	// Anything that inherits from object
	else if (typeObject.isSubClass( PyBaseObject_Type, errorClear ))
	{
		hook = py_setattr_hook;
	}
	else
	{
		NGT_ERROR_MSG( "Unknown Python type %s\n", typeObject.str( errorClear ).c_str() );
		return;
	}


	// Attach hook
	// TODO work out a way to use a wrapper instead?
	// PyObject_GenericSetAttr?
	assert( pyType->tp_setattro != hook );
	pyType->tp_setattro = hook;
	PyType_Modified( pyType );

	//// Construct new hook
	//static PyMethodDef s_methods[] =
	//{
	//	{
	//		SETATTR_NAME,
	//		reinterpret_cast< PyCFunction >( &py_setattr_hook ),
	//		METH_VARARGS|METH_KEYWORDS,
	//		"Listener to notify the NGT Reflection System\n"
	//		"x.__setattr__('name', value) <==> x.name = value"
	//	},
	//	{ nullptr, nullptr, 0, nullptr }
	//};

	//auto pyFunction = PyCFunction_New( s_methods, pythonObject.get() );
	//auto functionObject = PyScript::ScriptObject( pyFunction,
	//	PyScript::ScriptObject::FROM_NEW_REFERENCE );

	//PyObject * self = nullptr;
	//auto pyMethod = PyMethod_New( pyFunction, self, typeObject.get() );
	//auto methodObject = PyScript::ScriptObject( pyMethod,
	//	PyScript::ScriptObject::FROM_NEW_REFERENCE );
}


void detachListenerHooks( PyScript::ScriptObject & pythonObject )
{
	// __setattr__ hooks are added to the Python *type*, not the *instance*
	// So we must count how many reflected Python objects are using the type
	// Add an attribute to the object to track the number of reflected Python objects
	auto typeObject = PyScript::ScriptType::getType( pythonObject );
	auto pyType = pythonObject.get()->ob_type;

	auto foundIt = g_hooks.find( typeObject );
	if (foundIt == g_hooks.end())
	{
		// Not hooked
		// An error must have occured in attachListenerHooks()
		return;
	}

	// Decrement count
	auto & hookCount = foundIt->second.hookCount;
	assert( hookCount > 0 );
	--hookCount;

	if (hookCount > 0)
	{
		// Still other reflected Python objects using this type
		return;
	}

	// Restore old setattr
	pyType->tp_setattro = foundIt->second.oldHook;

	// Remove from map
	g_hooks.erase( foundIt );
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


DefinitionDetails::DefinitionDetails( IComponentContext & context,
	const PyScript::ScriptObject & pythonObject )
	: context_( context )
	, name_( DefinitionDetails::generateName( pythonObject ) )
	, pythonObject_( pythonObject )
	, metaData_( MetaNone() )
{
	attachListenerHooks( pythonObject_ );
}


DefinitionDetails::~DefinitionDetails()
{
	detachListenerHooks( pythonObject_ );
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

MetaHandle DefinitionDetails::getMetaData() const
{
	return metaData_;
}

ObjectHandle DefinitionDetails::create( const IClassDefinition & classDefinition ) const
{
	// Python definitions should be created based on a PyScript::PyObject
	// Clone instance
	auto scriptType = PyScript::ScriptType::create( impl_->pythonObject_ );
		scriptType = PyScript::ScriptType::getType( impl_->pythonObject_ );
	auto newPyObject = scriptType.genericAlloc( PyScript::ScriptErrorPrint() );
	if (newPyObject == nullptr)
	{
		return nullptr;
	}
	return DefinedInstance::create( context_,
		PyScript::ScriptObject( newPyObject,
			PyScript::ScriptObject::FROM_NEW_REFERENCE) );
}


void * DefinitionDetails::upCast( void * object ) const
{
	return nullptr;
}


PropertyIteratorImplPtr DefinitionDetails::getPropertyIterator() const
{
	return std::make_shared< PropertyIterator >( impl_->context_,
}


IClassDefinitionModifier * DefinitionDetails::getDefinitionModifier() const
{
	return const_cast< DefinitionDetails * >( this );
}


void DefinitionDetails::addProperty( const IBasePropertyPtr & reflectedProperty, MetaHandle metaData )
{
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
