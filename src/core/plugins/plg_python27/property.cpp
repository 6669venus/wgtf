#include "pch.hpp"
#include "property.hpp"


namespace ReflectedPython
{


namespace
{

Variant scriptObjectToVariant( PyScript::ScriptObject& object )
{
	// Get attribute as a string
	PyScript::ScriptErrorPrint errorHandler;
	PyScript::ScriptString str = object.str( errorHandler );
	const char * value = str.c_str();

	// Let variant convert string to type
	// Variant will create storage so don't worry about str going out of scope
	return Variant( value );
}

}


Property::Property( const char* key,
	PyScript::ScriptObject& pythonObject )
	: IBaseProperty()
	, key_( key )
	, pythonObject_( pythonObject )
{
}


const TypeId & Property::getType() const
{
	// See Property::get()
	// All types are returned as strings,
	// Variant can handle converting from string to the desired type
	return TypeId::getType< const char* >();
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
	return false;
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
	return (PyMethod_Check( attribute.get() ));

	// TODO NGT-1163 Function support
	//return attribute_.isCallable();
}


bool Property::set( const ObjectHandle & handle,
	const Variant & value,
	const IDefinitionManager & definitionManager ) const
{
	// TODO NGT-1162 Set support
	const std::string str = value.value< std::string >();
	assert( !str.empty() );

	PyScript::ScriptString scriptString = PyScript::ScriptString::create( str );

	//auto pObject = reflectedCast< ReflectedPython::DefinedInstance >( handle,
	//	definitionManager ).get();
	//assert( pObject != nullptr );

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
	assert( attribute.exists() );
	if (!attribute.exists())
	{
		return Variant();
	}

	return scriptObjectToVariant( attribute );
}


Variant Property::invoke( const ObjectHandle& object,
	const ReflectedMethodParameters& parameters )
{
	PyScript::ScriptArgs args = PyScript::ScriptArgs::none();
	PyScript::ScriptErrorPrint errorHandler;
	const bool allowNullMethod = false;
	PyScript::ScriptObject returnValue = pythonObject_.callMethod( key_.c_str(),
		args,
		errorHandler,
		allowNullMethod );

	return scriptObjectToVariant( returnValue );
}


size_t Property::parameterCount() const
{
	// TODO NGT-1163 Method support
	return 0;
}


} // namespace ReflectedPython

