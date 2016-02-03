#include "pch.hpp"
#include "defined_instance.hpp"

#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_reflection/interfaces/i_class_definition.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"
#include "interfaces/core_python_script/i_scripting_engine.hpp"
#include "i_script_object_definition_registry.hpp"
#include "property.hpp"


const char * SETATTR_NAME = "__setattr__";
const char * OLD_SETATTR_NAME = "__old_setattr__";

static int py_setattr_hook( PyObject * self, PyObject * name, PyObject * value )
{
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

	// Call through to original setattr
	auto selfObject = PyScript::ScriptObject( self,
		PyScript::ScriptObject::FROM_BORROWED_REFERENCE );

	auto typeObject = PyScript::ScriptType::getType( selfObject );

	auto args = PyScript::ScriptArgs::create( self, name, value );
	const bool allowNullMethod = false;
	auto result = typeObject.callMethod( OLD_SETATTR_NAME,
		args,
		PyScript::ScriptErrorPrint(),
		allowNullMethod );

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

	PyScript::ScriptErrorPrint errorPrint;
	//PyScript::ScriptModule module = PyScript::ScriptModule::import( "inspect",
	//	PyScript::ScriptErrorPrint( "Unable to import\n" ) );
	//auto isClassArgs = PyScript::ScriptArgs::create( pythonObject.get() );
	//const bool allowNullMethod = false;
	//auto isClass = module.callMethod( "isclass", isClassArgs, errorPrint, allowNullMethod );

	// Hook attribute listeners into object
	//if (PyScript::ScriptInstance::check( pythonObject ))
	//{
	//	// TODO old-style class
	//}
	//else if (PyScript::ScriptType::check( pythonObject ))
	//else if (isClass.exists() && isClass.isTrue( errorPrint ))
	//if (pythonObject.hasAttribute( SETATTR_NAME ))

	// TODO Just my test class
	if (strcmp( pythonObject.typeNameOfObject(), "NewClass" ) == 0)
	{
		auto pyType = pythonObject.get()->ob_type;
		const auto isAlreadyTracked = (pyType->tp_setattro == py_setattr_hook);

		if (!isAlreadyTracked)
		{
			auto typeObject = PyScript::ScriptType::getType( pythonObject );

			// Construct new hook
			static PyMethodDef s_methods[] =
			{
				{
					SETATTR_NAME,
					reinterpret_cast< PyCFunction >( &py_setattr_hook ),
					METH_VARARGS|METH_KEYWORDS,
					"Listener to notify the NGT Reflection System\n"
					"x.__setattr__('name', value) <==> x.name = value"
				},
				{ nullptr, nullptr, 0, nullptr }
			};

			auto pyFunction = PyCFunction_New( s_methods, pythonObject.get() );
			auto functionObject = PyScript::ScriptObject( pyFunction,
				PyScript::ScriptObject::FROM_NEW_REFERENCE );

			PyObject * self = nullptr;
			auto pyMethod = PyMethod_New( pyFunction, self, typeObject.get() );
			auto methodObject = PyScript::ScriptObject( pyMethod,
				PyScript::ScriptObject::FROM_NEW_REFERENCE );

			// Save old setattr
			auto oldSetattr = pythonObject.getAttribute( SETATTR_NAME,
				PyScript::ScriptErrorClear() );
			const auto hasOldAttr = oldSetattr.exists();

			// Hook uses old version
			assert( hasOldAttr );

			if (hasOldAttr)
			{
				const auto saved = typeObject.setAttribute( OLD_SETATTR_NAME,
					oldSetattr,
					errorPrint );
				assert( saved );

				// TODO work out a way to use a wrapper instead?
				pyType->tp_setattro = py_setattr_hook;
				PyType_Modified( pyType );
			}
		}
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
