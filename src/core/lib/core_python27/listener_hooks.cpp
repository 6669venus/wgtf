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
std::weak_ptr< ReflectedPython::HookListener > g_listener;


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
static PyObject * wrap_setattr( PyObject * self, PyObject * args, void * wrapped )
{
	PyObject * name;
	PyObject * value;
	if (!PyArg_UnpackTuple( args, "", 2, 2, &name, &value ))
	{
		return nullptr;
	}
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
	assert( !g_listener.expired() );
	auto listener = g_listener.lock();
	if (listener->entered())
	{
		// -- Set attribute using default hook
		// __setattr__( name, value )
		setattrofunc func = static_cast< setattrofunc >( wrapped );
		const int res = (*func)(self, name, value);
		if (res < 0)
		{
			return nullptr;
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
		setattrofunc func = static_cast< setattrofunc >( wrapped );
		const int res = (*func)(self, name, value);
		if (res < 0)
		{
			return nullptr;
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
	setattrofunc func = static_cast< setattrofunc >( wrapped );
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
		return nullptr;
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


/**
 *	Table mapping the __setattr__ name to the tp_setattro offset and
 *	slot_tp_setattro wrapper function.
 *	Based on slotdefs from typeobject.c.
 *	TODO not sure what function does.
 */
static wrapperbase g_wrappers[] =
{
	{
		"__setattr__", // name of wrapper
		offsetof( PyTypeObject, tp_setattro ), // offset to tp_setattro slot
		slot_tp_setattro, // function
		wrap_setattr, // wrapper for function
		PyDoc_STR( "x.__setattr__('name', value) <==> x.name = value" ), // doc for wrapper
		0, // flags
		nullptr // name_strobj
	},
	{ nullptr } // Sentinel
};


const char * g_hookCountName = "__setattrHookCount";
PyScript::ScriptString g_pyHookCountName;

const char * g_originalSetattrName = "__originalSetattr";
PyScript::ScriptString g_pyOriginalSetattrName;
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


void attachListenerHooks( PyScript::ScriptObject & pythonObject )
{
#if ENABLE_PYTHON_LISTENER_HOOKS
	// __setattr__ hooks are added to the Python *type*, not the *instance*
	// So we must count how many reflected Python objects are using the type
	// Add an attribute to the object to track the number of reflected Python objects
	// Ignore errors if the attribute does not exist - first time hook is added
	auto typeObject = PyScript::ScriptType::getType( pythonObject );
	assert( typeObject.exists() );
	auto typeDict = typeObject.getDict();

	// Do this first time
	if (!g_pyHookCountName.exists())
	{
		g_pyHookCountName = PyScript::ScriptString( PyString_InternFromString( g_hookCountName ),
			PyScript::ScriptObject::FROM_NEW_REFERENCE );
		assert( g_pyHookCountName.exists() && "Out of memory interning listener hook names" );
	}
	// Do this first time
	if (g_pyOriginalSetattrName == nullptr)
	{
		g_pyOriginalSetattrName = PyScript::ScriptString( PyString_InternFromString( g_originalSetattrName ),
			PyScript::ScriptObject::FROM_NEW_REFERENCE );
		assert( g_pyOriginalSetattrName.exists() && "Out of memory interning listener hook names" );
	}

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
			const auto setSuccess = typeDict.setItem( g_pyHookCountName,
				hookCountLong,
				PyScript::ScriptErrorClear() );
			assert( setSuccess );

			return;
		}
	}

	// Add counter
	const long hookCount = 1;
	auto hookCountLong = PyScript::ScriptLong::create( hookCount );
	assert( hookCountLong.exists() );
	assert( hookCountLong.asLong() > 0 );
	const auto setSuccess1 = typeDict.setItem( g_pyHookCountName,
		hookCountLong,
		PyScript::ScriptErrorClear() );
	assert( setSuccess1 );

	// -- From typeobject.c add_operators
	auto & wrapperBase = g_wrappers[ 0 ];
	if (wrapperBase.name_strobj == nullptr)
	{
		wrapperBase.name_strobj = PyString_InternFromString( wrapperBase.name );
		assert( wrapperBase.name_strobj && "Out of memory interning listener hook names" );
	}

	auto wrapperName = PyScript::ScriptString( wrapperBase.name_strobj,
		PyScript::ScriptObject::FROM_BORROWED_REFERENCE );

	// Attach hook
	auto originalSetattr = typeDict.getItem( wrapperName,
		PyScript::ScriptErrorClear() );
	if (originalSetattr != nullptr)
	{
		assert( g_pyOriginalSetattrName.exists() );
		const auto setSuccess2 = typeDict.setItem( g_pyOriginalSetattrName,
			originalSetattr,
			PyScript::ScriptErrorClear() );
		assert( setSuccess2 );
	}

	auto pyType = reinterpret_cast< PyTypeObject * >( typeObject.get() );
	if( pyType->tp_setattro != static_cast< setattrofunc >( wrapperBase.function ) )
	{
		void * pFunctionToBeWrapped = pyType->tp_setattro;
		auto wrapperObject = PyScript::ScriptDescrWrapper::create( typeObject,
			wrapperBase,
			pFunctionToBeWrapped );
		assert( wrapperObject.exists() );
		const auto setSuccess3 = typeDict.setItem( wrapperObject.name(),
			wrapperObject,
			PyScript::ScriptErrorClear() );
		assert( setSuccess3 );

		// -- From typeobject.c update_one_slot
		pyType->tp_setattro = static_cast< setattrofunc >( wrapperBase.function );
	}

	typeObject.modified();
#endif // ENABLE_PYTHON_LISTENER_HOOKS
}


void detachListenerHooks( PyScript::ScriptObject & pythonObject )
{
#if ENABLE_PYTHON_LISTENER_HOOKS
	if (g_pHookContext == nullptr)
	{
		// Some other system is keeping a Python object after Python has
		// shut down
		return;
	}

	// __setattr__ hooks are added to the Python *type*, not the *instance*
	// So we must count how many reflected Python objects are using the type
	// Add an attribute to the object to track the number of reflected Python objects
	auto typeObject = PyScript::ScriptType::getType( pythonObject );
	auto typeDict = typeObject.getDict();

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
		const auto setSuccess = typeDict.setItem( g_pyHookCountName,
			hookCountLong,
			PyScript::ScriptErrorClear() );
		assert( setSuccess );

		return;
	}

	auto & wrapperBase = g_wrappers[ 0 ];
	assert( wrapperBase.name_strobj != nullptr );
	auto wrapperName = PyScript::ScriptString( wrapperBase.name_strobj,
		PyScript::ScriptObject::FROM_BORROWED_REFERENCE );

	assert( g_pyOriginalSetattrName.exists() );
	const auto originalSetattr = typeDict.getItem( g_pyOriginalSetattrName,
		PyScript::ScriptErrorClear() );

	const auto wrapperObject = typeDict.getItem( wrapperName,
		PyScript::ScriptErrorClear() );

	if (originalSetattr.exists())
	{
		const auto setSuccess = typeDict.setItem( wrapperName,
			originalSetattr,
			PyScript::ScriptErrorClear() );
		assert( setSuccess );

		const auto removeSuccess = typeDict.delItem( g_pyOriginalSetattrName,
			PyScript::ScriptErrorClear() );
		assert( removeSuccess );
	}
	else
	{
		const auto removeSuccess = typeDict.delItem( wrapperName,
			PyScript::ScriptErrorClear() );
		assert( removeSuccess );
	}

	assert( g_pyHookCountName.exists() );
	const auto removeSuccess = typeDict.delItem( g_pyHookCountName,
		PyScript::ScriptErrorClear() );
	assert( removeSuccess );

	// Restore old setattr
	auto pyType = reinterpret_cast< PyTypeObject * >( typeObject.get() );
	auto pyWrapper = reinterpret_cast< PyWrapperDescrObject * >( wrapperObject.get() );
	pyType->tp_setattro = static_cast< setattrofunc >( pyWrapper->d_wrapped );
	typeObject.modified();
#endif // ENABLE_PYTHON_LISTENER_HOOKS
}


} // namespace ReflectedPython
