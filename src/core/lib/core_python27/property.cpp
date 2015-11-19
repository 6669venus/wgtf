#include "pch.hpp"
#include "property.hpp"
#include "type_converters/i_type_converter.hpp"

#include "core_dependency_system/depends.hpp"
#include "core_reflection/reflected_method_parameters.hpp"
#include "core_script/type_converter_queue.hpp"

#include "wg_pyscript/py_script_object.hpp"


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

	// Need to store a copy of the string
	std::string key_;
	PyScript::ScriptObject pythonObject_;
};


Property::Implementation::Implementation( IComponentContext & context,
	const char * key,
	const PyScript::ScriptObject & pythonObject )
	: ImplementationDepends( context )
	, key_( key )
	, pythonObject_( pythonObject )
{
}

Property::Property( IComponentContext & context,
	const char * key,
	const PyScript::ScriptObject & pythonObject )
	: IBaseProperty()
	, impl_( new Implementation( context, key, pythonObject ) )
{
}


const TypeId & Property::getType() const /* override */
{
	// See Property::get()
	// All types are returned as strings,
	// Variant can handle converting from string to the desired type
	return TypeId::getType< const char * >();
}


const char * Property::getName() const /* override */
{
	return impl_->key_.c_str();
}


const MetaBase * Property::getMetaData() const /* override */
{
	return nullptr;
}


bool Property::readOnly() const /* override */
{
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


bool Property::set( const ObjectHandle & handle,
	const Variant & value,
	const IDefinitionManager & definitionManager ) const /* override */
{
	auto pTypeConverters = impl_->get< PythonTypeConverters >();
	assert( pTypeConverters != nullptr );

	PyScript::ScriptObject scriptObject;
	const bool success = pTypeConverters->toScriptType( value, scriptObject );
	assert( success );
	PyScript::ScriptErrorPrint errorHandler;
	return impl_->pythonObject_.setAttribute( impl_->key_.c_str(),
		scriptObject,
		errorHandler );
}


Variant Property::get( const ObjectHandle & handle,
	const IDefinitionManager & definitionManager ) const /* override */
{
	PyScript::ScriptErrorPrint errorHandler;

	// Get the attribute
	PyScript::ScriptObject attribute = impl_->pythonObject_.getAttribute(
		impl_->key_.c_str(),
		errorHandler );


	auto pTypeConverters = impl_->get< PythonTypeConverters >();
	assert( pTypeConverters != nullptr );

	Variant value;
	const bool success = pTypeConverters->toVariant( attribute, value );
	assert( success );
	return value;
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
		PyScript::ScriptString scriptString;
		const bool success = pTypeConverters->toScriptType( parameter,
			scriptString );
		assert( success );
		tuple.setItem( i, scriptString );
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
	const bool success = pTypeConverters->toVariant( returnValue, result );
	assert( success );
	return result;
}


size_t Property::parameterCount() const /* override */
{
	// Python arguments are passed together as a tuple
	// so just say the tuple is 1 argument
	// since the real number of arguments is unknown until the tuple is parsed
	return this->isMethod() ? 1 : 0;
}


} // namespace ReflectedPython

