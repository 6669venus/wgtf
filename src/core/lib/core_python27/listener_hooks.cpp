#include "pch.hpp"

#include "listener_hooks.hpp"

#include "defined_instance.hpp"

#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_reflection/interfaces/i_class_definition.hpp"
#include "core_script/type_converter_queue.hpp"
#include "core_reflection/property_accessor.hpp"
#include "type_converters/converters.hpp"
#include "wg_pyscript/py_script_object.hpp"


IComponentContext * g_pHookContext = nullptr;
ReflectedPython::HookLookup * g_pHookLookup_ = nullptr;
std::weak_ptr< ReflectedPython::HookListener > g_listener_;


/**
 *	Hook for listening to property changes.
 *
 *	Python functions should stay in the global namespace.
 *
 *	@param self Python object.
 *	@param name of attribute to be set.
 *		May add a new attribute if one does not exist.
 *	@param value to be set on the attribute.
 *	@return -1 on error.
 */
int pySetattrHook( PyObject * self,
	PyObject * name,
	PyObject * value )
{
	// -- Check arguments
	if (name == nullptr)
	{
		PyErr_Format( PyExc_AttributeError,
			"Cannot set attribute with no name." );
		return -1;
	}
	if (value == nullptr)
	{
		PyErr_Format( PyExc_AttributeError,
			"Cannot set attribute with no value." );
		return -1;
	}

	// -- Get default hook
	auto selfObject = PyScript::ScriptObject( self,
		PyScript::ScriptObject::FROM_BORROWED_REFERENCE );
	auto typeObject = PyScript::ScriptType::getType( selfObject );

	assert( g_pHookLookup_ != nullptr );
	auto & hookLookup = (*g_pHookLookup_);

	// TODO would be better to use a descriptor/wrapper instead of storing
	// the default hook in a map because it means a map search for each
	// attribute set
	auto foundIt = hookLookup.find( typeObject );
	assert( foundIt != hookLookup.end() );
	auto & defaultHook = foundIt->second.defaultHook_;
	assert( defaultHook != nullptr );

	// -- Check if listeners do not need to be notified
	// If the HookListener has already been entered, then this property must
	// have been set by the command system and we do not need to fire another
	// notification.
	// If it has not been entered, then this property must have been set by
	// a script.
	assert( !g_listener_.expired() );
	auto listener = g_listener_.lock();
	if (listener->entered())
	{
		// -- Set attribute using default hook
		// __setattr__( name, value )
		defaultHook( self, name, value );
		return 0;
	}

	// -- Check if listeners need to be notified
	assert( g_pHookContext != nullptr );
	auto & context = (*g_pHookContext);
	auto handle = ReflectedPython::DefinedInstance::find( context, selfObject );

	// Item is not in reflection system, do not need to send notification
	if (!handle.isValid())
	{
		// -- Set attribute using default hook
		// __setattr__( name, value )
		defaultHook( self, name, value );
		return 0;
	}

	// -- Pre-notify UI
	// TODO support preItemsInserted/postItemsInserted
	auto tmpName = PyScript::ScriptObject( name,
		PyScript::ScriptObject::FROM_BORROWED_REFERENCE );
	auto nameObject = PyScript::ScriptString::create( tmpName );
	if (!nameObject.exists())
	{
		PyErr_Format( PyExc_AttributeError,
			"Cannot set attribute with no name." );
		return -1;
	}
	auto valueObject = PyScript::ScriptObject( value,
		PyScript::ScriptObject::FROM_BORROWED_REFERENCE );


	auto pDefinedInstance = handle.getBase< ReflectedPython::DefinedInstance >();
	assert( pDefinedInstance != nullptr );
	auto & definedInstance = (*pDefinedInstance);

	auto pTypeConverters = context.queryInterface< PythonType::Converters >();
	assert( pTypeConverters != nullptr );

	auto pDefinitionManager = context.queryInterface< IDefinitionManager >();
	assert( pTypeConverters != nullptr );

	// Get root object to construct PropertyAccessor.
	// PropertyAccessorListener requires a (root object, full path) pair to
	// detect changes.
	const auto & rootInstance = definedInstance.root();

	// Find existing ObjectHandle corresponding to instance
	auto rootHandle = ReflectedPython::DefinedInstance::find( context,
		rootInstance.pythonObject() );

	const std::string childPath( nameObject.c_str() );
	std::string fullPath( definedInstance.fullPath() );
	if (!fullPath.empty())
	{
		fullPath += DOT_OPERATOR;
	}
	fullPath += childPath;

	auto pDefinition = rootInstance.getDefinition();
	auto propertyAccessor = pDefinition->bindProperty( fullPath.c_str(), rootHandle );

	Variant variantValue;
	const bool success = pTypeConverters->toVariant( valueObject,
		variantValue,
		rootInstance,
		childPath );
	assert( success );

	auto& listeners = pDefinitionManager->getPropertyAccessorListeners();
	const auto itBegin = listeners.cbegin();
	const auto itEnd = listeners.cend();
	for (auto it = itBegin; it != itEnd; ++it)
	{
		auto listener = it->lock();
		assert( listener != nullptr );
		listener->preSetValue( propertyAccessor, variantValue );
	}

	// -- Set attribute using default hook
	// __setattr__( name, value )
	defaultHook( self, name, value );

	// -- Post-notify UI
	for (auto it = itBegin; it != itEnd; ++it)
	{
		auto listener = it->lock();
		assert( listener != nullptr );
		listener->postSetValue( propertyAccessor, variantValue );
	}

	return 0;
}


namespace ReflectedPython
{


HookListener::HookListener()
	: entered_( 0 )
{
}


void HookListener::preSetValue( const PropertyAccessor & accessor,
	const Variant & value ) /*override*/
{
	++entered_;
}


void HookListener::postSetValue( const PropertyAccessor & accessor,
	const Variant & value ) /*override*/
{
	--entered_;
}


bool HookListener::entered() const
{
	return (entered_ > 0);
}


void attachListenerHooks( PyScript::ScriptObject & pythonObject,
	HookLookup & hookLookup )
{
	// __setattr__ hooks are added to the Python *type*, not the *instance*
	// So we must count how many reflected Python objects are using the type
	// Add an attribute to the object to track the number of reflected Python objects
	// Ignore errors if the attribute does not exist - first time hook is added
	auto typeObject = PyScript::ScriptType::getType( pythonObject );
	auto pyType = pythonObject.get()->ob_type;
	assert( pyType != nullptr );
	{
		auto foundIt = hookLookup.find( typeObject );
		if (foundIt != hookLookup.end())
		{
			// Already hooked
			return;
		}

		// Add counter
		// Save old setattr
		HookInfo hookInfo;
		hookInfo.hookCount_ = 1;
		hookInfo.defaultHook_ = pyType->tp_setattro;
		assert( hookInfo.defaultHook_ != nullptr );
		hookLookup[ typeObject ] = hookInfo;
	}

	// Attach hook
	// TODO work out a way to use a wrapper instead?
	// PyObject_GenericSetAttr?
	assert( pyType->tp_setattro != pySetattrHook );
	pyType->tp_setattro = pySetattrHook;
	PyType_Modified( pyType );

	//// Construct new hook
	//const char * SETATTR_NAME = "__setattr__";
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


void detachListenerHooks( PyScript::ScriptObject & pythonObject,
	HookLookup & hookLookup )
{
	// __setattr__ hooks are added to the Python *type*, not the *instance*
	// So we must count how many reflected Python objects are using the type
	// Add an attribute to the object to track the number of reflected Python objects
	auto typeObject = PyScript::ScriptType::getType( pythonObject );
	auto pyType = pythonObject.get()->ob_type;

	auto foundIt = hookLookup.find( typeObject );
	if (foundIt == hookLookup.end())
	{
		// Not hooked
		// An error must have occured in attachListenerHooks()
		return;
	}

	// Decrement count
	auto & hookCount = foundIt->second.hookCount_;
	assert( hookCount > 0 );
	--hookCount;

	if (hookCount > 0)
	{
		// Still other reflected Python objects using this type
		return;
	}

	// Restore old setattr
	pyType->tp_setattro = foundIt->second.defaultHook_;
	PyType_Modified( pyType );

	// Remove from map
	hookLookup.erase( foundIt );
}


void cleanupListenerHooks( HookLookup & hookLookup )
{
	// Remove reflection system hooks for any leaks
	for (auto itr = std::begin( hookLookup ); itr != std::end( hookLookup ); ++itr)
	{
		auto & type = itr->first;
		auto pyType = reinterpret_cast< PyTypeObject * >( type.get() );
		if (pyType == nullptr)
		{
			continue;
		}

		pyType->tp_setattro = itr->second.defaultHook_;
		PyType_Modified( pyType );
	}
}


} // namespace ReflectedPython
