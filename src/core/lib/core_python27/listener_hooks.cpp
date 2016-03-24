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


#if ENABLE_PYTHON_LISTENER_HOOKS
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
		return defaultHook( self, name, value );
	}

	// -- Check if listeners need to be notified
	assert( g_pHookContext != nullptr );
	auto & context = (*g_pHookContext);
	auto handle = ReflectedPython::DefinedInstance::find( context, selfObject );

	// selfObject should always be found in the reflection system
	// because the hook should be attached & detached when the object
	// enters & leaves the system.
	// But the setattr hook may be triggered by setting an attribute on
	// a reflected object's *type*. So just ignore this as the type is not
	// tracked by the reflection system.
	if (!handle.isValid())
	{
		// -- Set attribute using default hook
		// __setattr__( name, value )
		return defaultHook( self, name, value );
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
	assert( pDefinitionManager != nullptr );

#if ENABLE_FULL_PATH_PYTHON_LISTENER_HOOKS
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
	assert( pDefinition != nullptr );
	auto propertyAccessor = pDefinition->bindProperty( fullPath.c_str(), rootHandle );

#else // ENABLE_FULL_PATH_PYTHON_LISTENER_HOOKS

	const std::string childPath( nameObject.c_str() );
	auto pDefinition = definedInstance.getDefinition();
	assert( pDefinition != nullptr );
	auto propertyAccessor = pDefinition->bindProperty( childPath.c_str(), handle );
#endif // ENABLE_FULL_PATH_PYTHON_LISTENER_HOOKS

	Variant variantValue;
	const bool success = pTypeConverters->toVariant( valueObject,
		variantValue,
		selfObject,
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
	const auto result = defaultHook( self, name, value );

	// -- Post-notify UI
	for (auto it = itBegin; it != itEnd; ++it)
	{
		auto listener = it->lock();
		assert( listener != nullptr );
		listener->postSetValue( propertyAccessor, variantValue );
	}

	return result;
}
#endif // ENABLE_PYTHON_LISTENER_HOOKS


namespace ReflectedPython
{


bool ScriptObjectCompare::operator()( const PyScript::ScriptObject & a,
	const PyScript::ScriptObject & b ) const
{
	// Not safe to use ScriptErrorPrint() during comparison because it might
	// cause recursion into
	// ScriptObjectDefinitionRegistry::findOrCreateDefinition() or
	// ScriptObjectDefinitionRegistry::findDefinition().

	// Be careful using this as a comparator, if it gets an error comparing
	// 2 objects, then it does not guarantee correct ordering
	const auto result = a.compareTo( b, PyScript::ScriptErrorRetain() ) < 0;
	if (PyScript::Script::hasError())
	{
		// Error comparing objects, just compare pointer addresses
		PyScript::Script::clearError();
		return (a.get() < b.get());
	}
	return result;
}


//bool ScriptObjectEqualTo::operator()( const PyScript::ScriptObject & a,
//	const PyScript::ScriptObject & b ) const
//{
//	return a.compareTo( b, PyScript::ScriptErrorClear() ) == 0;
//}


//size_t ScriptObjectHash::operator()( const PyScript::ScriptObject & a ) const
//{
//	return (size_t)a.get();
//}


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
#if ENABLE_PYTHON_LISTENER_HOOKS
	// __setattr__ hooks are added to the Python *type*, not the *instance*
	// So we must count how many reflected Python objects are using the type
	// Add an attribute to the object to track the number of reflected Python objects
	// Ignore errors if the attribute does not exist - first time hook is added
	auto pyType = pythonObject.get()->ob_type;
	{
		auto typeObject = PyScript::ScriptType::getType( pythonObject );
		assert( typeObject.exists() );
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
		hookLookup.emplace( typeObject, hookInfo );
	}

	// Attach hook
	// TODO work out a way to use a descriptor/wrapper instead
	assert( pyType->tp_setattro != pySetattrHook );
	pyType->tp_setattro = pySetattrHook;
	PyType_Modified( pyType );
#endif // ENABLE_PYTHON_LISTENER_HOOKS
}


void detachListenerHooks( PyScript::ScriptObject & pythonObject,
	HookLookup & hookLookup )
{
#if ENABLE_PYTHON_LISTENER_HOOKS
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
#endif // ENABLE_PYTHON_LISTENER_HOOKS
}


void cleanupListenerHooks( HookLookup & hookLookup )
{
#if ENABLE_PYTHON_LISTENER_HOOKS
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
#endif // ENABLE_PYTHON_LISTENER_HOOKS
}


} // namespace ReflectedPython
