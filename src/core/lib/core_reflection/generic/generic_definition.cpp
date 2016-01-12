#include "generic_definition.hpp"

#include "core_reflection/generic/generic_object.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/utilities/definition_helpers.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_reflection/object_handle.hpp"

const char * GenericDefinition::getName() const
{
	return name_.c_str();
}

//------------------------------------------------------------------------------
GenericDefinition::GenericDefinition( const char* name )
	:name_( name == nullptr
				? RefObjectId::generate().toString()
				: name )
{
}


//------------------------------------------------------------------------------
void GenericDefinition::init( IClassDefinitionModifier & modifier )
{
	modifier_ = &modifier;
}


//------------------------------------------------------------------------------
ObjectHandle GenericDefinition::create( const IClassDefinition & definition ) const
{
	auto pInst = std::unique_ptr< GenericObject >( new GenericObject() );
	pInst->setDefinition( const_cast< IClassDefinition * >( &definition ) );
	return ObjectHandle( std::move( pInst ), &definition );
}


//------------------------------------------------------------------------------
ObjectHandle GenericDefinition::createBaseProvider(
	const ReflectedPolyStruct & polyStruct ) const
{
	auto pType =
		ReflectionUtilities::dynamicCast< GenericObject >( polyStruct );
	return createBaseProvider( polyStruct.getDefinition(), pType );
}


//------------------------------------------------------------------------------
ObjectHandle GenericDefinition::createBaseProvider(
	const IClassDefinition & definition, const void * pThis ) const
{
	return ObjectHandle(
		static_cast< const GenericObject * >( pThis ), &definition );
}