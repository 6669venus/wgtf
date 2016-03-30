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
std::weak_ptr< ReflectedPython::HookListener > g_listener_;


namespace ReflectedPython
{


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


/**
 *	Copied from typeobject.c.
 *	Calls a method on a *class/type*, but binds to the *instance*.
 *	E.g. looks up a method like Class.method()
 *	but then it binds to the instance:
 *	instance = Class()
 *	instance.method()
 */
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


/**
 *	Copied from typeobject.c.
 */
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


static wrapperbase g_wrappers[] =
{
	{
		"__setattr__", // name of wrapper
		offsetof( PyTypeObject, tp_setattro ), // offset to tp_setattro slot
		ReflectedPython::slot_tp_setattro, // function
		ReflectedPython::wrap_setattr, // wrapper for function
		PyDoc_STR( "x.__setattr__('name', value) <==> x.name = value" ), // doc for wrapper
		0, // flags
		nullptr // name_strobj
	},
	{ nullptr } // sentinel
};


const char * g_hookCountName = "__setattrHookCount";
PyObject * g_pyHookCountName = nullptr;

const char * g_originalSetattrName = "__originalSetattr";
PyObject * g_pyOriginalSetattrName = nullptr;
#endif // ENABLE_PYTHON_LISTENER_HOOKS


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


void attachListenerHooks( PyScript::ScriptObject & pythonObject )
{
#if ENABLE_PYTHON_LISTENER_HOOKS
	// __setattr__ hooks are added to the Python *type*, not the *instance*
	// So we must count how many reflected Python objects are using the type
	// Add an attribute to the object to track the number of reflected Python objects
	// Ignore errors if the attribute does not exist - first time hook is added
	auto pyType = pythonObject.get()->ob_type;

	// Do this first time
	if (g_pyHookCountName == nullptr)
	{
		g_pyHookCountName = PyString_InternFromString( g_hookCountName );
		if (g_pyHookCountName == nullptr)
		{
			Py_FatalError( "Out of memory interning listener hook names" );
			return;
		}
	}
	// Do this first time
	if (g_pyOriginalSetattrName == nullptr)
	{
		g_pyOriginalSetattrName = PyString_InternFromString( g_originalSetattrName );
		if (g_pyOriginalSetattrName == nullptr)
		{
			Py_FatalError( "Out of memory interning listener hook names" );
			return;
		}
	}

	auto typeObject = PyScript::ScriptType::getType( pythonObject );
	assert( typeObject.exists() );
	auto hookCountObject = typeObject.getAttribute( g_hookCountName,
		PyScript::ScriptErrorClear() );
	if (hookCountObject.exists())
	{
		// Already hooked, increment count
		auto hookCountLong = PyScript::ScriptLong::create( hookCountObject );
		assert( hookCountLong.exists() );
		if (!hookCountLong.exists())
		{
			auto hookCount = hookCountLong.asLong();
			++hookCount;

			hookCountLong = PyScript::ScriptLong::create( hookCount );
			assert( hookCountLong.exists() );
			assert( hookCountLong.asLong() > 0 );
			const auto setResult =
				PyDict_SetItem( pyType->tp_dict, g_pyHookCountName, hookCountLong.get() );
			PyScript::Script::printError();
			assert( setResult == 0 );

			return;
		}
	}

	// Add counter
	const long hookCount = 1;
	auto hookCountLong = PyScript::ScriptLong::create( hookCount );
	assert( hookCountLong.exists() );
	assert( hookCountLong.asLong() > 0 );
	const auto setResult1 =
		PyDict_SetItem( pyType->tp_dict, g_pyHookCountName, hookCountLong.get() );
	PyScript::Script::printError();
	assert( setResult1 == 0 );

	// -- From typeobject.c add_operators
	auto & wrapper = g_wrappers[ 0 ];
	if (wrapper.name_strobj == nullptr)
	{
		wrapper.name_strobj = PyString_InternFromString( wrapper.name );
		if (!wrapper.name_strobj)
		{
			Py_FatalError( "Out of memory interning listener hook names" );
			return;
		}
	}

	// Attach hook
	void * pFunctionToBeWrapped = &::PyObject_GenericSetAttr;
	auto pyWrapper = PyDescr_NewWrapper( pyType, &wrapper, pFunctionToBeWrapped );
	assert( pyWrapper != nullptr );

	auto pyOriginalEntry = PyDict_GetItem( pyType->tp_dict, wrapper.name_strobj );
	if (pyOriginalEntry != nullptr)
	{
		assert( g_pyOriginalSetattrName != nullptr );
		const auto setResult2 =
			PyDict_SetItem( pyType->tp_dict, g_pyOriginalSetattrName, pyOriginalEntry );
		PyScript::Script::printError();
		assert( setResult2 == 0 );
	}

	assert( wrapper.name_strobj != nullptr );
	const auto setResult3 =
		PyDict_SetItem( pyType->tp_dict, wrapper.name_strobj, pyWrapper );
	PyScript::Script::printError();
	assert( setResult3 == 0 );

	Py_DECREF( pyWrapper );

	PyType_Modified( pyType );

#endif // ENABLE_PYTHON_LISTENER_HOOKS
}


void detachListenerHooks( PyScript::ScriptObject & pythonObject )
{
#if ENABLE_PYTHON_LISTENER_HOOKS
	// __setattr__ hooks are added to the Python *type*, not the *instance*
	// So we must count how many reflected Python objects are using the type
	// Add an attribute to the object to track the number of reflected Python objects
	auto typeObject = PyScript::ScriptType::getType( pythonObject );
	auto pyType = pythonObject.get()->ob_type;

	auto hookCountObject = typeObject.getAttribute( g_hookCountName,
		PyScript::ScriptErrorClear() );
	if (!hookCountObject.exists())
	{
		// Not hooked
		// An error must have occured in attachListenerHooks()
		return;
	}

	// Decrement count
	auto hookCountLong = PyScript::ScriptLong::create( hookCountObject );
	assert( hookCountLong.exists() );
	if (!hookCountLong.exists())
	{
		return;
	}

	auto hookCount = hookCountLong.asLong();
	assert( hookCount > 0 );
	--hookCount;

	if (hookCount > 0)
	{
		// Still other reflected Python objects using this type
		// Update new count
		hookCountLong = PyScript::ScriptLong::create( hookCount );
		assert( hookCountLong.exists() );
		assert( hookCountLong.asLong() > 0 );
		const auto setResult =
			PyDict_SetItem( pyType->tp_dict, g_pyHookCountName, hookCountLong.get() );
		PyScript::Script::printError();
		assert( setResult == 0 );

		return;
	}

	auto & wrapper = g_wrappers[ 0 ];
	assert( wrapper.name_strobj != nullptr );
	//const auto removed1 = PyDict_DelItem( pyType->tp_dict, wrapper.name_strobj ) < 0;
	const auto pyOriginalEntry = PyDict_GetItem( pyType->tp_dict, g_pyOriginalSetattrName );
	if (pyOriginalEntry != nullptr)
	{
		const auto setResult =
			PyDict_SetItem( pyType->tp_dict, wrapper.name_strobj, pyOriginalEntry );
		PyScript::Script::printError();
		assert( setResult == 0 );

		assert( g_pyOriginalSetattrName != nullptr );
		const auto removeResult = PyDict_DelItem( pyType->tp_dict, g_pyOriginalSetattrName );
		PyScript::Script::printError();
		assert( removeResult == 0 );
	}
	else
	{
		const auto removeResult =
			PyDict_DelItem( pyType->tp_dict, wrapper.name_strobj );
		PyScript::Script::printError();
		assert( removeResult == 0 );
	}

	assert( g_pyHookCountName != nullptr );
	const auto removeResult = PyDict_DelItem( pyType->tp_dict, g_pyHookCountName );
	PyScript::Script::printError();
	assert( removeResult == 0 );

	// Restore old setattr
	PyType_Modified( pyType );
#endif // ENABLE_PYTHON_LISTENER_HOOKS
}


} // namespace ReflectedPython
