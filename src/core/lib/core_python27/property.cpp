#include "pch.hpp"
#include "property.hpp"
#include "type_converters/converter_queue.hpp"
#include "type_converters/i_type_converter.hpp"

#include "core_dependency_system/depends.hpp"
#include "core_reflection/reflected_method_parameters.hpp"
#include "core_script/type_converter_queue.hpp"

#include "wg_pyscript/py_script_object.hpp"
#include "wg_types/hash_utilities.hpp"

#include "core_python27/defined_instance.hpp"

typedef TypeConverterQueue< PythonType::IConverter,
	PyScript::ScriptObject > PythonTypeConverters;


namespace ReflectedPython
{


typedef Depends< PythonTypeConverters > ImplementationDepends;
class Property::Implementation
	: public ImplementationDepends
{
public:
	Implementation( IComponentContext & context,
		const char * key,
		const PyScript::ScriptObject & pythonObject );

	Implementation( IComponentContext & context,
		const char * key,
		const TypeId & typeId,
		const PyScript::ScriptObject & pythonObject );

	bool setValue( const Variant & value );
	Variant getValue();


	// Need to store a copy of the string
	std::string key_;
	TypeId type_;
	PyScript::ScriptObject pythonObject_;
	uint64_t hash_;
};


Property::Implementation::Implementation( IComponentContext & context,
	const char * key,
	const PyScript::ScriptObject & pythonObject )
	: ImplementationDepends( context )
	, key_( key )
	, pythonObject_( pythonObject )
	, hash_( HashUtilities::compute( key_ ) )
{
	const auto attribute = pythonObject_.getAttribute( key_.c_str(),
		PyScript::ScriptErrorPrint() );
	assert( attribute.exists() );
	type_ = PythonType::scriptTypeToTypeId( attribute );
}


Property::Implementation::Implementation( IComponentContext & context,
	const char * key,
	const TypeId & typeId,
	const PyScript::ScriptObject & pythonObject )
	: ImplementationDepends( context )
	, key_( key )
	, type_( typeId )
	, pythonObject_( pythonObject )
	, hash_( HashUtilities::compute( key_ ) )
{
	// TODO: set a default value of type_ on the attribute
}


bool Property::Implementation::setValue( const Variant & value )
{
	auto pTypeConverters = get< PythonTypeConverters >();
	assert( pTypeConverters != nullptr );

	PyScript::ScriptObject scriptObject;
	const bool success = pTypeConverters->toScriptType( value, scriptObject );
	assert( success );
	PyScript::ScriptErrorPrint errorHandler;
	if (!pythonObject_.setAttribute( key_.c_str(),
		scriptObject,
		errorHandler ))
	{
		return false;
	}

	type_ = PythonType::scriptTypeToTypeId( scriptObject );
	return true;
}


Variant Property::Implementation::getValue()
{
	PyScript::ScriptErrorPrint errorHandler;

	// Get the attribute
	PyScript::ScriptObject attribute = pythonObject_.getAttribute(
		key_.c_str(),
		errorHandler );


	auto pTypeConverters = get< PythonTypeConverters >();
	assert( pTypeConverters != nullptr );

	Variant value;
	const bool success = pTypeConverters->toVariant( attribute, value );
	assert( success );
	return value;
}


Property::Property( IComponentContext & context,
	const char * key,
	const PyScript::ScriptObject & pythonObject )
	: IBaseProperty()
	, impl_( new Implementation( context, key, pythonObject ) )
{
}


Property::Property( IComponentContext & context,
	const char * key,
	const TypeId & typeId,
	const PyScript::ScriptObject & pythonObject )
	: IBaseProperty()
	, impl_( new Implementation( context, key, typeId, pythonObject ) )
{
}


const TypeId & Property::getType() const /* override */
{
	return impl_->type_;
}


const char * Property::getName() const /* override */
{
	return impl_->key_.c_str();
}


uint64_t Property::getNameHash() const /* override */
{
	return impl_->hash_;
}


MetaHandle Property::getMetaData() const /* override */
{
	return nullptr;
}


bool Property::readOnly() const /* override */
{
	// Python uses EAFP, so it can't check if a property is read-only before
	// trying to set it.
	// Have to try to set and check for an exception.
	// https://docs.python.org/2/glossary.html#term-eafp
	return false;
}


bool Property::isMethod() const /* override */
{
	// Get the attribute
	PyScript::ScriptErrorPrint errorHandler;
	PyScript::ScriptObject attribute = impl_->pythonObject_.getAttribute(
		impl_->key_.c_str(),
		errorHandler );
	assert( attribute.exists() );
	if (!attribute.exists())
	{
		return false;
	}

	// Checks if the attribute is "callable", it may be:
	// - an instance with a __call__ attribute
	// or
	// - a type with a tp_call member, such as
	// -- a method on a class
	// -- a function/lambda type
	return attribute.isCallable();
}


bool Property::isValue() const /* override */
{
	// Attribute must exist
	return true;
}


bool Property::set( const ObjectHandle & handle,
	const Variant & value,
	const IDefinitionManager & definitionManager ) const /* override */
{
	return impl_->setValue( value );
}


Variant Property::get( const ObjectHandle & handle,
	const IDefinitionManager & definitionManager ) const /* override */
{
	return impl_->getValue();
}


Variant Property::invoke( const ObjectHandle& object,
	const ReflectedMethodParameters& parameters ) /* override */
{
	const bool callable = this->isMethod();
	assert( callable );
	if (!callable)
	{
		return Variant();
	}

	auto pTypeConverters = impl_->get< PythonTypeConverters >();
	assert( pTypeConverters != nullptr );

	// Parse arguments
	auto tuple = PyScript::ScriptTuple::create( parameters.size() );
	size_t i = 0;

	for (auto itr = parameters.cbegin();
		(i < parameters.size()) && (itr != parameters.cend());
		++i, ++itr)
	{
		auto parameter = (*itr);
		PyScript::ScriptObject scriptObject;
		const bool success = pTypeConverters->toScriptType( parameter, scriptObject );
		assert( success );
		tuple.setItem( i, scriptObject );
	}

	PyScript::ScriptArgs args = PyScript::ScriptArgs( tuple.get(),
		PyScript::ScriptObject::FROM_BORROWED_REFERENCE );

	// Call method
	PyScript::ScriptErrorPrint errorHandler;
	const bool allowNullMethod = false;
	PyScript::ScriptObject returnValue = impl_->pythonObject_.callMethod(
		impl_->key_.c_str(),
		args,
		errorHandler,
		allowNullMethod );

	// Return value
	Variant result;

	if (returnValue.exists())
	{
		const bool success = pTypeConverters->toVariant( returnValue, result );
		assert( success );
	}

	return result;
}


size_t Property::parameterCount() const /* override */
{
	// Python arguments are passed together as a tuple
	// so just say the tuple is 1 argument
	// since the real number of arguments is unknown until the tuple is parsed
	//return this->isMethod() ? 1 : 0;
	if (!this->isMethod())
	{
		return 0;
	}


	PyScript::ScriptErrorPrint errorHandler;
	auto method = impl_->pythonObject_.getAttribute( impl_->key_.c_str(), errorHandler );
	if (!method.exists())
	{
		return 0;
	}

	auto func = method.get();
	// TODO other callable objects?
	if (!PyMethod_Check( func ))
	{
		return 0;
	}
	func = PyMethod_GET_FUNCTION(func);
	if (!PyFunction_Check( func ))
	{
		return 0;
	}
	auto code = (PyCodeObject *)PyFunction_GET_CODE(func);
	if (code == nullptr)
	{
		return 0;
	}

	if (code->co_argcount > 0)
	{
		// Subtract "self"
		// TODO classmethods
		return (code->co_argcount - 1);
	}

	if (code->co_flags & (CO_VARARGS | CO_VARKEYWORDS))
	{
		NGT_WARNING_MSG( "Variable arguments and keyword arguments are not "
			"supported by the reflection system\n" );
	}
	return 0;
}


} // namespace ReflectedPython

