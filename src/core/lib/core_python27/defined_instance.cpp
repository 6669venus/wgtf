#include "pch.hpp"

#include "defined_instance.hpp"
#include "listener_hooks.hpp"
#include "property.hpp"

#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_reflection/interfaces/i_class_definition.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"
#include "interfaces/core_python_script/i_scripting_engine.hpp"
#include "i_script_object_definition_registry.hpp"


const char * SETATTR_NAME = "__setattr__";
const char * OLD_SETATTR_NAME = "__old_setattr__";
const char * REFLECTED_OBJECT_NAME = "__reflectedObject";


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
