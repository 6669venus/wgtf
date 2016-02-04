#include "pch.hpp"
#include "defined_instance.hpp"

#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_reflection/interfaces/i_class_definition.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"
#include "interfaces/core_python_script/i_scripting_engine.hpp"
#include "i_script_object_definition_registry.hpp"
#include "property.hpp"
#include "core_script/type_converter_queue.hpp"
#include "type_converters/i_type_converter.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/property_accessor_listener.hpp"

typedef TypeConverterQueue< PythonType::IConverter,
	PyScript::ScriptObject > PythonTypeConverters;

const char * SETATTR_NAME = "__setattr__";
const char * OLD_SETATTR_NAME = "__old_setattr__";
const char * REFLECTED_OBJECT_NAME = "__reflectedObject";

IComponentContext * g_context_ = nullptr;


// New-style classes
static int py_setattr_hook( PyObject * self, PyObject * name, PyObject * value )
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

	auto selfObject = PyScript::ScriptObject( self,
		PyScript::ScriptObject::FROM_BORROWED_REFERENCE );
	auto typeObject = PyScript::ScriptType::getType( selfObject );
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

	// -- Notify UI
	// TODO support preItemsInserted/postItemsInserted
	assert( g_context_ != nullptr );
	auto & context = (*g_context_);
	auto handle =
		ReflectedPython::DefinedInstance::create( context, selfObject );
	auto definedInstance = *handle.getBase< ReflectedPython::DefinedInstance >();

	auto pTypeConverters = context.queryInterface< PythonTypeConverters >();
	assert( pTypeConverters != nullptr );

	auto pDefinitionManager = context.queryInterface< IDefinitionManager >();
	assert( pTypeConverters != nullptr );

	Variant variantValue;
	const bool success = pTypeConverters->toVariant( valueObject, variantValue );
	assert( success );

	auto pDefinition = definedInstance.getDefinition();
	auto propertyAccessor = pDefinition->bindProperty( nameObject.c_str(),
		definedInstance );

	auto& listeners = pDefinitionManager->getPropertyAccessorListeners();
	const auto itBegin = listeners.cbegin();
	const auto itEnd = listeners.cend();
	for (auto it = itBegin; it != itEnd; ++it)
	{
		const auto & listener = (*it);
		listener.get()->preSetValue( propertyAccessor, variantValue );
	}

	// -- Set attribute on super class
	// superClass = super( ClassType, self )
	auto superArgs = PyScript::ScriptTuple::create( 2 );
	superArgs.setItem( 0, PyScript::ScriptType::getType( selfObject ) );
	superArgs.setItem( 1, selfObject );
	PyObject * superKWArgs = nullptr;
	auto pSuperClass = PyObject_Call(
		reinterpret_cast< PyObject * >( &PySuper_Type ),
		superArgs.get(),
		superKWArgs );
	if (pSuperClass == nullptr)
	{
		PyErr_Format( PyExc_AttributeError,
			"Cannot get super class." );
		return -1;
	}
	if (pSuperClass->ob_type == nullptr)
	{
		PyErr_Format( PyExc_AttributeError,
			"Cannot get super class." );
		return -1;
	}
	if (pSuperClass->ob_type->tp_setattro == nullptr)
	{
		PyErr_Format( PyExc_AttributeError,
			"Cannot get super class." );
		return -1;
	}

	// superClass.__setattr__( name, value )
	pSuperClass->ob_type->tp_setattro( self, name, value );

	// -- Notify UI
	for (auto it = itBegin; it != itEnd; ++it)
	{
		const auto & listener = (*it);
		listener.get()->postSetValue( propertyAccessor, variantValue );
	}

	return 0;
}


// Old-style classes
//@see instance_setattr(PyInstanceObject *inst, PyObject *name, PyObject *v)
static int py_instance_setattr_hook( PyInstanceObject * inst, PyObject * name, PyObject * v )
{
	// -- Check arguments
	if (name == nullptr)
	{
		PyErr_Format( PyExc_AttributeError,
			"Cannot set attribute with no name." );
		return -1;
	}
	if (v == nullptr)
	{
		PyErr_Format( PyExc_AttributeError,
			"Cannot set attribute with no value." );
		return -1;
	}

	auto selfObject = PyScript::ScriptObject( reinterpret_cast< PyObject * >( inst ),
		PyScript::ScriptObject::FROM_BORROWED_REFERENCE );
	if (!PyScript::ScriptInstance::check( selfObject ))
	{
		PyErr_Format( PyExc_TypeError,
			"Wrong type hook attacked to object." );
		return -1;
	}
	auto typeObject = PyScript::ScriptType::getType( selfObject );
	auto tmpName = PyScript::ScriptObject( name,
		PyScript::ScriptObject::FROM_BORROWED_REFERENCE );
	auto nameObject = PyScript::ScriptString::create( tmpName );
	if (!nameObject.exists())
	{
		PyErr_Format( PyExc_AttributeError,
			"Cannot set attribute with no name." );
		return -1;
	}
	auto valueObject = PyScript::ScriptObject( v,
		PyScript::ScriptObject::FROM_BORROWED_REFERENCE );

	// -- Notify UI
	assert( g_context_ != nullptr );
	auto & context = (*g_context_);
	auto handle =
		ReflectedPython::DefinedInstance::create( context, selfObject );
	auto definedInstance = *handle.getBase< ReflectedPython::DefinedInstance >();

	auto pTypeConverters = context.queryInterface< PythonTypeConverters >();
	assert( pTypeConverters != nullptr );

	auto pDefinitionManager = context.queryInterface< IDefinitionManager >();
	assert( pTypeConverters != nullptr );

	Variant variantValue;
	const bool success = pTypeConverters->toVariant( valueObject, variantValue );
	assert( success );

	auto pDefinition = definedInstance.getDefinition();
	auto propertyAccessor = pDefinition->bindProperty( nameObject.c_str(),
		definedInstance );

	auto& listeners = pDefinitionManager->getPropertyAccessorListeners();
	const auto itBegin = listeners.cbegin();
	const auto itEnd = listeners.cend();
	for (auto it = itBegin; it != itEnd; ++it)
	{
		const auto & listener = (*it);
		listener.get()->preSetValue( propertyAccessor, variantValue );
	}

	// -- Set attribute on dict
	// self.__dict__[ name ] = value
	auto dict = selfObject.getAttribute( "__dict__",
		PyScript::ScriptErrorPrint() );
	if (!dict.exists())
	{
		PyErr_Format( PyExc_AttributeError,
			"Cannot get __dict__." );
		return -1;
	}

	auto dictObject = PyScript::ScriptDict::create( dict );
	if (!dictObject.exists())
	{
		PyErr_Format( PyExc_AttributeError,
			"Cannot get __dict__." );
		return -1;
	}

	const auto result = dictObject.setItem( nameObject,
		valueObject,
		PyScript::ScriptErrorPrint() );
	if (!result)
	{
		PyErr_Format( PyExc_AttributeError,
			"Could not set value." );
		return -1;
	}

	// -- Notify UI
	for (auto it = itBegin; it != itEnd; ++it)
	{
		const auto & listener = (*it);
		listener.get()->postSetValue( propertyAccessor, variantValue );
	}

	return 0;
}


namespace ReflectedPython
{


DefinedInstance::DefinedInstance()
	: BaseGenericObject()
	, pythonObject_( nullptr )
	, pDefinition_( nullptr )
	, context_( nullptr )
{
	assert( false && "Always construct with a Python object" );
}


DefinedInstance::DefinedInstance(
	IComponentContext & context,
	const PyScript::ScriptObject & pythonObject,
	std::shared_ptr< IClassDefinition > & definition )
	: BaseGenericObject()
	, pythonObject_( pythonObject )
	, pDefinition_( definition )
	, context_( &context )
{
	g_context_ = context_;
	setDefinition( pDefinition_.get() );
}


DefinedInstance::~DefinedInstance()
{
}


/* static */ ObjectHandle DefinedInstance::create( IComponentContext & context,
	const PyScript::ScriptObject & pythonObject )
{
	assert( pythonObject.exists() );

	// Get a definition that's the same for each ScriptObject instance
	auto pRegistry = context.queryInterface< IScriptObjectDefinitionRegistry >();
	assert( pRegistry != nullptr );
	auto & registry = (*pRegistry);

	auto pDefinition = registry.getDefinition( pythonObject );
	assert( pDefinition != nullptr );
	auto & definition = (*pDefinition);

	const auto & id = registry.getID( pythonObject );

	// Search for an existing object handle that's using that definition
	auto pObjectManager = context.queryInterface< IObjectManager >();
	assert( pObjectManager != nullptr );
	auto & objectManager = (*pObjectManager);
	auto handle = objectManager.getObject( id );
	if (handle.isValid())
	{
		return handle;
	}

	// Existing object handle not found, construct a new instance
	auto pInstance = std::unique_ptr< ReflectedPython::DefinedInstance >(
		new DefinedInstance( context, pythonObject, pDefinition ) );
	ObjectHandleT< ReflectedPython::DefinedInstance > handleT(
		std::move( pInstance ),
		&definition );
	handle = handleT;

	// Register with IObjectManager to generate an ID
	// IObjectManager should take a weak reference
	handle = objectManager.registerObject( handle, id );
	assert( handle.isValid() );

	// Add hooks for listening to setattr and delattr
	// TODO delattr
	PyScript::ScriptErrorPrint errorPrint;
	auto typeObject = PyScript::ScriptType::getType( pythonObject );
	if (PyScript::ScriptInstance::check( pythonObject ))
	{
		auto pyType = pythonObject.get()->ob_type;
		const auto isAlreadyTracked = (pyType->tp_setattro ==
			reinterpret_cast< setattrofunc >( py_instance_setattr_hook ));

		if (!isAlreadyTracked)
		{

			// Construct new hook
			//static PyMethodDef s_methods[] =
			//{
			//	{
			//		SETATTR_NAME,
			//		reinterpret_cast< PyCFunction >( &py_instance_setattr_hook ),
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

			//// Save old setattr
			//auto oldSetattr = typeObject.getAttribute( SETATTR_NAME,
			//	PyScript::ScriptErrorClear() );
			//const auto hasOldAttr = oldSetattr.exists();

			//if (hasOldAttr)
			//{
			//	const auto saved = typeObject.setAttribute( OLD_SETATTR_NAME,
			//		oldSetattr,
			//		errorPrint );
			//	assert( saved );
			//}

			//const auto saved = pythonObject.setAttribute( REFLECTED_OBJECT_NAME,
			//	handle,
			//	PyScript::ScriptErrorPrint() );
			//assert( saved );

			// TODO work out a way to use a wrapper instead?
			// PyObject_GenericGetAttr?
			pyType->tp_setattro =
				reinterpret_cast< setattrofunc >( py_instance_setattr_hook );
			PyType_Modified( pyType );
		}
	}
	// Anything that inherits from object
	else if (typeObject.isSubClass( PyBaseObject_Type, errorPrint ))
	{
		auto pyType = pythonObject.get()->ob_type;
		const auto isAlreadyTracked = (pyType->tp_setattro == py_setattr_hook);

		if (!isAlreadyTracked)
		{
			//auto typeObject = PyScript::ScriptType::getType( pythonObject );

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

			//// Save old setattr
			//auto oldSetattr = pythonObject.getAttribute( SETATTR_NAME,
			//	PyScript::ScriptErrorClear() );
			//const auto hasOldAttr = oldSetattr.exists();

			//if (hasOldAttr)
			//{
			//	const auto saved = typeObject.setAttribute( OLD_SETATTR_NAME,
			//		oldSetattr,
			//		errorPrint );
			//	assert( saved );
			//}

			//const auto saved = pythonObject.setAttribute( REFLECTED_OBJECT_NAME,
			//	handle,
			//	PyScript::ScriptErrorPrint() );
			//assert( saved );

			// TODO work out a way to use a wrapper instead?
			// PyObject_GenericGetAttr?
			pyType->tp_setattro = py_setattr_hook;
			PyType_Modified( pyType );
		}
	}
	else
	{
		NGT_ERROR_MSG( "Unknown Python type %s\n", typeObject.str( errorPrint ).c_str() );
	}

	// Registered reference
	return handle;
}


const PyScript::ScriptObject & DefinedInstance::pythonObject() const
{
	return pythonObject_;
}


ObjectHandle DefinedInstance::getDerivedType() const
{
	auto pRegistry = context_->queryInterface< IScriptObjectDefinitionRegistry >();
	assert( pRegistry != nullptr );
	auto & registry = (*pRegistry);

	auto pObjectManager = context_->queryInterface< IObjectManager >();
	assert( pObjectManager != nullptr );
	auto & objectManager = (*pObjectManager);

	const auto & id = registry.getID( pythonObject_ );
	return objectManager.getObject( id );
}


ObjectHandle DefinedInstance::getDerivedType()
{
	auto pRegistry = context_->queryInterface< IScriptObjectDefinitionRegistry >();
	assert( pRegistry != nullptr );
	auto & registry = (*pRegistry);

	auto pObjectManager = context_->queryInterface< IObjectManager >();
	assert( pObjectManager != nullptr );
	auto & objectManager = (*pObjectManager);

	const auto & id = registry.getID( pythonObject_ );
	return objectManager.getObject( id );
}


} // namespace ReflectedPython
