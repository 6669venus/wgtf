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
static PyObject *
wrap_setattr(PyObject *self, PyObject *args, void *wrapped)
{
	PyObject *name, *value;

	if (!PyArg_UnpackTuple(args, "", 2, 2, &name, &value))
		return NULL;
	assert( wrapped != nullptr );

	auto selfObject = PyScript::ScriptObject( self,
		PyScript::ScriptObject::FROM_BORROWED_REFERENCE );
	auto typeObject = PyScript::ScriptType::getType( selfObject );

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
		setattrofunc func = (setattrofunc)wrapped;
		const int res = (*func)(self, name, value);
		if (res < 0)
		{
			return NULL;
		}
		Py_RETURN_NONE;
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
		setattrofunc func = (setattrofunc)wrapped;
		const int res = (*func)(self, name, value);
		if (res < 0)
		{
			return NULL;
		}
		Py_RETURN_NONE;
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
		return nullptr;
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
	setattrofunc func = (setattrofunc)wrapped;
	const int res = (*func)(self, name, value);

	// -- Post-notify UI
	for (auto it = itBegin; it != itEnd; ++it)
	{
		auto listener = it->lock();
		assert( listener != nullptr );
		listener->postSetValue( propertyAccessor, variantValue );
	}

	if (res < 0)
	{
		return NULL;
	}
	Py_RETURN_NONE;
}
#endif // ENABLE_PYTHON_LISTENER_HOOKS


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


static PyObject *
call_method(PyObject *o, char *name, PyObject **nameobj, char *format, ...)
{
	va_list va;
	PyObject *args, *func = 0, *retval;
	va_start(va, format);

	func = _PyObject_LookupSpecial(o, name, nameobj);
	if (func == NULL) {
		va_end(va);
		if (!PyErr_Occurred())
			PyErr_SetObject(PyExc_AttributeError, *nameobj);
		return NULL;
	}

	if (format && *format)
		args = Py_VaBuildValue(format, va);
	else
		args = PyTuple_New(0);

	va_end(va);

	if (args == NULL)
		return NULL;

	assert(PyTuple_Check(args));
	retval = PyObject_Call(func, args, NULL);

	Py_DECREF(args);
	Py_DECREF(func);

	return retval;
}


static int
slot_tp_setattro(PyObject *self, PyObject *name, PyObject *value)
{
	PyObject *res;
	static PyObject *delattr_str, *setattr_str;

	if (value == NULL)
		res = call_method(self, "__delattr__", &delattr_str,
						  "(O)", name);
	else
		res = call_method(self, "__setattr__", &setattr_str,
						  "(OO)", name, value);
	if (res == NULL)
		return -1;
	Py_DECREF(res);
	return 0;
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

	auto typeObject = PyScript::ScriptType::getType( pythonObject );
	assert( typeObject.exists() );
	auto foundIt = hookLookup.find( typeObject );
	if (foundIt != hookLookup.end())
	{
		// Already hooked
		HookInfo & hookInfo = (*foundIt).second;
		++hookInfo.hookCount_;
		return;
	}

	// Add counter
	auto emplaced = hookLookup.emplace( typeObject, HookInfo() );
	HookInfo & hookInfo = emplaced.first->second;

	// Save old setattr
	hookInfo.hookCount_ = 1;

	// -- From typeobject.c add_operators
	hookInfo.wrapper_.name = "__setattr__"; // name of wrapper
	hookInfo.wrapper_.offset = offsetof( PyTypeObject, tp_setattro ); // offset to tp_setattro slot
	hookInfo.wrapper_.function = slot_tp_setattro;//pyType->tp_setattro; // default function ??
	hookInfo.wrapper_.wrapper = wrap_setattr; // wrapper for function
	hookInfo.wrapper_.doc = PyDoc_STR( "x.__setattr__('name', value) <==> x.name = value" ); // doc for wrapper
	hookInfo.wrapper_.flags = 0; // flags
	hookInfo.wrapper_.name_strobj = nullptr; // name_strobj
	if (hookInfo.wrapper_.name_strobj == nullptr)
	{
		hookInfo.wrapper_.name_strobj = PyString_InternFromString( hookInfo.wrapper_.name );
		if (!hookInfo.wrapper_.name_strobj)
		{
			Py_FatalError( "Out of memory interning listener hook names" );
			return;
		}
	}

	// Attach hook
	//auto pyOriginalEntry = PyDict_GetItem( pyType->tp_dict, hookInfo.wrapper_.name_strobj );


	char * ptr = (char *)pyType;
	if (ptr != NULL)
	{
		ptr += hookInfo.wrapper_.offset;
	}
	void * pFunctionToBeWrapped = &PyObject_GenericSetAttr;
	//void ** pFunctionToBeWrapped = (void**)ptr;
	auto pyWrapper = PyDescr_NewWrapper( pyType, &hookInfo.wrapper_, pFunctionToBeWrapped );
	auto pyDescr = (PyWrapperDescrObject *)pyWrapper;
	assert( pyWrapper != nullptr );

	if (PyDict_SetItem( pyType->tp_dict, hookInfo.wrapper_.name_strobj, pyWrapper ) < 0)
	{
		return;
	}

	//assert( pyType->tp_setattro != pySetattrHook );
	Py_DECREF( pyWrapper );

	//auto pyNewEntry = PyDict_GetItem( pyType->tp_dict, hookInfo.wrapper_.name_strobj );

	// -- From typeobject.c update_one_slot
	//pyType->tp_setattro = (setattrofunc)hookInfo.wrapper_.function;

	//assert( pyType->tp_setattro != pySetattrHook );
	//pyType->tp_setattro = pySetattrHook;
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
	HookInfo & hookInfo = (*foundIt).second;
	auto & hookCount = hookInfo.hookCount_;
	assert( hookCount > 0 );
	--hookCount;

	if (hookCount > 0)
	{
		// Still other reflected Python objects using this type
		return;
	}

	const auto removed = PyDict_DelItem( pyType->tp_dict, hookInfo.wrapper_.name_strobj ) < 0;
	//assert( removed );

	// Restore old setattr
	//pyType->tp_setattro = (setattrofunc)hookInfo.wrapper_.function;
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

		const auto removed = PyDict_DelItem( pyType->tp_dict, itr->second.wrapper_.name_strobj ) < 0;
		//assert( removed );

		//pyType->tp_setattro = itr->second.defaultHook_;
		PyType_Modified( pyType );
	}
	hookLookup.clear();
#endif // ENABLE_PYTHON_LISTENER_HOOKS
}


} // namespace ReflectedPython
