#include "pch.hpp"
#include "property.hpp"

#include "core_reflection/reflected_method_parameters.hpp"
#include "core_script/type_converter_queue.hpp"
#include "type_converters/i_type_converter.hpp"


namespace ReflectedPython
{


Property::Property( const PythonTypeConverters & typeConverters,
	const char * key,
	PyScript::ScriptObject & pythonObject )
	: IBaseProperty()
	, typeConverters_( typeConverters )
	, key_( key )
	, pythonObject_( pythonObject )
{
}


const TypeId & Property::getType() const
{
	// See Property::get()
	// All types are returned as strings,
	// Variant can handle converting from string to the desired type
	return TypeId::getType< const char * >();
}


const char * Property::getName() const
{
	return key_.c_str();
}


const MetaBase * Property::getMetaData() const
{
	return nullptr;
}


bool Property::readOnly() const
{
	return pythonObject_.isAttributeSetDisabled( key_.c_str() );
}


bool Property::isMethod() const
{
	// Get the attribute
	PyScript::ScriptErrorPrint errorHandler;
	PyScript::ScriptObject attribute = pythonObject_.getAttribute( key_.c_str(),
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


bool Property::set( const ObjectHandle & handle,
	const Variant & value,
	const IDefinitionManager & definitionManager ) const
{
	PyScript::ScriptString scriptString;
	const bool success = typeConverters_.toScriptType( value, scriptString );
	assert( success );
	PyScript::ScriptErrorPrint errorHandler;
	return pythonObject_.setAttribute( key_.c_str(), scriptString, errorHandler );
}


Variant Property::get( const ObjectHandle & handle,
	const IDefinitionManager & definitionManager ) const
{
	PyScript::ScriptErrorPrint errorHandler;

	// Get the attribute
	PyScript::ScriptObject attribute = pythonObject_.getAttribute( key_.c_str(),
		errorHandler );

	Variant value;
	const bool success = typeConverters_.toVariant( attribute, value );
	assert( success );
	return value;
}


Variant Property::invoke( const ObjectHandle& object,
	const ReflectedMethodParameters& parameters )
{
	const bool callable = this->isMethod();
	assert( callable );
	if (!callable)
	{
		return Variant();
	}

	// Parse arguments
	auto tuple = PyScript::ScriptTuple::create( parameters.size() );
	size_t i = 0;
	for (auto itr = parameters.cbegin();
		(i < parameters.size()) && (itr != parameters.cend());
		++i, ++itr)
	{
		auto parameter = (*itr);
		PyScript::ScriptString scriptString;
		const bool success = typeConverters_.toScriptType( parameter,
			scriptString );
		assert( success );
		tuple.setItem( i, scriptString );
	}

	PyScript::ScriptArgs args = PyScript::ScriptArgs( tuple.get(),
		PyScript::ScriptObject::FROM_BORROWED_REFERENCE );

	// Call method
	PyScript::ScriptErrorPrint errorHandler;
	const bool allowNullMethod = false;
	PyScript::ScriptObject returnValue = pythonObject_.callMethod( key_.c_str(),
		args,
		errorHandler,
		allowNullMethod );

	// Return value
	Variant result;
	const bool success = typeConverters_.toVariant( returnValue, result );
	assert( success );
	return result;
}


size_t Property::parameterCount() const
{
	// Python arguments are passed together as a tuple
	// so just say the tuple is 1 argument
	// since the real number of arguments is unknown until the tuple is parsed
	return this->isMethod() ? 1 : 0;
}


} // namespace ReflectedPython

