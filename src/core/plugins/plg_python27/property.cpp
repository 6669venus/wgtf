#include "pch.hpp"
#include "property.hpp"


namespace ReflectedPython
{


Property::Property( const char* name,
	PyScript::ScriptObject& attribute )
	: IBaseProperty()
	, name_( name )
	, attribute_( attribute )
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
	return name_.c_str();
}


const MetaBase * Property::getMetaData() const
{
	return nullptr;
}


bool Property::readOnly() const
{
	// TODO NGT-1162 Set support
	return true;
}


bool Property::isMethod() const
{
	// TODO NGT-1163 Method support
	return (PyMethod_Check( attribute_.get() ));
	//return attribute_.isCallable();
}


bool Property::set( const ObjectHandle & handle,
	const Variant & value,
	const IDefinitionManager & definitionManager ) const
{
	// TODO NGT-1162 Set support
	assert( false && "The method or operation is not implemented." );
	return false;
}


Variant Property::get( const ObjectHandle & handle,
	const IDefinitionManager & definitionManager ) const
{
	// Get attribute as a string
	PyScript::ScriptErrorPrint errorHandler;
	PyScript::ScriptString str = attribute_.str( errorHandler );
	const char * value = str.c_str();

	// Let variant convert string to type
	// Variant will create storage so don't worry about str going out of scope
	return Variant( value );
}


Variant Property::invoke( const ObjectHandle& object,
	const ReflectedMethodParameters& parameters )
{
	// TODO NGT-1163 Method support
	assert( false && "The method or operation is not implemented." );
	return Variant();
}


size_t Property::parameterCount() const
{
	// TODO NGT-1163 Method support
	return 0;
}


} // namespace ReflectedPython

