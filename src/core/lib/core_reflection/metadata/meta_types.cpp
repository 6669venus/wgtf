#include "meta_types.hpp"
#include "meta_impl.hpp"
#include "../utilities/definition_helpers.hpp"

#define SET_UP_OBJECT( type, ctor_parameters ) \
	MetaBase * basePointer = new type ctor_parameters; \
	basePointer->setDefinitionName( getClassIdentifier< type >() );\
	return *basePointer;


//==============================================================================
MetaBase & MetaNone()
{
	SET_UP_OBJECT( MetaNoneObj, () )
}


//==============================================================================
MetaBase & MetaMinMax( float min, float max )
{
	SET_UP_OBJECT( MetaMinMaxObj, ( min, max ) )
}


//==============================================================================
MetaBase & MetaEnum( IEnumGenerator * enumGenerator )
{
	SET_UP_OBJECT( MetaEnumObj, ( enumGenerator ) )
}

//==============================================================================
MetaBase & MetaEnum( const wchar_t * enumString )
{
	SET_UP_OBJECT( MetaEnumObj, ( enumString ) )
}

//==============================================================================
MetaBase & MetaSlider()
{
	SET_UP_OBJECT( MetaSliderObj, () )
}


//==============================================================================
MetaBase & MetaGroup( const wchar_t * groupName )
{
	SET_UP_OBJECT( MetaGroupObj, ( groupName ) )
}


//==============================================================================
MetaBase & MetaAttributeDisplayName( const char * attributePath )
{
	SET_UP_OBJECT( MetaAttributeDisplayNameObj, ( attributePath ) )
}


//==============================================================================
MetaBase & MetaDisplayName( const wchar_t * displayName )
{
	SET_UP_OBJECT( MetaDisplayNameObj, ( displayName ) )
}


//==============================================================================
MetaBase & MetaPanelLayout( const char * layoutFile, const char * bindingsFile )
{
	SET_UP_OBJECT( MetaPanelLayoutObj, ( layoutFile, bindingsFile ) )
}


//==============================================================================
MetaBase & MetaNoNull()
{
	SET_UP_OBJECT( MetaNoNullObj, () )
}


//==============================================================================
MetaBase & MetaColor()
{
	SET_UP_OBJECT( MetaColorObj, () )
}


//==============================================================================
MetaBase & MetaStepSize(float stepSize)
{
	SET_UP_OBJECT(MetaStepSizeObj, ( stepSize ))
}


//==============================================================================
MetaBase & MetaDecimals(int decimals)
{
	SET_UP_OBJECT(MetaDecimalsObj, ( decimals ))
}


//==============================================================================
MetaBase & MetaHidden()
{
	SET_UP_OBJECT( MetaHiddenObj, () )
}


//==============================================================================
MetaBase & MetaThumbnail( int width, int height )
{
	SET_UP_OBJECT( MetaThumbnailObj, ( width, height ) )
}


//==============================================================================
MetaBase & MetaInPlace( const char * propertyName )
{
	SET_UP_OBJECT( MetaInPlaceObj, ( propertyName ) )
}


//==============================================================================
MetaBase & MetaSelected( const char * propertyName )
{
	SET_UP_OBJECT( MetaSelectedObj, ( propertyName ) )
}

//==============================================================================
MetaBase & MetaInPlacePropertyName( const char * propertyName )
{
	SET_UP_OBJECT( MetaInPlacePropertyNameObj, ( propertyName ) )
}

//==============================================================================
MetaBase & MetaCommandBase(
	const wchar_t * commandName, const IMetaCommandExecutable * commandExecutable )
{
	SET_UP_OBJECT( MetaCommandObj,  ( commandName, commandExecutable ) );
}

//==============================================================================
MetaBase & MetaNoSerialization()
{
	SET_UP_OBJECT( MetaNoSerializationObj, () )
}

//==============================================================================
MetaBase & MetaUniqueId( const char * id )
{
	SET_UP_OBJECT( MetaUniqueIdObj, ( id ) );
}

//==============================================================================
MetaBase & MetaOnStack()
{
	SET_UP_OBJECT( MetaOnStackObj, () );
}

MetaBase & MetaReadOnly()
{
	SET_UP_OBJECT( MetaReadOnlyObj, () );
}

MetaBase & MetaUrl()
{
	SET_UP_OBJECT( MetaUrlObj, () );
}
