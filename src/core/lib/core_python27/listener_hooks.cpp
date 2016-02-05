#include "pch.hpp"

#include "listener_hooks.hpp"

#include "defined_instance.hpp"

#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_reflection/interfaces/i_class_definition.hpp"
#include "core_script/type_converter_queue.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/property_accessor_listener.hpp"
#include "type_converters/i_type_converter.hpp"
#include "wg_pyscript/py_script_object.hpp"


typedef TypeConverterQueue< PythonType::IConverter,
	PyScript::ScriptObject > PythonTypeConverters;


IComponentContext * g_pHookContext = nullptr;


int py_setattr_hook( PyObject * self,
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

	// -- Pre-notify UI
	// TODO support preItemsInserted/postItemsInserted
	assert( g_pHookContext != nullptr );
	auto & context = (*g_pHookContext);
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

	// -- Post-notify UI
	for (auto it = itBegin; it != itEnd; ++it)
	{
		const auto & listener = (*it);
		listener.get()->postSetValue( propertyAccessor, variantValue );
	}

	return 0;
}


int py_instance_setattr_hook( PyInstanceObject * inst,
	PyObject * name,
	PyObject * v )
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

	// -- Pre-notify UI
	assert( g_pHookContext != nullptr );
	auto & context = (*g_pHookContext);
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

	// -- Post-notify UI
	for (auto it = itBegin; it != itEnd; ++it)
	{
		const auto & listener = (*it);
		listener.get()->postSetValue( propertyAccessor, variantValue );
	}

	return 0;
}

